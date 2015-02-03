#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "ui.h"
#include "timer.h"
#include "pin.h"
#include "oc.h"
#include "uart.h"
#include <stdio.h>

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

    uint16_t rawdata = 0;
    float datareading = 0;
    uint8_t datastr[16] = {0};

    uint32_t pause = 0;

    while (1) {
        if (pause == 0) {
            // send data over UART
            rawdata = pin_read(&A[5]) >> 6;
            datareading = rawdata / 939.0; // scale by measured maximum ADC voltage

            // calibrate to the angle sensor's output and convert to 
            //  angle (0-180 degrees)
            datareading -= .055;
            datareading /= .89;
            datareading *= 180;

            sprintf(datastr,"%.4f\r\n",datareading);
            uart_puts(&uart1,datastr);
            pause = 200000;
        }

        if(!sw_read(&sw1)) {
            sw1Mem=0;
        }

        if (pause) {
            pause--;
        }
    }
}