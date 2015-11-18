/* -*- mode: c++; c-basic-offset: 4; -*- */
#include <stdint.h>

#include "ctlsysctl.hpp"
#include "math.hpp"
#include "servo.hpp"

#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"

#include "inc/hw_memmap.h"

servo::servo() {}

servo::servo(memory_address_t pwm_base, memory_address_t pwm_gen,
             memory_address_t pwm_out, uint32_t min, uint32_t max,
             uint32_t rest, uint32_t ms_smooth) {
    this->pwm_base = pwm_base;
    this->pwm_gen = pwm_gen;
    this->pwm_out = pwm_out;

    this->min_duty = min;
    this->max_duty = max;
    this->rest_duty = rest;

    this->ms_smooth = ms_smooth;

    ctlsys::enable_periph(pwm_base);

    pwm_init();
}

uint32_t servo::force(uint32_t pw) {
    pw = set(pw, true);
    start_duty = pw;
    end_duty = pw;
    return pw;
}

void servo::set_smooth(uint32_t pw) {
    if (pw != end_duty) {
        start_duty = current_duty;
        ms_counter = 0;
    }

    end_duty = pw;
}

void servo::step() {

    if (ms_counter < ms_smooth && (current_duty != end_duty)) {
        set(lerp(ms_counter, 0, ms_smooth, (int32_t)start_duty, (int32_t)end_duty));
        ++ms_counter;
    }
}

uint32_t servo::set(uint32_t pw, bool force) {
    if (!force) { pw = clamp(pw, min_duty, max_duty); }
    current_duty = pw;
    PWMPulseWidthSet(pwm_base, pwm_out, pw*clock_div);
    return current_duty;
}

void servo::start() {
    PWMGenEnable(pwm_base, pwm_gen);
    force(rest_duty);
}

void servo::stop() {
    PWMGenDisable(pwm_base, pwm_gen);
}

void servo::pwm_init() {
    uint32_t status = StartCritical();

    SysCtlPWMClockSet(SYSCTL_PWMDIV_8);
    /* clock_div is directly related to SYSCTL_PWMDIV_8 */
    clock_div = 2;

    PWMGenConfigure(pwm_base, pwm_gen, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenPeriodSet(pwm_base, pwm_gen, pwm_max_period);
    PWMGenEnable(pwm_base, pwm_gen);

    uint32_t out_bit;
    uint32_t gpio_base;
    uint32_t gpio_pin;
    uint32_t gpio_configuration;

    switch(pwm_out) {
    case PWM_OUT_0:
        gpio_base = GPIO_PORTB_BASE;
        gpio_configuration = GPIO_PB6_M0PWM0;
        gpio_pin = GPIO_PIN_6;
        out_bit = PWM_OUT_0_BIT;
        break;
    case PWM_OUT_1:
        gpio_base = GPIO_PORTB_BASE;
        gpio_configuration = GPIO_PB7_M0PWM1;
        gpio_pin = GPIO_PIN_7;
        out_bit = PWM_OUT_1_BIT;
        break;
    case PWM_OUT_2:
        gpio_base = GPIO_PORTB_BASE;
        gpio_configuration = GPIO_PB4_M0PWM2;
        gpio_pin = GPIO_PIN_4;
        out_bit = PWM_OUT_2_BIT;
        break;
    case PWM_OUT_3:
        gpio_base = GPIO_PORTB_BASE;
        gpio_configuration = GPIO_PB5_M0PWM3;
        gpio_pin = GPIO_PIN_5;
        out_bit = PWM_OUT_3_BIT;
        break;
    case PWM_OUT_4:
        gpio_base = GPIO_PORTE_BASE;
        gpio_configuration = GPIO_PE4_M0PWM4;
        gpio_pin = GPIO_PIN_4;
        out_bit = PWM_OUT_4_BIT;
        break;
    default:
        while(1) {}
    }

    /* Enable the outputs. */
    ctlsys::enable_periph(gpio_base);
    GPIOPinConfigure(gpio_configuration);
    GPIOPinTypePWM(gpio_base, gpio_pin);
    PWMOutputState(pwm_base, out_bit, true);

    EndCritical(status);
}

uint32_t servo::get() {
    return current_duty;
}
