#include "main_menu.h"
#include "utils.h"

bool startup = true;
const int startupTime = 3000;


void setup()
{
  appInitSuccessfully = readIntFromEEPROM(eepromAppInitSuccessfullyAddress);
  // Init only when asked
  if (appInitSuccessfully == OFF)
  {
    initDefaults();
  }
  // Init if memory is unknown
  if (appInitSuccessfully != OFF && appInitSuccessfully != ON)
  {
    initDefaults();
  }
  // the zero refers to the MAX7219 number, it is zero for 1 chip
  lc.shutdown(0, false);                // turn off power saving, enables display
  lc.setIntensity(0, matrixBrightness); // sets brightness (0~15 possible values)
  lc.clearDisplay(0);                   // clear screen

  lcd.begin(16, 2);

  //custom characters
  lcd.createChar(0, upArrowChar);
  lcd.createChar(1, scrollArrowChar);
  lcd.createChar(2, leftArrowChar);
  lcd.createChar(3, rightArrowChar);
  lcd.createChar(4, downArrowChar);
  lcd.createChar(5, horizontalArrowsChar);
  lcd.createChar(6, downSign);

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
  pinMode(BUZZER, OUTPUT);

  // Init vars
  screenStatus = STATUS_MAINMENU;
  pointerMode = POINTER_SCROLL;
  currentMainMenuIndex = 0;
}



void loop()
{
  if(millis() > startupTime) {
    startup = false;}
  if(startup) {
    lcd.setCursor(0,0);
    if(currentUsername == defaultName) {
      lcd.print("Hello! Please");
      lcd.setCursor(0,1);
      lcd.print("add your name!");
    }
    else {
      lcd.print("Welcome,");
      lcd.setCursor(8,1);
      lcd.print(currentUsername);
    }
  }
  if(!startup) {
  handleMenu();
  }
}
