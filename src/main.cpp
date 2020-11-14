#define UINT_MAX = 65535

#include <stdint.h>
#include <iostream>
#include <bitset>
#include "mbed.h"

#include "Sensor.h"
#include "Array.h"
#include "Galvo.h"
#include "math.h"


using namespace std;

//define LED activation representation pins
DigitalOut tLLED(PF_8);
DigitalOut tRLED(PF_7);
DigitalOut bLLED(PF_9);
DigitalOut bRLED(PG_1);

//photodiode input sensor pins 
AnalogIn tL(A2);  
AnalogIn bL(A3);  
AnalogIn tR(A4);  
AnalogIn bR(A5);  

//Communication pin to the galvo MCU
DigitalOut activation_indicator(PG_0);

//initialize serial monitor
Serial mac2(USBTX, USBRX);

//defining the data bits to be sent over the lines 
volatile uint16_t x;
volatile uint16_t y;

volatile float xFloat;
volatile float yFloat;

volatile bool xParity; 
volatile bool yParity;

float frequency = 5;
float period = 1/frequency;
int stepCount = 876;
float wait_time = period/stepCount;

//One half of the clocking period, effectively. Steptime of 1us gives 2us period = 0.5Mhz 
int stepTime = 2;  //in microseconds (using wait_us())

void updateFrequency( float newFrequency){
    frequency = newFrequency;
    period = 1/frequency;
    wait_time = period/stepCount;
}

void updateStepCount(int newStepCount){
    stepCount = newStepCount;
    wait_time = period/stepCount;
}

void wait_sec(float time){
    wait_us(1000000*time);
}

int main(){
    Sensor topLeft;
    Sensor topRight;
    Sensor bottomLeft; 
    Sensor bottomRight;

    //adding each of the sensor objects to their respective pins, LEDs, and names
    topLeft.sensor = &tL;
    topRight.sensor = &tR;
    bottomLeft.sensor = &bL;
    bottomRight.sensor = &bR;

    topLeft.led = &tLLED;
    topRight.led = &tRLED;
    bottomLeft.led = &bLLED;
    bottomRight.led = &bRLED;

    topLeft.name = '1';
    topRight.name = '2';
    bottomLeft.name = '3';
    bottomRight.name = '4';

    //building the sensor array object and moving the sensors into it 

    Array array;
    array.sensors[0] = topLeft;
    array.sensors[1] = topRight;
    array.sensors[2] = bottomLeft;
    array.sensors[3] = bottomRight;

    array.activation_indicator = &activation_indicator;

    //initialize the values and calibrate the sensors 
    
    //before the initial calibration, we need to set the thresholds all high so that it doesn't get tripped initially
    for(int i=0; i<4; i++){
        array.sensors[i].threshold = 1;
    }

    array.activation_indicator->write(0);

    array.calibrate();
    array.blink();
    mac2.printf("\n\n");
    mac2.printf("Initial calibration \r\n");
    array.printCalibration();
    mac2.printf("\n\n");

    int i = 0;

    while(1){
        array.read();
        // array.displayLEDs();
        i++;

        if(i==100){
            array.displayValue();
            i=0;
        }
        wait_sec(0.01);
    }
}