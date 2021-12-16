#include "main_menu.h"
#include "utils.h"

void setup()
{
  Serial.begin(9600);

  // the zero refers to the MAX7219 number, it is zero for 1 chip
  lc.shutdown(0, false);                // turn off power saving, enables display
  lc.setIntensity(0, matrixBrightness); // sets brightness (0~15 possible values)
  lc.clearDisplay(0);                   // clear screen

  lcd.begin(16, 2);
  lcd.createChar(0, upArrowChar);    // create a new custom character (index 0)
  lcd.createChar(1, scrollArrowChar);  // create a new custom character (index 1)
  lcd.createChar(2, leftArrowChar);  // create a new custom character (index 2)
  lcd.createChar(3, rightArrowChar); // create a new custom character (index 3)
  lcd.createChar(4, downArrowChar); // create a new custom character (index 4)
  lcd.createChar(5, horizontalArrowsChar); // create a new custom character (index 4)

  analogWrite(contrastPin, currentContrast);

  analogWrite(screenBrightnessPin, screenBrightness);

  highScoreMemoryInit();

  // set up joy pins
  pinMode(joystickButtonPin, INPUT_PULLUP);
  // setInputPullupPin(joystickButtonPin);


  // Init vars
  screenStatus = "mainMenu";
  pointerMode = "scroll";
  currentMenuIndex = 0;

}

void loop()
{
  handleMenu();
}
