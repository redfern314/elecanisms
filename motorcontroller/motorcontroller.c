
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ------------------------- PREPROCESSOR DIRECTIVES -------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "ui.h"
#include "timer.h"
#include "pin.h"
#include "oc.h"
#include "uart.h"
#include "usb.h"
#include <stdio.h>

#define true                    1
#define false                   0
#define PWM_FREQ                5000     // 5000Hz
#define DUTY_CYCLE_MIN          15000    // overcome static friction with the power of torque (used to be 22000)
#define FORWARD_PWM_CONST       1.780850
#define BACKWARD_PWM_CONST      1.815800
#define MIDDLE_PWM_CONST        1.810000


#define SET_MODE                0   // Vendor request that sets the feedback mode
#define SET_TEXTURE             1   // Vendor request that sets the texture frequency

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// --------------------------- VARIABLE DEFINITIONS --------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

typedef enum {
    MODE_NONE,
    MODE_SPRING,
    MODE_DAMP,
    MODE_TEXTURE,
    MODE_WALL
} mode_t;

// Variables that can be set via USB
mode_t mode = MODE_NONE;
int textureThreshold = 1000;

// Position tracking variables
int updatedPos = 0;     // keeps track of the latest updated value of the MR sensor reading
int positionDiff = 0;    //Calculates the change in position per change motor timer 
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

float lastAngle = 0;
float test = 0;

uint16_t motorDutyCycle = 0; // duty cycle (0-65535)
int8_t motorDirection = 1; // 1 is forwards; -1 is reverse

// Sensor data
float backEMF = 0;    
uint16_t statusFlag = 0;
float currentSensor = 0; // summation of the x most recent current samples
float motorCurrent = 0; // the most recent average of current for use in calculation
uint16_t angleSensor = 0;    
uint16_t samples = 0;

uint16_t prevDutyCycle = 0;
int8_t prevMotorDirection = 1;

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------- SENSING FUNCTIONS ----------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

float readSensor(_PIN* pin) {
    uint16_t data = pin_read(pin); // get the ADC reading
    data = data >> 6; // right-justify the 10-bit ADC reading
    return data / 939.0; // scale by measured maximum ADC voltage and return
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
    
    // Keep track of flips over 180 degrees
    if((lastRawOffset > flipThresh) && (!flipped)) { // enter this anytime the last offset is greater than the flip threshold AND it has not just flipped
        if(lastRawDiff > 0) {        // check to see which direction the drive wheel was turning
            flipNumber--;              // cw rotation 
        } else {                     // if(rawDiff < 0)
            flipNumber++;              // ccw rotation
        }
        if(rawOffset > flipThresh) { // check to see if the data was good and the most current offset is above the threshold
            updatedPos = rawPos + flipNumber*rawOffset; // update the pos value to account for flips over 180deg using the most current offset 
            tempOffset = rawOffset;
        } else {                     // in this case there was a blip in the data and we want to use lastactualOffset instead
            updatedPos = rawPos + flipNumber*lastRawOffset;  // update the pos value to account for any flips over 180deg using the LAST offset
            tempOffset = lastRawOffset;
        }
        flipped = true;            // set boolean so that the next time through the loop won't trigger a flip
    } else {                        // anytime no flip has occurred
        updatedPos = rawPos + flipNumber*tempOffset; // need to update pos based on what most recent offset is 
        flipped = false;
    }
    // printf("%i\r\n",updatedPos);
}

