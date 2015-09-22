#ifndef __criticalpp__
#define __criticalpp__

#include <stdint.h>
#include <stdbool.h>

class critical {
public:
    static inline uint32_t StartCritical(void) {
        uint32_t msk;
        asm("MRS    %[mask], PRIMASK" : [mask] "=r" (msk));
        asm("CPSID  I");
        return msk;
    }

    static inline void EndCritical(uint32_t primask) {
        asm("MSR    PRIMASK, %[mask]" : [mask]"=r"(primask));
    }
};

#endif
