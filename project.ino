#include "main_menu.h"
#include "utils.h"
byte matrixBrightness = 2;
const int minThreshold = 200;
const int maxThreshold = 600;
const byte moveInterval = 100;
unsigned long long lastMoved = 0;
const byte matrixSize = 8;
bool matrixChanged = true;


byte matrixByte[matrixSize] = {
  B11100000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000
};


void setup() {
  Serial.begin(9600);
  setupMainMenu();


  // the zero refers to the MAX7219 number, it is zero for 1 chip
  lc.shutdown(0, false); // turn off power saving, enables display
  lc.setIntensity(0, matrixBrightness); // sets brightness (0~15 possible values)
  lc.clearDisplay(0);// clear screen

}



void loop() {

    main_menu_loop();
    

}
