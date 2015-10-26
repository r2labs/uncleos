/* -*- mode: c++; c-basic-offset: 4; */
/* Created by Hershal Bhave and Eric Crosson on <2015-03-15 Sun> */
/* Revision History: Look in Git FGT */

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
#include "uncleos/syscalls.h"

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
    uart0.atomic_printf("args: %s", args);
}

int8_t query_joint_pulse_width(char* args) {
    uint8_t jointnum = args[0]-'0';
    uart0.atomic_printf("1000\n");

    return 0;
}

int8_t set_joint_pulse_width(char* args) {
    uint8_t jointnum = args[0]-'0';
    uint32_t pw = args[3]*1000 + args[4]*100 + args[5]*10 + args[6];

    if (jointnum == 1) {
        blink->toggle(PIN_RED);
    } else if (jointnum == 2) {
        blink->toggle(PIN_BLUE);
    } else if (jointnum == 3) {
        blink->toggle(PIN_GREEN);
    }

    return 0;
}

int main(void) {

    ctlsys::set_clock();
    IntMasterDisable();

    blink = new blinker(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

    UART0_RX_BUFFER = buffer<char, UART0_RX_BUFFER_SIZE>(&UART0_RX_SEM);
    uart0 = uart(UART0_BASE, INT_UART0, &UART0_RX_BUFFER);

    shell0 = shell(&uart0);
    shell0.register_command("test", test_cmd);
    shell0.register_command("QP", query_joint_pulse_width);
    shell0.register_command("J", set_joint_pulse_width);

    os_threading_init();
    schedule(shell_handler, 200);
    schedule(toggle_blue, 200);
    os_launch();
}


extern "C" void __cxa_pure_virtual() { while (1) {} }
