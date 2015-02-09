#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "ui.h"
#include "timer.h"
#include "pin.h"
#include "oc.h"
#include "uart.h"
#include <stdio.h>

#define true     1
#define false    0

// Position tracking variables
int updatedPos = 0;     // keeps track of the latest updated value of the MR sensor reading
int rawPos = 0;         // current raw reading from MR sensor
int lastRawPos = 0;     // last raw reading from MR sensor
int lastLastRawPos = 0; // last last raw reading from MR sensor
int flipNumber = 0;     // keeps track of the number of flips over the 180deg mark
int tempOffset = 0;
int rawDiff = 0;
int lastRawDiff = 0;
int rawOffset = 0;
int lastRawOffset = 0;
const int flipThresh = 500;  // threshold to determine whether or not a flip over the 180 degree mark occurred
int flipped = false;

uint16_t numChecks = 0;

float lastAngle = 0;

// Run this to print out the current angle of the magnet
void readAngle() {
    uint16_t rawdata = 0;
    float datareading = 0;

    rawdata = pin_read(&A[5]) >> 6;
    datareading = rawdata / 939.0; // scale by measured maximum ADC voltage

    // calibrate to the angle sensor's output and convert to 
    //  angle (0-180 degrees)
    // the raw reading is roughly between 0.052 and 0.892
    datareading -= .052;
    datareading /= .84;
    datareading *= 180;

    printf("%.4f\r\n",datareading);
}

// Run this at set intervals to print out the number of intervals between magnet "flips"
void readFlips() {
    rawPos = pin_read(&A[5]) >> 6;
  
    // Calculate differences between subsequent MR sensor readings
    rawDiff = rawPos - lastRawPos;          //difference btwn current raw position and last raw position
    lastRawDiff = rawPos - lastLastRawPos;  //difference btwn current raw position and last last raw position
    rawOffset = abs(rawDiff);
    lastRawOffset = abs(lastRawDiff);
    
    // Update position record-keeping variables
    lastLastRawPos = lastRawPos;
    lastRawPos = rawPos;

    numChecks++;
    
    // Keep track of flips over 180 degrees
    if((lastRawOffset > flipThresh) && (!flipped)) { // enter this anytime the last offset is greater than the flip threshold AND it has not just flipped
        // if(lastRawDiff > 0) {        // check to see which direction the drive wheel was turning
        //     flipNumber--;              // cw rotation 
        // } else {                     // if(rawDiff < 0)
        //     flipNumber++;              // ccw rotation
        // }
        // if(rawOffset > flipThresh) { // check to see if the data was good and the most current offset is above the threshold
        //     updatedPos = rawPos + flipNumber*rawOffset; // update the pos value to account for flips over 180deg using the most current offset 
        //     tempOffset = rawOffset;
        // } else {                     // in this case there was a blip in the data and we want to use lastactualOffset instead
        //     updatedPos = rawPos + flipNumber*lastRawOffset;  // update the pos value to account for any flips over 180deg using the LAST offset
        //     tempOffset = lastRawOffset;
        // }
        printf("%u\r\n",numChecks);
        numChecks = 0;
        flipped = true;            // set boolean so that the next time through the loop won't trigger a flip
    } else {                        // anytime no flip has occurred
        // updatedPos = rawPos + flipNumber*tempOffset; // need to update pos based on what most recent offset is 
        flipped = false;
    }
}

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

    timer_every(&timer1,0.00001,&readAngle);

    while (1) {
        
    }
}