#define UINT_MAX = 65535

#include <stdint.h>
#include <iostream>
#include <bitset>
#include "mbed.h"

using namespace std;

DigitalOut spiFlag();

DigitalOut xPar();
DigitalOut yPar();

SPI xBus();
SPI yBus();

DigitalIn xAck();
DigitalIn yAck();

float x;
float y;

//the uint16_t versions of the x and y coordinates
uint16_t xBitty;
uint16_t yBitty;

bool xParity;
bool yParity;

