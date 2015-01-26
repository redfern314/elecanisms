#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "ui.h"
#include "timer.h"
#include "pin.h"
#include "oc.h"

int16_t main(void) {
    init_clock();
    init_ui();
    init_timer();
    init_pin();
    init_oc();

    int sw1Mem=0;
    int sw2Mem=0;
    int sw3Mem=0;
    float LEDtime = 1;

    int selectedLED = 0;
    _LED LEDlist[] = {led1,led2,led3};

    led_on(&led1);
    timer_setPeriod(&timer2, LEDtime);
    timer_start(&timer2);

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
    // pin_digitalOut(&D[6]);
    oc_pwm(&oc1,&D[6],&timer3,100,30000); // 5kHz, 50% duty cycle

    // Set IN2 low
    pin_digitalOut(&D[5]);
    pin_clear(&D[5]);

    // Ignore SLEW (D7)
    // Ignore INV (D8)

    while (1) {
        if (timer_flag(&timer2)) {
            timer_lower(&timer2);
            led_toggle(&LEDlist[selectedLED]);
        }

        if (sw_read(&sw1) && !sw1Mem) {
            selectedLED++;
            selectedLED %= 3;
            sw1Mem = 1;

            int i;
            for(i=0;i<3;i++) {
                led_off(&LEDlist[i]);
            }
        }

        if(!sw_read(&sw1)) {
            sw1Mem=0;
        }

        if (sw_read(&sw2) && !sw2Mem) {
            led_write(&led2, !sw_read(&sw2));
            sw2Mem=1;
            LEDtime*=2;
            timer_setPeriod(&timer2, LEDtime);
            timer_start(&timer2);
        }

        if(!sw_read(&sw2)) {
            sw2Mem=0;
        }

        if (sw_read(&sw3) && !sw3Mem) {
            led_write(&led3, !sw_read(&sw3));
            sw3Mem=1;
            LEDtime/=2;
            timer_setPeriod(&timer2, LEDtime);
            timer_start(&timer2);
        }


        if(!sw_read(&sw3)) {
            sw3Mem=0;
        }
    }
}