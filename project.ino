#include "main_menu.h"
#include "utils.h"



//const int buzzerPin = 13;


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

  setup_main_menu();


  // the zero refers to the MAX7219 number, it is zero for 1 chip
  lc.shutdown(0, false); // turn off power saving, enables display
  lc.setIntensity(0, matrixBrightness); // sets brightness (0~15 possible values)
  lc.clearDisplay(0);// clear screen

}






void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  //lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  //lcd.print(2000 + millis() / 1000);
//
//  // for byte matrix
//  for (int row = 0; row < matrixSize; row++)
//  {
//    lc.setRow(0, row, matrixByte[row]);
//  }
  
    main_menu_loop();

  //tetris_loop();
  //Serial.println("NU");
}
