using namespace std;

#include "mbed.h"
#include "galvo.h"
#include "Array.h"

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

float pi = M_PI;


Galvo::Galvo(){

    xOffset = 0.0;
    yOffset = 0.0;

    max = 9.0;
}

// this function maps the 0-3.3V input voltage from the MCU
// to the -9v to 9v output of the signal amplifier
void Galvo::setX(float voltage){
    if(voltage<-7 || voltage>7){
        x->write_u16(1.65*(0xFFFF/3.3));
    }

    else{
        voltage = voltage * (1.65/9.2391);
        voltage = voltage + 1.65;
        voltage = voltage*(0xFFFF/3.3);

        x->write_u16(voltage);
    }
}

// this function maps the 0-3.3V input voltage from the MCU
// to the -9v to 9v output of the signal amplifier
void Galvo::setY(float voltage){

    voltage = -voltage; 
    
    if(voltage<-7 || voltage>7){
        y->write_u16(1.65*(0xFFFF/3.3));
    }

    else{
        
        voltage = voltage * (1.65/9.2391);
        voltage = voltage + 1.65;
        voltage = voltage*(0xFFFF/3.3);

        y->write_u16(voltage);
    }
}

void Galvo::circle(float voltageX, float voltageY, float xFreq, float yFreq){

    float periodX = 1/xFreq;
    float periodY = 1/yFreq;

    for(int i=0; i<500; i++){
        setX( voltageX * cos(2*pi*xFreq*(((float)i/500))));
        setY( voltageY * sin(2*pi*yFreq*(((float)i/500))));
        wait_us(1000000 * (float)periodX / 500);
    }
}

tuple<float,float> Galvo::circleWithOffset(float amp, float freq, float xOffset, float yOffset, float t){
    return make_tuple(xOffset+amp*cos(2*pi*freq*t) , yOffset+amp*sin(2*pi*freq*t));
}

//this just takes the galvo and sets the DAC pins to the offset valuess
void Galvo::move(){
    setX(xOffset);
    setY(yOffset);
}