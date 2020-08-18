// #include "mbed.h"
// #include "mbed_events.h"
// #include "rtos.h"
// // #include "FastPWM.h"

// DigitalOut clockPlus(PA_6);
// DigitalOut clockMinus(PE_8);

// DigitalOut syncPlus(PC_7);
// DigitalOut syncMinus(PE_13);
 
// DigitalOut xPlus(PA_7);
// DigitalOut xMinus(PD_14);

// DigitalOut yPlus(PB_4);
// DigitalOut yMinus(PF_12); 

// Serial mac2(USBTX, USBRX);

// volatile uint16_t xPos;
// volatile uint16_t yPos;

// volatile int xDataBitArray[16];
// volatile int xParityBit;

// volatile int yDataBitArray[16];
// volatile int yParityBit;

// volatile int controlBits [3] = {0,0,1};

// volatile int stepTime;

// Thread galvo;
// Thread xParser;
// Thread yParser;

// void set_position(){
//     while(1){
//         // printf("in set position");

//         for(int i=0; i<=2; i++){
//             xPlus = controlBits[i];
//             xMinus = !xPlus;
            
//             yPlus = controlBits[i];
//             yMinus = !yPlus;

//             clockPlus = !clockPlus;
//             clockMinus = !clockMinus;
            
//             wait_us(stepTime);

//             clockPlus = !clockPlus;
//             clockMinus = !clockMinus;
            
//             wait_us(stepTime);
//         }

//         for(int i=0; i<=15; i++){

//             xPlus = xDataBitArray[i];
//             xMinus = !xPlus;
            
//             yPlus = yDataBitArray[i];
//             yMinus = !yPlus;

//             clockPlus = !clockPlus;
//             clockMinus = !clockMinus;
            
//             wait_us(stepTime);

//             clockPlus = !clockPlus;
//             clockMinus = !clockMinus;
            
//             wait_us(stepTime);
//         }

//         xPlus = xParityBit;
//         xMinus = !xPlus;
        
//         yPlus = yParityBit;
//         yMinus = !yPlus;

//         syncPlus = !syncPlus;
//         syncMinus = !syncMinus;
//         clockPlus = !clockPlus;
//         clockMinus = !clockMinus;
        
//         wait_us(stepTime);

//         clockPlus = !clockPlus;
//         clockMinus = !clockMinus;
        
//         wait_us(stepTime);

//         xPlus = controlBits[0];
//         xMinus = !xPlus;

//         yPlus = controlBits[0];
//         yMinus = !yPlus;

//         syncPlus = !syncPlus;
//         syncMinus = !syncMinus;
//         }
// }

// void parse_x_bits(){
//     while(1){
//         // printf("in parse x");

//         for(int i=0; i<=15; i++){
//             xDataBitArray[i] = (xPos >> 15);
//             xPos<<=1;
//         }       
//     }
// }

// void parse_y_bits(){
//     while(1){
//         // printf("in parse y"); 

//         for(int i=0; i<=15; i++){
//             xDataBitArray[i] = (yPos >> 15);
//             yPos<<=1;
//         }     
//     }
  
// }

// int main() {
    
//     printf("test");

//     stepTime = 4;
 
//     int yParityBit = 1;

//     int xParityBit = 1;

//     int xPos = 15000;
//     int yPos = 15000;

//     //initialize the clocks to the opposite values so that when you invert the first time, they'll be in the right value

//     clockPlus = 0;
//     clockMinus = 1;

//     syncPlus = 1;
//     syncMinus = 0;

//     // galvo.set_priority(osPriorityRealtime4);
//     // xParser.set_priority(osPriorityRealtime);
//     // yParser.set_priority(osPriorityRealtime);

//     galvo.start(set_position);
//     xParser.start(&parse_x_bits);
//     yParser.start(parse_y_bits);

//     while (1){
//         xPos += 100;
//         yPos += 100;

//         //part of the semaphor method 

//         // ThisThread::sleep_for(100);
//     }
// }

