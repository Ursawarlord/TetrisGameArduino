#include "main_menu.h"
#include "utils.h"

void setup()
{

  Serial.begin(9600);
   appInitSuccessfully = readIntFromEEPROM(eepromAppInitSuccessfullyAddress);
   // Init only when asked
   if(appInitSuccessfully == OFF) {
    initDefaults();
   }
   // Init if memory is unknown
   if(appInitSuccessfully != OFF && appInitSuccessfully != ON) {
     initDefaults();
   }
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

  currentContrast = readIntFromEEPROM(eepromContrastAddress);
  LCDBrightness = readIntFromEEPROM(eepromScreenBrightnessAddress);
  matrixBrightness = readIntFromEEPROM(eepromMatrixBrightnessAddress);
  difficultyValue = readIntFromEEPROM(eepromDifficultyAddress);
  reverseJoystick = (readIntFromEEPROM(eepromReverseJoystickAddress) > 0) ? true : false;
  
  
  readStringFromEEPROM(eepromCurrentUsernameStartOffset, &currentUsername);
  analogWrite(contrastPin, currentContrast);
  analogWrite(LCDBrightnessPin, LCDBrightness);

  // set up joy pins
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(pauseButtonPin, INPUT_PULLUP);  

  // Init vars
  screenStatus = STATUS_MAINMENU;
  pointerMode = POINTER_SCROLL;
  currentMainMenuIndex = 0;
}


void loop()
{
  handleMenu();
}
