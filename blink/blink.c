#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "ui.h"
#include "timer.h"

int16_t main(void) {
    init_clock();
    init_ui();
    init_timer();

    int sw1Mem=0;
    int sw2Mem=0;
    int sw3Mem=0;
    float LEDtime = 1;

    int selectedLED = 0;
    _LED LEDlist[] = {led1,led2,led3};

    led_on(&led1);
    timer_setPeriod(&timer2, LEDtime);
    timer_start(&timer2);

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