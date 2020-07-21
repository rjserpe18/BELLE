
#ifndef SENSOR.H

#define SENSOR.H

#include "mbed.h"
#include <math.h>

class Sensor{
    public:

        Sensor();
  
        void calibrate();
        
        AnalogIn* sensor;
        char name;
        float read;
        float offset; 
        float previousRead;
        float delta;
        float min;
        float max;
        float margin;
        float threshold;
        DigitalOut* led;
};
#endif