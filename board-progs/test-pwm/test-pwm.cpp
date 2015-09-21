/* -*- mode: c++; c-basic-offset: 4; */
/* Created by Hershal Bhave on [2015-09-20 Sun] */
/* Revision History: Look in Git FGT */

#include <stdint.h>

#include "unclelib/blinker.hpp"
#include "unclelib/bufferpp.hpp"
#include "unclelib/ctlsysctl.hpp"
#include "unclelib/math.hpp"
#include "unclelib/motorpp.hpp"
#include "unclelib/shellpp.hpp"
#include "unclelib/switchpp.hpp"
#include "unclelib/uartpp.hpp"

#include "uncleos/os.h"
#include "uncleos/schedule.h"

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/uart.h"
#include "driverlib/pwm.h"

#include "inc/hw_memmap.h"

extern "C" void __cxa_pure_virtual() { while (1) {} }

#define UART0_RX_BUFFER_SIZE 8
static semaphore UART0_RX_SEM;
static buffer<char, UART0_RX_BUFFER_SIZE> UART0_RX_BUFFER;

uint16_t duty_cycle;
blinker blink;
uart uart0;
shell shell0;

motor motor0, motor1;
semaphore motor_start, motor_stop;

lswitch switch0;
semaphore sem_switch;

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
        blink.blink(PIN_RED);
    }
}

extern "C" void GPIOPortF_Handler() {
    switch0.ack();
    switch0.debounce();
}

extern "C" void Timer1A_Handler() {

    switch0.end_debounce();
}

void switch_responder() {

    const uint32_t counter_max = SysCtlClockGet()/100;
    uint32_t pins, counter;
    uint16_t left_speed, right_speed;

    while(1) {
        if(sem_switch.guard() && switch0.debounced_data) {
            counter = 0;

            if(switch0.debounced_data == BUTTON_LEFT) {
                blink.toggle(PIN_RED);
                duty_cycle = clamp(duty_cycle-1, 0, 200);
                motor0.set(duty_cycle*motor::pwm_max_period/200);
            } else if(switch0.debounced_data == BUTTON_RIGHT) {
                blink.toggle(PIN_BLUE);
                duty_cycle = clamp(duty_cycle+1, 0, 200);
                motor0.set(duty_cycle*motor::pwm_max_period/200);
            }
            uart0.atomic_printf("%i\n", duty_cycle/2);

            /* while (++counter < counter_max) { } */
        }
        os_surrender_context();
    }
}

void shell_handler() {

    while(1) {
        if(UART0_RX_SEM.guard()) {

            bool ok;
            char recv = UART0_RX_BUFFER.get(ok);

            if(ok) { shell0.accept(recv); }
        }
        os_surrender_context();
    }
}

int main(void) {

    ctlsys::set_clock();
    IntMasterDisable();

    blink = blinker(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

    UART0_RX_BUFFER = buffer<char, UART0_RX_BUFFER_SIZE>(&UART0_RX_SEM);
    uart0 = uart(UART0_BASE, INT_UART0);

    duty_cycle = 15;

    shell0 = shell(&uart0, &motor_start, &motor_stop);
    motor0 = motor(GPIO_PORTA_BASE, GPIO_PIN_6, PWM0_BASE, PWM_GEN_0, PWM_OUT_0);
    motor1 = motor(GPIO_PORTA_BASE, GPIO_PIN_7, PWM0_BASE, PWM_GEN_0, PWM_OUT_1);

    motor0.set(duty_cycle*motor::pwm_max_period/200);
    motor0.start();

    switch0 = lswitch(GPIO_PORTF_BASE, BUTTONS_BOTH,
                      &sem_switch, 1, TIMER_A, GPIO_BOTH_EDGES,
                      INT_GPIOF_TM4C123, true);

    os_threading_init();
    schedule(shell_handler, 200);
    /* schedule(thread_uart_update, 200); */
    schedule(switch_responder, 200);
    os_launch();
}
