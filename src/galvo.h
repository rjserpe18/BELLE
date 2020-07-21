
#ifndef GALVO.H

#define GALVO.H

#include "mbed.h"
#include <math.h>

class Galvo{
    public:

        Galvo();
        AnalogOut* x;
        AnalogOut* y;
  
        float xOffset;
        float yOffset;

        float max;

        void setX(float voltage);
        void setY(float voltage);

        void circle(float voltageX, float voltageY, float xFreq, float yFreq);
        tuple<float,float> circleWithOffset(float amp, float freq, float xOffset, float yOffset, float frac);

        void move();
};

#endif
