/* -*- mode: c++; c-basic-offset: 4; */
/* Created by Hershal Bhave and Eric Crosson on <2015-10-28 Wednesday> */
/* Revision History: Look in Git FGT */

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"

#include "driverlib/interrupt.h"
#include "driverlib/uart.h"
#include "driverlib/pwm.h"
#include "driverlib/timer.h"

#include "unclelib/blinker.hpp"
#include "unclelib/ctlsysctl.hpp"
#include "unclelib/servo.hpp"
#include "unclelib/bufferpp.hpp"
#include "unclelib/semaphorepp.hpp"
#include "unclelib/shellpp.hpp"
#include "unclelib/switchpp.hpp"
#include "unclelib/timerpp.hpp"
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
    unsigned long operator()(const uint32_t& k) const {
        return k % 10;
    }
};
HashMap<uint32_t, uint32_t, MyKeyHash>* hmap;

blinker blink;
servo servos[5];
uart uart0;
shell shell0;
lswitch switch0;
semaphore sem_switch;
timer timer0;
uint8_t jointnum = 1;

uint32_t lerp(uint32_t x, uint32_t x_min, uint32_t x_max,
              uint32_t y_min, uint32_t y_max) {
    if (x > x_max) { return y_max; }
    else if (x < x_min) { return y_min; }
    return y_min + ((y_max - y_min)*(x - x_min))/(x_max - x_min);
}

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
    }
}

void shell_handler() {
    shell0.shell_handler();
}

int8_t set_joint_smooth_pulse_width(char* args) {
    uint8_t jointnum = 10*(args[0]-'0')+args[1]-'0';
    uint32_t pw = (args[3]-'0')*1000 + (args[4]-'0')*100 + (args[5]-'0')*10 + (args[6]-'0');
    servos[jointnum].set_smooth(pw);
    return 0;
}

int8_t set_joint_pulse_width(char* args) {
    uint8_t jointnum = 10*(args[0]-'0')+args[1]-'0';
    uint32_t pw = (args[3]-'0')*1000 + (args[4]-'0')*100 + (args[5]-'0')*10 + (args[6]-'0');
    servos[jointnum].set(pw);
    return 0;
}

int8_t set_joint_discrete(char* args) {
    uint8_t jointnum = 10*(args[0]-'0')+args[1]-'0';
    uint32_t pw = args[3] == 'H' ? servos[jointnum].max_duty : servos[jointnum].min_duty;
    servos[jointnum].set(pw);
    return 0;
}

int8_t query_joint_pulse_width(char* args) {
    uint8_t jointnum = 10*(args[0]-'0')+args[1]-'0';
    uint32_t num = servos[jointnum].get();
    uart0.atomic_printf("%d\n", num);
    return 0;
}

int8_t query_all_joints(char* args) {
    for (uint8_t i=0; i<5; ++i) {
        if (jointnum == i) {
            uart0.atomic_printf("* %d: %d us, %d deg\n", i, servos[i].get(),
                                lerp(servos[i].get(),
                                     servos[i].min_duty,
                                     servos[i].max_duty, 0, 180));
        } else {
            uart0.atomic_printf("  %d: %d us, %d deg\n", i, servos[i].get(),
                                lerp(servos[i].get(),
                                     servos[i].min_duty,
                                     servos[i].max_duty, 0, 180));
        }
    }
    return 0;
}

int8_t set_joint_twiddler(char* args) {
    int num = args[0]-'0';
    if (num >= 5) {
        return 1;
    }
    jointnum = num;
    return 0;
}

int8_t goto_rest(char* args) {
    for (uint8_t i=0; i<5; ++i) {
        servos[i].force(servos[i].rest_duty);
    }
    return 0;
}

int8_t set_estimated_angle(char* args) {
    uint8_t jointnum = args[0]-'0';
    int32_t angle = (args[2]-'0')*100 + (args[3]-'0')*10 + (args[4]-'0');
    uint32_t computed_pw = lerp(angle, 0, 180,
                                servos[jointnum].min_duty,
                                servos[jointnum].max_duty);
    servos[jointnum].set(computed_pw);
    uart0.atomic_printf("%d, %d\n", angle, servos[jointnum].get());
    return 0;
}

extern "C" void GPIOPortF_Handler() {
    switch0.ack();
    switch0.debounce();
}

extern "C" void Timer0A_Handler() {

    blink.toggle(PIN_GREEN);
    for (int i=0; i<5; ++i) {
        servos[i].step();
    }
    timer0.ack();
}

extern "C" void Timer1A_Handler() {
    switch0.end_debounce();
}

void switch_responder() {
    while(1) {
        if(sem_switch.guard() && switch0.debounced_data) {
            if(switch0.debounced_data == BUTTON_LEFT) {
                blink.blink(PIN_RED);
                servos[jointnum].force(servos[jointnum].get() + 1);
            } else if(switch0.debounced_data == BUTTON_RIGHT) {
                servos[jointnum].force(servos[jointnum].get() - 1);
                blink.blink(PIN_BLUE);
            }
            uart0.atomic_printf("%d: %d us, %d deg\n", jointnum, servos[jointnum].get(),
                                lerp(servos[jointnum].get(),
                                     servos[jointnum].min_duty,
                                     servos[jointnum].max_duty, 0, 180));
        }
        os_surrender_context();
    }
}

int main(void) {

    ctlsys::set_clock();
    IntMasterDisable();

    UART0_RX_BUFFER = buffer<char, UART0_RX_BUFFER_SIZE>(&UART0_RX_SEM);
    uart0 = uart(UART0_BASE, INT_UART0, &UART0_RX_BUFFER);

    servos[0] = servo(PWM0_BASE, PWM_GEN_0, PWM_OUT_0, 600, 2500, 1500, 1500);
    servos[1] = servo(PWM0_BASE, PWM_GEN_0, PWM_OUT_1, 600, 2400, 1945, 1500);
    servos[2] = servo(PWM0_BASE, PWM_GEN_1, PWM_OUT_2, 500, 2100, 1860, 1500);
    servos[3] = servo(PWM0_BASE, PWM_GEN_2, PWM_OUT_4, 500, 2550, 1500, 1500);
    servos[4] = servo(PWM0_BASE, PWM_GEN_1, PWM_OUT_3, 1000, 2500, 1000, 1500);

    for (int8_t i=0; i<5; ++i) {
        servos[i].start();
    }

    shell0 = shell(&uart0);
    shell0.register_command("QP", query_joint_pulse_width);
    shell0.register_command("J", set_joint_smooth_pulse_width);
    shell0.register_command("S", set_joint_pulse_width);
    shell0.register_command("D", set_joint_discrete);
    shell0.register_command("T", set_joint_twiddler);
    shell0.register_command("E", set_estimated_angle);
    shell0.register_command("A", query_all_joints);
    shell0.register_command("R", goto_rest);

    blink = blinker(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

    switch0 = lswitch(GPIO_PORTF_BASE, BUTTONS_BOTH,
                      &sem_switch, 1, TIMER_A, GPIO_BOTH_EDGES,
                      INT_GPIOF_TM4C123, true);

    timer0 = timer(0, TIMER_A, TIMER_CFG_PERIODIC, SysCtlClockGet() / 1000,
                   ctlsys::timer_timeout_from_subtimer(TIMER_A));

    timer0.start();

    os_threading_init();
    schedule(shell_handler, 200);
    schedule(switch_responder, 200);
    os_launch();
}


extern "C" void __cxa_pure_virtual() { while (1) {} }
