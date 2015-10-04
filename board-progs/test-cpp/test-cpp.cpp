/* -*- mode: c++; c-basic-offset: 4; */
/* Created by Hershal Bhave and Eric Crosson on <2015-03-15 Sun> */
/* Revision History: Look in Git FGT */

#include <sys/stat.h>

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"

#include "driverlib/interrupt.h"
#include "driverlib/uart.h"

#include "unclelib/ctlsysctl.hpp"
#include "unclelib/blinker.hpp"
#include "unclelib/bufferpp.hpp"
#include "unclelib/semaphorepp.hpp"
#include "unclelib/shellpp.hpp"
#include "unclelib/uartpp.hpp"

#include "uncleos/nexus.h"
#include "uncleos/os.h"
#include "uncleos/schedule.h"

#include "unclelib/hashmap.hpp"

#define UART0_RX_BUFFER_SIZE 32
static semaphore UART0_RX_SEM;
static buffer<char, UART0_RX_BUFFER_SIZE> UART0_RX_BUFFER;
struct MyKeyHash {
    unsigned long operator()(const uint32_t& k) const
    {
        return k % 10;
    }
};
HashMap<uint32_t, uint32_t, MyKeyHash>* hmap;

blinker* blink;
uart uart0;
shell shell0;

extern "C" void UART0_Handler(void) {

    if(!(uart0.ack() & (UART_INT_RX | UART_INT_RT))) {
        return;
    }

    while(UARTCharsAvail(UART0_BASE)) {
        char recv = uart0.get_char();

        /* Regardless of newline received, our convention is to
         * mark end-of-lines in a buffer with the CR character. */
        switch(recv) {
        case '\n':
            if (uart::LAST_WAS_CR) {
                uart::LAST_WAS_CR = false;
                continue;
            }
            break;
        case '\r':
            uart::LAST_WAS_CR = true;
            break;
        case 0x1b:
            recv = '\r';
            break;
        default: break;
        }
        UART0_RX_BUFFER.notify((const int8_t) recv);
        blink->blink(PIN_RED);
    }
}

extern "C" {
    int _close(int file) { return -1; }

    int _fstat(int file, struct stat *st) {
        st->st_mode = S_IFCHR;
        return 0;
    }

    int _isatty(int file) { return 1; }

    int _lseek(int file, int ptr, int dir) { return -1; }

    int _open(const char *name, int flags, int mode) { return -1; }

    int _read(int file, char *ptr, int len) {
        return -1;
    }

    // This stub function is required by stdlib
    extern "C" int _kill(int pid, int sig)
    {
        static_cast<void>(pid);
        static_cast<void>(sig);
        return -1;
    }

    // This stub function is required by stdlib
    extern "C" int _getpid(void) {
        return 1;
    }

    /* // This stub function is required by stdlib */
    /* extern "C" void _exit() */
    /* { */
    /*     exit(-1); */
    /* } */

    char *_heap_end = 0;
    caddr_t _sbrk(int incr) {
        extern char _heap_bottom; /* Defined by the linker */
        extern char _heap_top; /* Defined by the linker */
        char *prev__heap_end;

        if (_heap_end == 0) {
            _heap_end = &_heap_bottom;
        }
        prev__heap_end = _heap_end;

        if (_heap_end + incr > &_heap_top) {
            /* Heap and stack collision */
            return (caddr_t)0;
        }

        _heap_end += incr;
        return (caddr_t) prev__heap_end;
    }

    int _write(int file, char *ptr, int len) {

        return len;
    }
}

void toggle_blue() {

    while (1) {
        blink->toggle(PIN_BLUE);
        os_surrender_context();
    }
}

void shell_handler() {
    shell0.shell_handler();
}

int8_t test_cmd(char* args) {
    uart0.atomic_printf("%s", args);
}
int main(void) {

    ctlsys::set_clock();
    IntMasterDisable();

    blink = new blinker(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

    char one[] = {'o','n','e'};
    char two[] = {'t','w','o'};
    char three[] = {'t','h','r','e','e'};

    /* hmap = new HashMap<uint32_t, uint32_t, MyKeyHash>(); */
    /* hmap->put(SuperFastHash(one, ustrlen(one)), 2); */
    /* hmap->put(SuperFastHash(two, ustrlen(two)), 3); */
    /* hmap->put(SuperFastHash(three, ustrlen(three)), 4); */
    /* hmap->put(2, 123); */

    /* uint32_t vals[4]; */
    /* bool status = 0; */
    /* status |= hmap->get(SuperFastHash(one, ustrlen(one)), vals[0]); */
    /* status |= hmap->get(SuperFastHash(two, ustrlen(two)), vals[1]); */
    /* status |= hmap->get(SuperFastHash(three, ustrlen(three)), vals[2]); */
    /* status |= hmap->get(2, vals[3]); */

    UART0_RX_BUFFER = buffer<char, UART0_RX_BUFFER_SIZE>(&UART0_RX_SEM);
    uart0 = uart(UART0_BASE, INT_UART0, &UART0_RX_BUFFER);

    shell0 = shell(&uart0);
    shell0.register_command("test", test_cmd);

    os_threading_init();
    schedule(shell_handler, 200);
    schedule(toggle_blue, 200);
    os_launch();
}


extern "C" void __cxa_pure_virtual() { while (1) {} }
