/* -*- mode: c; c-basic-offset: 4; -*- */
/* Created by Hershal Bhave and Eric Crosson 2015-10-04 */
/* Revision history: Look in Git FGT */
#ifndef __syscalls__
#define __syscalls__

/*! \addtogroup
   * @{
    */

#include <sys/stat.h>

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
    int _kill(int pid, int sig)
    {
        static_cast<void>(pid);
        static_cast<void>(sig);
        return -1;
    }

    // This stub function is required by stdlib
    int _getpid(void) {
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

#endif

/* End Doxygen group
    * @}
     */
