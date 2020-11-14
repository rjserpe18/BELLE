using namespace std;

#include "mbed.h"
#include <math.h>
#include "Array.h"


Array::Array(){
    detected = 0;
}

Serial mac(USBTX, USBRX);

void Array::calibrate(){

    float oldThreshold [4];
    detected = 0;

    for(int i=0; i<4; i++){
        oldThreshold[i] = sensors[i].threshold;

        sensors[i].offset = 0;
        sensors[i].min = 1;
        sensors[i].max = -1;
    }
    
    //gathering 1000 points for calibration
    for(int i=0; i<1000; i++){
        //loop over each of the sensors
        for(int j=0; j<4;j++){
            sensors[j].read = sensors[j].sensor->read();

            //first check if the target was hit- in this case, we need to throw out the calibration
            if(sensors[j].read > sensors[j].threshold){
                //blink();
                detected = 1;
                break;
            }

            if(sensors[j].read > sensors[j].max){
            sensors[j].max = sensors[j].read;
            }
            if(sensors[j].read < sensors[j].min){
                sensors[j].min = sensors[j].read;
            }
            sensors[j].offset+=sensors[j].read;
        } 
        if(detected == 1){
            break;
        }
        wait_ms(10);
    }

    //if nothing was tripped during calibration, generate new calibration data 
    if(detected == 0){
        for(int i=0; i<4; i++){
            sensors[i].offset = sensors[i].offset/1000;
            sensors[i].margin = abs(sensors[i].max-sensors[i].offset);
            sensors[i].threshold = sensors[i].offset+6*sensors[i].margin;
        } 
    }
    else{
        //if the flag got tripped during calibration, revert to the old calibration values
        for(int i=0; i<4; i++){
            sensors[i].threshold=oldThreshold[i];
        }
    }
}

void Array::printCalibration(){
    mac.printf("\r");
    for(int i=0; i<4; i++){
        mac.printf("%c offset: %f, max %f, min %f, margin %f, threshold %f\r\n",sensors[i].name, sensors[i].offset, sensors[i].max,sensors[i].min, sensors[i].margin, sensors[i].threshold);
    }
}

void Array::read(){

    detected = 0;
    for(int i=0; i<4; i++){
        sensors[i].read = sensors[i].sensor->read();

        if(sensors[i].read > sensors[i].threshold){
            detected = 1;
        }
    }
    activation_indicator->write(detected);
}

// void Array::read(){
    
//     //first push back the reading and reset the current read
//     for(int i=0; i<4; i++){
//         sensors[i].previousRead = sensors[i].read;
//         sensors[i].read = 0;
//     }

//     //now gather a few readings
//     int readingCount = 100;

//     for(int i=0; i<readingCount; i++){
//         for(int j=0; j<4; j++){
//             sensors[j].read += sensors[j].sensor->read();
//         }

//         wait_ms(1);
//     }

//     //reset the array sum to zero
//     sum = 0;

//     //now divide out by the reading count
//     for(int i=0; i<4; i++){
//         sensors[i].read = sensors[i].read / readingCount;

//         if(sensors[i].read > sensors[i].threshold){
//             detected = true;
//         }

//         sensors[i].delta = (sensors[i].read - sensors[i].previousRead)/sensors[i].previousRead;
//         sum += sensors[i].read;
//     }

//     //sensors [3] = [topLeft, topRight, bottomLeft, bottomRight]

//     //yDiff -> the net change on the top minus the net change on the bottom 
//     yDiff = (sensors[0].delta + sensors[1].delta)/2 - (sensors[2].delta + sensors[3].delta)/2;

//     xDiff = (sensors[1].delta + sensors[3].delta)/2 - (sensors[0].delta + sensors[2].delta)/2;

//     yDiff = yDiff/sum;
//     xDiff = xDiff/sum;
// }

void Array::displayValue(){
    mac.printf("\r");

    for(int i=0; i<4; i++){
        mac.printf("%c: %f   ",sensors[i].name, sensors[i].read);
    }
    mac.printf("\n");
}

void Array::blink(){
    for(int i=0; i<4; i++){
        sensors[i].led->write(1);
    }
    wait_ms(250);
    
    for(int i=0; i<4; i++){
        sensors[i].led->write(0);
    }

}

void Array::displayLEDs(){
    for(int i=0; i<4; i++){
        if(sensors[i].read > sensors[i].threshold ){
            sensors[i].led->write(1);
        }
        else{
            sensors[i].led->write(0);
        }
    }
}
