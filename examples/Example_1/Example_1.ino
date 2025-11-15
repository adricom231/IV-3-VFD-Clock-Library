#include <Arduino.h>
#include <VFD.h>

const int latchPin_vfd = 2;
const int clockPin_vfd = 1;
const int dataPin_vfd = 44;

const int latchPin_enable = 7;
const int clockPin_enable = 8;
const int dataPin_enable = 9;

VFD vfd(dataPin_vfd, clockPin_vfd, latchPin_vfd, 6);
VFD clon(dataPin_enable, clockPin_enable, latchPin_enable, 1);


void setup() {
  vfd.begin();
  clon.begin();
  clon.onAll();
  byte pattern[6] = {
    B10110111,
    B00001000,
    B00010111,
    B10110001,
    B00001000,
    B10110111
  };
  vfd.printDisplayRaw(pattern);
  delay(5000);
  vfd.printDisplay("-[AS]-");
}


void loop() {



}




//1- right bottom
//2- przecinek
//- 3right top
//4- top
//5- middle
//6- left top
//7- left bottom
//8- bottom
//byte data =B11111111;
//byte data_z = B00000000;
//byte data_0 =B10110111; 
//byte data_9 =B10111101; 
//byte data_8 =B10111111;
//byte data_7 =B10110000;
//byte data_6 =B10011111;
//byte data_5 =B10011101;
//byte data_4 =B10101100;
//byte data_3 =B10111001;
//byte data_2 =B00111011;
//byte data_1 =B10100000;
