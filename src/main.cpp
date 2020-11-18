#define UINT_MAX = 65535

#include <stdint.h>
#include <iostream>
#include <bitset>
#include <map>
#include <iterator>

#include "mbed.h"

#include "math.h"


#define pi 3.141592

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

InterruptIn activation_indicator(PF_13);
DigitalIn activation_button(PG_0);

//initialize serial monitor
Serial mac2(USBTX, USBRX);

//defining the data bits to be sent over the lines 
volatile uint16_t x;
volatile uint16_t y;

volatile float xFloat;
volatile float yFloat;

volatile float xOffset;
volatile float yOffset;

volatile bool xParity; 
volatile bool yParity;

volatile int step_count;
float wait_time;

float scaling_factor = 0.50;
float inc = 0.05;

const int master_table_resolution = 1000;
int scale_index = 0;

volatile int loop_index = 0;
volatile float current_scale;

const float step_interval = 0.025;
const int step_resolution = (int)((float)1/step_interval);

volatile float scales [step_resolution];
volatile float step_counts [step_resolution];


//One half of the clocking period, effectively. Steptime of 1us gives 2us period = 0.5Mhz 
int stepTime = 2;  //in microseconds (using wait_us())

void coordsTo16(){    
    //this could likely be sped up...
    // xFloat = xFloat/2;
    // yFloat = yFloat/2;

    // x = (0.5 + xFloat) * UINT16_MAX;
    // y = (0.5 + yFloat) * UINT16_MAX;

    //in fact it can! Thanks myles
    x = (uint16_t)  ( ( ((int32_t) (xFloat * UINT16_MAX)) + UINT16_MAX)/2);
    y = (uint16_t)  ( ( ((int32_t) (yFloat * UINT16_MAX)) + UINT16_MAX)/2 );
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

void write_without_convert(){
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

void wait_sec(float time){
    wait_us(1000000*time);
}

void hit_handler(void){
    loop_index = 0;
    scale_index = 0;
    current_scale = scales[scale_index];
    step_count = step_counts[scale_index];

    xOffset = xFloat;
    yOffset = yFloat;

    while(activation_indicator)
    {
        wait_ns(10);
    }
}

int main(){

    clockPlus = 0;
    clockMinus = 1;

    //stepTime = 1;
    syncPlus = 0;
    syncMinus = 1;

    xFloat = 0;
    yFloat = 0;

    xOffset = 0;
    yOffset = 0;

    x = 0;
    y = 0;
 
    xParity = 0;
    yParity = 0;

    write();

    float sinTable [master_table_resolution];
    float cosTable [master_table_resolution];

    uint16_t sinTable_16 [master_table_resolution];
    uint16_t cosTable_16 [master_table_resolution];

    //populate sin and cos tables
    for(int i=0; i<master_table_resolution; i++){
        sinTable[i] = cos(2*pi*((float)i/master_table_resolution));
        cosTable[i] = sin(2*pi*((float)i/master_table_resolution));

        sinTable_16[i] = (uint16_t)  ( ( ((int32_t) (sinTable[i] * UINT16_MAX)) + UINT16_MAX)/2);
        cosTable_16[i] = (uint16_t)  ( ( ((int32_t) (cosTable[i] * UINT16_MAX)) + UINT16_MAX)/2);
    }

    for(int i=0; i<step_resolution; i++){
    	scales[i] = step_interval * (i+1);
        step_counts[i] = ceil(300*(scales[i])+50);
    }

    //attach rising edge interrupt to pin from sensor board 
    activation_indicator.rise(&hit_handler);

    scale_index = 0;
    int local_index;

    while(1){

        current_scale = scales[scale_index];
        step_count = step_counts[scale_index];

        for(int i=0; i<step_resolution; i++){
            current_scale = scales[scale_index];
            step_count = step_counts[scale_index];

            //printf("Current Scale: %f, Current Step Count: %d\r\n",current_scale, step_count);

            for(loop_index=0; loop_index<step_count; loop_index++){
                
                local_index = floor((float)master_table_resolution/step_count * loop_index);

                xFloat = current_scale*cosTable[local_index] + xOffset;
                yFloat = current_scale*sinTable[local_index] + yOffset;

                if(xFloat > 1 || yFloat > 1){
                    xFloat = 0;
                    yFloat = 0;
                    
                    loop_index = 0;
                    scale_index = 0;
                    current_scale = scales[scale_index];              
                    step_count = step_counts[scale_index];
                }
                //printf("%f, %f\r\n",xFloat, yFloat);
                write();
            }

            

            scale_index+=1;
            if(scale_index == step_resolution){
                scale_index = 0;
            }
            //printf("\r\n");
        }
    }
}