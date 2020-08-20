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

//Defining the pins for SPI communication
DigitalIn spiFlag(PC_8);
DigitalIn xPar(PC_9);
DigitalIn yPar(PC_10);

SPISlave xBus(PE_6, PE_5, PE_2, PE_4);
SPISlave yBus(PB_5, PB_4, PB_3, PA_4);

bool xRead = 0;
bool yRead = 0;

DigitalOut xAck(PC_4);
DigitalOut yAck(PD_3);

//defining the data bits to be sent over the lines 
volatile uint16_t x;
volatile uint16_t y;
volatile bool xParity; 
volatile bool yParity;

//One half of the clocking period, effectively. Steptime of 1us gives 2us period = 0.5Mhz 
int stepTime = 4;  //in microseconds (using wait_us())

void write(){
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

    //send the parity bit
    xPlus = xParity;
    xMinus = !xPlus;

    yPlus = yParity;
    yMinus = !yPlus;

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

int main(){

    clockPlus = 0;
    clockMinus = 1;

    //stepTime = 1;
    syncPlus = 0;
    syncMinus = 1;

    x = 0;
    y = 0;
 
    xParity = 0;
    yParity = 0;

    xBus.frequency(10000000);
    yBus.frequency(10000000);

    while(1){
    
        write();
        
        if(spiFlag == 1){
            xParity = xPar;
            yParity = yPar;

            while(xRead == 0){
                if(xBus.receive()){
                    x = xBus.read();
                    xRead = !xRead;
                    xAck = !xAck;
                }
            }

            while(yRead == 0){
                if(yBus.receive()){
                    y = yBus.read();
                    yRead = !yRead;
                    yAck = !yAck;
                }
            }

            xRead = !xRead;
            yRead = !yRead;

            xAck = !xAck;
            yAck = !yAck;
        }
    }
}