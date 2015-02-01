#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "ui.h"
#include "timer.h"
#include "pin.h"
#include "oc.h"
#include "uart.h"

int16_t main(void) {
    init_clock();
    init_ui();
    init_timer();
    init_pin();
    init_oc();
    init_uart();

    led_on(&led3);
    pin_analogIn(&A[5]); // this is the angle sensor pin; we need ADC readings off of it

    uint8_t sw1Mem = 0;

    uint8_t uartbuffer[100] = {0}; // all elements start at 0

    // Set up the UART
    // UART number, TX pin, RX pin, RTS pin, CTS pin, baud, parity, transmission threshold, 
    //  stop bits, transmit buffer, transmit buffer size, receive buffer, receive buffer size
    uart_open(&uart1,&D[12],&D[13],NULL,NULL,9600,'N',1,1,uartbuffer,100,NULL,0);

    uint16_t data = 0;
    uint8_t datastr[3] = {0};
    datastr[2] = '\0'; // null terminate it

    while (1) {
        if (sw_read(&sw1) && !sw1Mem) {
            sw1Mem = 1;
            // led_on(&led3);
            // send something over UART
            data = pin_read(&A[5]);
            datastr[0] = (data & 0xFF00) >> 8;
            datastr[1] = (data & 0x00FF);
            uart_puts(&uart1,datastr);
        }

        if(!sw_read(&sw1)) {
            sw1Mem=0;
        }
    }
}