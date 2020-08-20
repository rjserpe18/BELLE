// #define UINT_MAX = 65535

// #include <stdint.h>
// #include <iostream>
// #include <bitset>
// #include "mbed.h"

// using namespace std;

// DigitalOut spiFlag(PC_8);

// DigitalOut xPar(PC_9);
// DigitalOut yPar(PC_10);

// DigitalIn xAck(PC_4);
// DigitalIn yAck(PD_3);

// Serial mac2(USBTX, USBRX);

// //use yellow, green, red, orange for MOSI, MISO, SCLK, SSEL  
// SPI xBus(PE_6, PE_5, PE_2, PE_4);
// SPI yBus(PB_5, PB_4, PB_3, PA_4);

// float x;
// float y;

// //the uint16_t versions of the x and y coordinates
// uint16_t xBitty;
// uint16_t yBitty;

// bool xParity;
// bool yParity;

// void coordsTo16(){
//     xBitty = x * UINT16_MAX;
//     yBitty = y * UINT16_MAX;

//     if(x < 0){
//         xParity = 0;
//     } else{
//         xParity = 1;
//     }

//     if(y < 0){
//         yParity = 0;
//     } else{
//         yParity = 1;
//     }
// }

// void write(){
    
//     coordsTo16();

//     xPar = xParity;
//     yPar = yParity;

//     //throw SPI flag
//     spiFlag = !spiFlag;
// `
//     mac2.printf("Attempting Write \r\n");

//     while(xAck == 0){
//         xBus.write(xBitty);
//     }

//     mac2.printf("Wrote x \r\n");

//     while(yAck == 0){
//         yBus.write(yBitty);
//     }

//     mac2.printf("Wrote y \r\n");

//     spiFlag = !spiFlag;
// }



// int main(){

//     spiFlag = 0;

//     x = 0;
//     y = 0;

//     float pos = 0;
//     float step = 0.1;

//     while(1){

//         x = 0.25 * cos(pos);
//         y = 0.25 * sin(pos);

//         write();

//         pos+=step;

//         wait_ms(100);

//     }

// }