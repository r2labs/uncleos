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

#include "uncleos/nexus.h"

#include "hashmap.hpp"
#include "strhash.hpp"

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

struct MyKeyHash {
    unsigned long operator()(const int& k) const
    {
        return k % 10;
    }
};

int main(void) {

    ctlsys::set_clock();
    IntMasterDisable();

    blink = new blinker(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

    char one[] = {'o','n','e'};
    char two[] = {'t','w','o'};
    char three[] = {'t','h','r','e','e'};

    HashMap<uint32_t, uint32_t, MyKeyHash> hmap;
    hmap.put(0, SuperFastHash(one, ustrlen(one)));
    hmap.put(1, SuperFastHash(two, ustrlen(two)));
    hmap.put(2, SuperFastHash(three, ustrlen(three)));
    hmap.put(3, 2);

    uint32_t vals[4];
    bool status = 0;
    status |= hmap.get(0, vals[0]);
    status |= hmap.get(1, vals[1]);
    status |= hmap.get(2, vals[2]);
    status |= hmap.get(3, vals[3]);

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
