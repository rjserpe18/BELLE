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

//using the first four rows of the second block of the CN8 header (on the dev board)
DigitalOut clockPlus(PA_3);
DigitalOut clockMinus(PD_7);

DigitalOut syncPlus(PC_0);
DigitalOut syncMinus(PD_6);

// DigitalOut xPlus(PC_3);
// DigitalOut xMinus(PD_5);

// DigitalOut yPlus(PC_1);
// DigitalOut yMinus(PD_4);






DigitalOut xPlus(PF_3);
DigitalOut xMinus(PD_4);

DigitalOut yPlus(PC_3);
DigitalOut yMinus(PD_5);


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

void coordsTo16(){    
    //this could likely be sped up...
    xFloat = xFloat/2;
    yFloat = yFloat/2;
    
    x = (0.5 + xFloat) * UINT16_MAX;
    y = (0.5 + yFloat) * UINT16_MAX;
}

void write(){

    coordsTo16();

    //flip the sync signal 
    syncPlus = !syncPlus;
    syncMinus = !syncMinus; 

            //Control Bit 1

    //pull clock high so that the data bits can change
    clockPlus = !clockPlus;
    clockMinus = !clockMinus;

    xPlus = 0;
    xMinus = !xPlus;

    yPlus = 0;
    yMinus = !yPlus;

    wait_us(stepTime);

    //pull clock low to sample the bits 
    clockPlus = !clockPlus;
    clockMinus = !clockMinus;

    wait_us(stepTime);

            //Control Bit 2

    //pull clock high so that the data bits can change
    clockPlus = !clockPlus;
    clockMinus = !clockMinus;

    //don't need to do any changing, x and y control bits are the same

    wait_us(stepTime);

    //pull clock low to sample the bits 
    clockPlus = !clockPlus;
    clockMinus = !clockMinus;

    wait_us(stepTime);

                //Control Bit 3

    //pull clock high so that the data bits can change
    clockPlus = !clockPlus;
    clockMinus = !clockMinus;

    //flipping control bits to 1 
    xPlus = !xPlus;
    xMinus = !xPlus;

    yPlus = !yPlus;
    yMinus = !yPlus;

    wait_us(stepTime);

    //pull clock low to sample the bits 
    clockPlus = !clockPlus;
    clockMinus = !clockMinus;

    wait_us(stepTime);

    //now sending the control bits
    for(int i=0; i<16; i++){

        //pull clock high to change bits
        clockPlus = !clockPlus;
        clockMinus = !clockMinus;

        xPlus = (x>>(15-i))&1;
        xMinus = !xPlus;
        
        yPlus = (y>>(15-i))&1;
        yMinus = !yPlus;

        wait_us(stepTime);

        //pull clock low to sample the bits 
        clockPlus = !clockPlus;
        clockMinus = !clockMinus;

        wait_us(stepTime);
    }

    //pull clock high to change bits
    clockPlus = !clockPlus;
    clockMinus = !clockMinus;

    //send the parity bit --- after some testing, seems to not really do anything?? We'll just set it to zero.
    // xPlus = xParity;
    // xMinus = !xPlus;

    xPlus = 0;
    xMinus = 1;
    
    yPlus = 0;
    yMinus = 1;


    // yPlus = yParity;
    // yMinus = !yPlus;

    //flip the sync bit to signal communication of parity bit
    syncPlus = !syncPlus;
    syncMinus = !syncPlus;

    wait_us(stepTime);

    //pull the clock low to read parity bits
    clockPlus = !clockPlus;
    clockMinus = !clockMinus;

    wait_us(stepTime);

    // //before we end, we'll increment x and y to see
    // x = x + 0b101;
    // y = y + 0b101;

    //now we're ready to start over. The next cycle starts by pulling sync and clock high, 
    //so we don't need to do that here
}

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

    //initialize the values and calibrate the sensors 
    
    //before the initial calibration, we need to set the thresholds all high so that it doesn't get tripped initially
    for(int i=0; i<4; i++){
        array.sensors[i].threshold = 1;
    }

    array.calibrate();
    mac2.printf("\n\n");
    mac2.printf("Initial calibration \r\n");
    array.printCalibration();
    mac2.printf("\n\n");

    int i = 0;

    while(1){
        array.read();
        array.displayLEDs();
        i++;

        if(i==100){
            array.displayValue();
            i=0;
        }
        wait_sec(0.01);

        // ab +=1;

        // if(ab == 20){
        //     array.displayValue();
        //     ab == 0;
        // }

        // mac2.printf("tl: %f  ",tL.read());
        // mac2.printf("tR: %f  ",tR.read());
        // mac2.printf("bR: %f  ",bR.read());
        // mac2.printf("bL: %f  \r\n",bL.read());

        
    }
}

// int main2(){

//     clockPlus = 0;
//     clockMinus = 1;

//     //stepTime = 1;
//     syncPlus = 0;
//     syncMinus = 1;

//     xFloat = 0;
//     yFloat = 0;

//     x = 0;
//     y = 0;
 
//     xParity = 0;
//     yParity = 0;

//     float pi = 3.14159;

//     // frequency = ;
//     // period = 1/frequency;
//     // stepCount = 1000*frequency;
//     // wait_time = period/stepCount;

//     stepCount = 200;

//     write();

//     float sinTable [stepCount];
//     float cosTable [stepCount];

//     for(int i=0; i<stepCount; i++){
//         sinTable[i] = 0.5*cos(2*pi*((float)i/stepCount));
//         cosTable[i] = 0.5*sin(2*pi*((float)i/stepCount));
//     }

//     float mult = 0.05;
//     float internal_mult = 0;

//     while(1){

//         // xFloat = 0;
//         // yFloat = 0; 
//         // write();

//         Array.

//         // for(int j=0; j<20; j++){

//         //     internal_mult = (float)mult*j;

//         //     for(int i=0; i<stepCount; i++){
//         //         xFloat = internal_mult*cosTable[i];
//         //         yFloat = internal_mult*sinTable[i];
//         //         write();
//         //         //wait(0.000001);
//         //     }

//         // }
        
        
//         // if(frequency <= 5){
//         //     updateFrequency(2*frequency);
//         // }
//     }
// }