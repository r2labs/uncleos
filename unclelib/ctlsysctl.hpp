/* -*- mode: c++; c-basic-offset: 4; */
#ifndef __ctlsysctl__
#define __ctlsysctl__

/*! \addtogroup CppTest Test of cpp functionality
 * @{
 */

#include "criticalpp.hpp"

class ctlsys : public critical {
public:
    /*! Enable peripherals based on a GPIO_PORTx_BASE. */
    static void enable_periph(uint32_t sys_periph);
    static uint32_t timer_timeout_from_subtimer(uint32_t subtimer);
    static uint32_t gpio_pin_to_int(uint32_t pin);
    static uint32_t periph_to_int(uint32_t periph);
    static void gpio_int_clear(uint32_t base, uint32_t pin);
    static void gpio_int_disable(uint32_t base, uint32_t pin);
    static void gpio_int_enable(uint32_t base, uint32_t pin, bool clear_int = false);
    static void set_clock(void);
};

#endif

/* End Doxygen group
 * @}
 */
