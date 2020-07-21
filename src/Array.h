#ifndef ARRAY.H
#define ARRAY.H

#include "mbed.h"
#include <math.h>
#include "Sensor.h"


class Array{
    public:
        Array();
        bool detected;

        float yDiff;
        float xDiff;

        float sum;

        Sensor sensors [4];
        void calibrate();
        void printCalibration();

        // void update();
        void displayLEDs();
         
        void read();
        void displayValue();
        void displayActivation();

        void blink();

        void updateDirection(float stepSize);
};


#endif