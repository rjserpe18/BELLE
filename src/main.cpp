#define UINT_MAX = 65535

#include <stdint.h>
#include <iostream>
#include <bitset>
#include "mbed.h"

using namespace std;

//using the first four rows of the second block of the CN8 header (on the dev board)
DigitalOut clockPlus(PA_3);
DigitalOut clockMinus(PD_7);

DigitalOut syncPlus(PC_0);
DigitalOut syncMinus(PD_6);

DigitalOut xPlus(PC_3);
DigitalOut xMinus(PD_5);

DigitalOut yPlus(PC_1);
DigitalOut yMinus(PD_4);

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
int stepTime = 4;  //in microseconds (using wait_us())

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

int main(){

    clockPlus = 0;
    clockMinus = 1;

    //stepTime = 1;
    syncPlus = 0;
    syncMinus = 1;

    xFloat = 0;
    yFloat = 0;

    x = 0;
    y = 0;
 
    xParity = 0;
    yParity = 0;

    float pi = 3.14159;

    frequency = 1;
    period = 1/frequency;
    stepCount = 1000*frequency;
    wait_time = period/stepCount;

    write();

    while(1){
        for(int i=0; i<stepCount; i++){
            xFloat = 0.5*cos(2*pi*((float)i/stepCount));
            yFloat = 0.5*sin(2*pi*((float)i/stepCount));
            write();
            wait(wait_time);
        }
        
        if(frequency <= 5){
            updateFrequency(2*frequency);
        }
    }
}