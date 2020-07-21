using namespace std;

#include "mbed.h"
#include <math.h>
#include "Sensor.h"

Sensor::Sensor(){
    max = -1;
    min = 100;
    offset = 0;
    margin = 0;
}

void Sensor::calibrate(){

    offset = 0;  

    for(int i=0; i<100; i++){

        read = sensor->read();

        if(read > max){
            max = read;
        }
        if(read < min){
            min = read;
        }
        offset+=read;
    }

    offset = offset / 100;
    margin = abs(max - min);
}
