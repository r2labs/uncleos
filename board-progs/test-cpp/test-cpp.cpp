/* -*- mode: c++; c-basic-offset: 4; */
/* Created by Hershal Bhave and Eric Crosson on <2015-03-15 Sun> */
/* Revision History: Look in Git FGT */

#include <sys/stat.h>

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"

#include "driverlib/interrupt.h"

#include "unclelib/ctlsysctl.hpp"
#include "unclelib/blinker.hpp"

blinker* blink;

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

int main(void) {

    ctlsys::set_clock();
    IntMasterDisable();

    blink = new blinker(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

    char* a = (char*)malloc(5*sizeof(char));

    blink->toggle(PIN_RED);
    a[0] = 10;
    a[1] = 'a';
    a[2] = 0xff;

    while(1) {
        blink->toggle(PIN_BLUE);
    }
}


extern "C" void __cxa_pure_virtual() { while (1) {} }
