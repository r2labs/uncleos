/* -*- mode: c; c-basic-offset: 4; -*- */
/* Created by Hershal Bhave and Eric Crosson 2015-03-01 */
/* Revision history: Look in Git FGT */
#ifndef __systick__
#define __systick__

/*! \addtogroup
 * @{
 */

#include "nexus.h"

/* Forward declarations */
extern void SysTickPeriodSet(uint32_t ui32Period);
extern uint32_t SysCtlClockGet(void);
extern void SysTickEnable(void);
extern void SysTickIntEnable(void);

/*! Provide a human-readable way to initialize the SysTick. */
always static inline
void systick_init(frequency_t frequency) {
    SysTickPeriodSet(SysCtlClockGet() / frequency);
    SysTickEnable();
    SysTickIntEnable();
}

#endif

/* End Doxygen group
 * @}
 */