// Current with no load and no motor connected (and full forward PWM): 1.786060
// Current with no load and no motor connected (and no PWM): 1.809630
// Current with no load and full forward PWM: 1.780850
// Current with no load and full backward PWM: 1.815800
// float average = 0;
// uint16_t numaverage = 0;
void currentSample() {
    // read all of our sensors and output to UART
    // statusFlag = pin_read(&D[1]);
    currentSensor += readSensor(&A[0]);
    // backEMF += readSensor(&A[1]);
    // (get angle reading)
    samples++;

    if (samples > 100) {
        // backEMF /= samples;
        currentSensor /= samples;
        // numaverage++;
        // average = average*(numaverage-1)+currentSensor;
        // average /= numaverage;

        // if (motorDirection == 1) {
        //     motorCurrent = currentSensor*3.3 - FORWARD_PWM_CONST;
        // } else {
        //     motorCurrent = currentSensor*3.3 - BACKWARD_PWM_CONST;
        // }
        motorCurrent = currentSensor * 3.3 - MIDDLE_PWM_CONST;
        if (motorCurrent < 0) {
            motorCurrent *= -1;
        }

        // printf("%f\r\n",currentSensor*3.3);
        // printf("%f\r\n",motorCurrent);
        // printf("Status: %i\r\nBack EMF: %f\r\nCurrent: %f\r\n",statusFlag,backEMF,currentSensor);

        samples = 0;
        // statusFlag = 0;
        currentSensor = 0;
        // backEMF = 0;
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------- ACTING FUNCTIONS -----------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// resets the motor's duty cycle and direction
void setMotor(void) {
    // printf("%u\t%i\t%i\r\n",motorDutyCycle,motorDirection,updatedPos);
    if (motorDirection != prevMotorDirection || motorDutyCycle != prevDutyCycle) {
        oc_free(&oc1);

        if (motorDirection == 1) {
            // Config PWM for IN1 (D6)
            oc_pwm(&oc1,&D[6],&timer3,PWM_FREQ,motorDutyCycle);

            // Set IN2 (D5) low
            pin_clear(&D[5]);
        } else {
            // Config PWM for IN2 (D5)
            oc_pwm(&oc1,&D[5],&timer3,PWM_FREQ,motorDutyCycle);

            // Set IN1 (D6) low
            pin_clear(&D[6]);
        }
        prevDutyCycle = motorDutyCycle;
        prevMotorDirection = motorDirection;
    }
}

// called on an interrupt to change the direction and duty cycle of the motor if necessary
void changeMotor() {
    switch (mode) {
        case MODE_NONE: {
            motorDutyCycle = 0;
            break;
        }
        case MODE_SPRING: {
            if (updatedPos < -100) {
                motorDutyCycle = DUTY_CYCLE_MIN+updatedPos*-4;
                motorDirection = 1;
            } else if (updatedPos > 100) {
                motorDutyCycle = DUTY_CYCLE_MIN+updatedPos*4;
                motorDirection = -1;
            } else {
                motorDutyCycle = 0;
            }
            printf("%u\t",motorDutyCycle);

            float x = 40000;
            float currentGain = 100000;

            //COMMENTED OUT, increase torque based on distance, already did above
            // float currentDesired = updatedPos*motorDirection*-1/x;
            // if (currentDesired > motorCurrent) {
            //     motorDutyCycle += (int)(currentDesired*y);
            // } else {
            motorDutyCycle += motorCurrent*currentGain;
            // }
            break;
        }
        case MODE_DAMP: {
            int dampingFactor = 100; 
            if(positionDiff-updatedPos>10){
                motorDutyCycle = DUTY_CYCLE_MIN+(positionDiff-updatedPos)*dampingFactor;
                motorDirection=1;    
            }
            else if(positionDiff-updatedPos<-10){
                motorDutyCycle = DUTY_CYCLE_MIN+(positionDiff-updatedPos)*dampingFactor;
                motorDirection=-1;    
            }
            else{
                motorDutyCycle = 0;
            }
            positionDiff=updatedPos;
            break;
        }
        case MODE_TEXTURE: {
            int texturePos = updatedPos % textureThreshold;
            if (updatedPos<0)
            {
                updatedPos*=-1;
            }
            if (texturePos<0)
            {
                texturePos*=-1;
            }
            if (texturePos < 100 || texturePos > (textureThreshold - 100))
            {
                motorDutyCycle=0;
            }
            else if (texturePos > (textureThreshold / 2.0)) {
                motorDutyCycle = DUTY_CYCLE_MIN;
                motorDirection = 1;
            }
            else {
                motorDutyCycle = DUTY_CYCLE_MIN;
                motorDirection = -1;
            }
            break;
        }
        case MODE_WALL: {
            if(updatedPos>1000){
                motorDutyCycle = 40000;
                motorDirection=-1;
            }
            else if (updatedPos<-1000){
                motorDutyCycle = 40000;
                motorDirection=1;
            }
            else{
                motorDutyCycle = 0;
            }
            break;
        }
    }

    setMotor();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ------------------------- COMMUNICATION FUNCTIONS --------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void VendorRequests(void) {
    WORD temp;

    switch (USB_setup.bRequest) {
        case SET_MODE:
            mode = (mode_t)USB_setup.wValue.w;
            // disregard USB_setup.wIndex.w, we're only sending one value
            BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0 
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        case SET_TEXTURE:
            textureThreshold = (int)USB_setup.wValue.w;
            // disregard USB_setup.wIndex.w, we're only sending one value
            BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0 
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        default:
            USB_error_flags |= 0x01;    // set Request Error Flag
    }
}

void VendorRequestsIn(void) {
    switch (USB_request.setup.bRequest) {
        default:
            USB_error_flags |= 0x01;                    // set Request Error Flag
    }
}

void VendorRequestsOut(void) {
    switch (USB_request.setup.bRequest) {
        default:
            USB_error_flags |= 0x01;                    // set Request Error Flag
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// -------------------------------- MAIN LOOP ---------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

int16_t main(void) {
    init_clock();
    init_ui();
    init_timer();
    init_pin();
    init_oc();
    init_uart();

    led_on(&led3);

    uint8_t sw1Mem = 0;

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
    oc_pwm(&oc1,&D[6],&timer3,PWM_FREQ,0); // 50% duty cycle

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

    mode = MODE_TEXTURE; // sets which type of feedback we're using

    InitUSB();                              // initialize the USB registers and serial interface engine
    while (USB_USWSTAT!=CONFIG_STATE) {     // while the peripheral is not configured...
        // printf("%u\t%i\t%i\r\n",motorDutyCycle,motorDirection,updatedPos);
        ServiceUSB();                       // ...service USB requests
    }

    timer_every(&timer1,0.0005,&readFlips);
    timer_every(&timer2,0.01,&changeMotor);
    timer_every(&timer4,0.0005,&currentSample);

    while (1) {
        ServiceUSB();                       // service any pending USB requests and wait for interrupts
    }
}