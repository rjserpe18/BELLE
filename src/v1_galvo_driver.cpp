// // run some code based on switching a switch (changing a pin state)
 
// #define _USE_MATH_DEFINES
// #include <cmath>
// #include <tuple>

// #ifndef M_PI
//     #define M_PI 3.14159265358979323846
// #endif

// #include "mbed.h"
// #include "Sensor.h"
// #include "Array.h"
// #include "Galvo.h"
// #include "math.h"

// Serial mac2(USBTX, USBRX);

// using namespace std;

// // define galvo X and Y pins
// AnalogOut dacX(PA_4);
// AnalogOut dacY(PA_5);

// //define sensor pins: 
//     //tL - top left
//     //tR - top right
//     //bL - bottom left
//     //bR - bottom right 

// AnalogIn tL(PC_3);  
// AnalogIn bL(PC_4);  
// AnalogIn tR(PC_1);  
// AnalogIn bR(PC_5);  

// //define LED activation representation pins
// DigitalOut tLLED(PF_8);
// DigitalOut tRLED(PF_7);
// DigitalOut bLLED(PF_9);
// DigitalOut bRLED(PG_1);

// //initializing the pins to control the laser and its intensity
// DigitalIn button(PC_0);
// DigitalOut laserPower(PA_3);


// //helper function for timing 

//     //TODO: Should probably move to the Thread::sleep_for usage, as that's 
//     //what's recommended in the mbedOS docs

// void wait_sec(float time){
//     wait_us(1000000*time);
// }

// int main() {
//     laserPower = 1;

//     Galvo galvo;
//     galvo.x = &dacX;
//     galvo.y = &dacY;

//     galvo.xOffset = 0;
//     galvo.yOffset = 0;

//     //effectively just setting the galvo to (0,0)
//     galvo.move();



//     Sensor topLeft;
//     Sensor topRight;
//     Sensor bottomLeft; 
//     Sensor bottomRight;

//     //adding each of the sensor objects to their respective pins, LEDs, and names
//     topLeft.sensor = &tL;
//     topRight.sensor = &tR;
//     bottomLeft.sensor = &bL;
//     bottomRight.sensor = &bR;

//     topLeft.led = &tLLED;
//     topRight.led = &tRLED;
//     bottomLeft.led = &bLLED;
//     bottomRight.led = &bRLED;

//     topLeft.name = '1';
//     topRight.name = '2';
//     bottomLeft.name = '3';
//     bottomRight.name = '4';

//     //building the sensor array object and moving the sensors into it 

//     Array array;
//     array.sensors[0] = topLeft;
//     array.sensors[1] = topRight;
//     array.sensors[2] = bottomLeft;
//     array.sensors[3] = bottomRight;

//     //initialize the values and calibrate the sensors 
    
//     //before the initial calibration, we need to set the thresholds all high so that it doesn't get tripped initially
//     for(int i=0; i<4; i++){
//         array.sensors[i].threshold = 1;
//     }

//     array.calibrate();
//     mac2.printf("\n\n");
//     mac2.printf("Initial calibration \r\n");
//     array.printCalibration();
//     mac2.printf("\n\n");

//     array.detected = false;
//     int calibrationCounter=0;
    
//     float amplitude = 0.1;
//     float frequency = 1;
//     int resolution = 50;
//     float stepTime = (float)(1/(resolution*frequency*70));
    
//     tuple <float,float> offSet;
//     tuple <float,float> coords;
    
//     float frac;

//     while(1) { 

//         for(int i=0; i<resolution; i++){

//             array.read();

//             if(array.detected == true){
//                 offSet = coords;
//                 amplitude = 0.1;

//                 while(array.detected == true){
//                     array.detected = false;
//                     array.read();
//                 }

//                 break;
//             }

//             frac = (float)i/resolution;

//             coords = galvo.circleWithOffset(amplitude, frequency, get<0>(offSet),get<1>(offSet),frac);

//             //mac2.printf("x: %f y: %f \r\n",get<0>(coords),get<1>(coords)); 

//             galvo.setX(get<0>(coords));
//             galvo.setY(get<1>(coords));
//         }

//         amplitude+= 0.01;
        
//         if(amplitude > 3.5){
//             amplitude = 0.1;
//         }

//         wait_sec(stepTime);

//      }
// }












//         // while(array.detected == false){   

//         //     //calibrate every 1000 readings  
//         //     if(calibrationCounter > 1000){
//         //         // mac2.printf("\r\ncalibrating...\r\n");
//         //         array.calibrate();
                
//         //         // mac2.printf("\n\n");
//         //         // array.printCalibration();
//         //         // mac2.printf("\n\n");

//         //         calibrationCounter = 0;
//         //     }

//         //     array.read();
//         //     // if(calibrationCounter%20003==0){
//         //     //     array.displayValue();
//         //     // }
//         //     calibrationCounter++;
//         // }

//         // while(array.detected == true){
//         //     // mac2.printf("detected!");
//         //     array.read();
//         //     array.displayLEDs();
//         // }
//         // array.detected = false;

//         // mac2.printf("\r\n\n");

//         // array.printCalibration();
//         // wait_sec(5);

//         // mac2.printf("\r\n\n");

//         // for (int i=0; i<=resolution; i++){

//         //     frac = (float) i/resolution;

//         //     setX(get<0>(coords));
//         //     setY(get<1>(coords));

//         //     read = photoResistor;
//         //     wait_sec(stepTime);

//         //     // mac.printf("%f\r\n",read);
//         //     coords = circleWOffset(amplitude, frequency, xOffset, yOffset, frac);

//         //     if(read<0.99999){
//         //         amplitude = 0.01;

//         //         xOffset = get<0>(coords);
//         //         yOffset = get<1>(coords);
                
//         //         while(read < 0.98){
//         //             read = photoResistor;
//         //             // mac.printf("%f\r\n",read);
//         //             wait_sec(0.01);
//         //         }
//         //     }

//         //     amplitude+=0.1/(resolution);

//         //     if(amplitude >= 4){
//         //         amplitude = 0.01;
//         //     }
//         // }