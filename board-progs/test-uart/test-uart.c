/* -*- mode: c; c-basic-offset: 4; -*- */
/* Created by Hershal Bhave and Eric Crosson on 2015-01-24 */
/* Revision History: Look in Git FGT */

/* Standard Libs */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* TI Includes */
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"

/* Driverlib Includes */
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"

#include "libuart/uart.h"
#include "libheart/heartbeat.h"

#include <sys/stat.h>

/*! Read the next character from the UART and write it back to the UART.
 *  \return void
 */
void UART0_Handler(void) {

    uint32_t ui32Status;

    /* Loop while there are characters in the receive FIFO. */
    while(UARTCharsAvail(UART0_BASE)) {
        /* Read the next character from the UART and write it back to the UART. */
        /* uart_send_char(uart_get_char()); */
        char* received_string = uart_get_string(1);
        uart_send_char(received_string[0]);
        /* free(received_string); */

        /* Blink the LED to show a character transfer is occuring. */
	heart_beat();

        /* Delay for 1 millisecond.  Each SysCtlDelay is about 3 clocks. */
        SysCtlDelay(SysCtlClockGet() / (1000 * 3));

        /* Turn off the LED */
        heart_off();
    }
}

/*! Accept input on UART 0, and parrot input back out to UART 0.
 * \return Exit status
 */
int main(void) {

    FPUEnable();
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);

    /* Enable the GPIO port that is used for the on-board LED. */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    /* Enable the GPIO pins for the LED (PF2). */
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);

    /* Enable the peripherals used by this example. */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    /* Enable processor interrupts. */
    IntMasterEnable();

    /* Set the active uart channel */
    uart_set_active_channel(UART0_BASE);

    /* Set GPIO A0 and A1 as UART pins. */
    uart_init();

    /* Prompt for text to be entered. */
    uart_send_string("Enter text:");

   /* Postpone death */
    while (1) {}
}
