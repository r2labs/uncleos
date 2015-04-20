#include "uartpp.hpp"

#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"

#include "driverlib/gpio.h"
#include "driverlib/uart.h"

uint32_t ustrlen(const char* s) {
    uint32_t len = 0;
    while(s[len]) { ++len; }
    return(len);
}

uart::uart() {}

/* \warning currently only allows uart from GPIO_PORTA on the TM4C123GXL */
uart::uart(uint32_t uart_baud_rate, memory_address_t uart_channel,
           memory_address_t uart_interrupt) {

    this->baud_rate = uart_baud_rate;
    this->channel = uart_channel;
    this->interrupt = uart_interrupt;

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0 +
                           (uart_channel - UART0_BASE) / 0x1000);

    /* Enable uart pins */
    /* todo: parametrize */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    UARTConfigSetExpClk(channel, SysCtlClockGet(), baud_rate,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                         UART_CONFIG_PAR_NONE));

    enable();
}

void uart::send_char(const char ch) {

    UARTCharPut(channel, ch);
}

void uart::send_string(const char* str) {

    uint32_t len = ustrlen(str);
    char* ptr = (char*)str;

    while(len--) {
        while(!UARTSpaceAvail(channel)) {}
        send_char(*(ptr++));
    }
}

void uart::send_newline(void) {

    send_string("\r\n");
}

char uart::get_char(void) {

    return UARTCharGet(channel);
}

void uart::ack(void) {

    uint32_t ui32Status;
    ui32Status = UARTIntStatus(channel, true);
    UARTIntClear(channel, ui32Status);
}

char* uart::get_string(const uint32_t length) {

    uint32_t remaining_chars = (uint32_t) length;

    ack();

    while(UARTCharsAvail(channel) && (remaining_chars > 0)) {
        buffer[remaining_chars-- - length] = get_char();
    }
    buffer[length] = 0;

    return buffer;
}

void uart::enable(void) {

    IntEnable(this->interrupt);
    UARTIntEnable(this->channel, UART_INT_RX | UART_INT_RT);
}

void uart::disable(void) {

    IntDisable(this->interrupt);
    UARTIntDisable(this->channel, UART_INT_RX | UART_INT_RT);
}
