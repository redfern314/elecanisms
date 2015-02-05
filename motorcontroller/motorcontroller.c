#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "ui.h"
#include "timer.h"
#include "pin.h"
#include "oc.h"
#include "uart.h"
#include <stdio.h>

float readSensor(_PIN* pin) {
    uint16_t data = pin_read(pin); // get the ADC reading
    data = data >> 6; // right-justify the 10-bit ADC reading
    return data / 939.0; // scale by measured maximum ADC voltage and return
}

int16_t main(void) {
    init_clock();
    init_ui();
    init_timer();
    init_pin();
    init_oc();
    init_uart();

    led_on(&led3);

    int8_t motorDirection = 1;
    uint8_t sw1Mem = 0;

    // Sensor data
    float backEMF = 0;    
    uint16_t statusFlag = 0;
    float currentSensor = 0;
    uint16_t angleSensor = 0;    

    // Motor output pins setup

    // Set EN high
    pin_digitalOut(&D[4]);
    pin_set(&D[4]);

    // Set D2 high
    pin_digitalOut(&D[2]);
    pin_set(&D[2]);

    // Set D1 low
    pin_digitalOut(&D[3]);
    pin_clear(&D[3]);

    // Config PWM for IN1 (D6), (<11kHz)
    pin_digitalOut(&D[6]);
    // oc_pwm(&oc1,&D[6],&timer3,100,30000); // 5kHz, 50% duty cycle

    // Set IN2 low
    pin_digitalOut(&D[5]);
    pin_clear(&D[5]);

    // Ignore SLEW (D7)
    // Ignore INV (D8)

    // Input feedback from motor setup

    // Status flag (D1)
    pin_digitalIn(&D[1]);

    // Current sensor from 10mOhm resistor, gain * 10 (A0)
    pin_analogIn(&A[0]);

    // Back EMF from motor (A1)
    pin_analogIn(&A[1]);

    uint32_t samples = 0;

    while (1) {
        // if (sw_read(&sw1) && !sw1Mem) {
        //     sw1Mem = 1;
        //     motorDirection *= -1;
        //     if (motorDirection == 1) {
        //         // Clear PWM for IN2 (D5)
        //         oc_free(&oc1);

        //         // Config PWM for IN1 (D6)
        //         oc_pwm(&oc1,&D[6],&timer3,100,30000); // 5kHz, 50% duty cycle

        //         // Set IN2 low
        //         pin_clear(&D[5]);
        //     } else {
        //         // Clear PWM for IN1 (D6)
        //         oc_free(&oc1);

        //         // Config PWM for IN2 (D5)
        //         oc_pwm(&oc1,&D[6],&timer3,100,30000); // 5kHz, 50% duty cycle

        //         // Set IN1 low
        //         pin_clear(&D[6]);
        //     }
        // }

        // if(!sw_read(&sw1)) {
        //     sw1Mem=0;
        // }

        // read all of our sensors and output to UART
        statusFlag = pin_read(&D[1]);
        currentSensor += readSensor(&A[0]);
        backEMF += readSensor(&A[1]);
        // (get angle reading)
        samples++;

        if (samples > 5000) {
            backEMF /= samples;
            currentSensor /= samples;

            printf("%f\r\n",readSensor(&A[0]));
            // printf("Status: %i\r\nBack EMF: %f\r\nCurrent: %f\r\n",statusFlag,backEMF,currentSensor);

            samples = 0;
            statusFlag = 0;
            currentSensor = 0;
            backEMF = 0;
        }
    }
}