#include "utils.h"
#include "game.h"

//Blinking cursor
unsigned long blinkTimer;
const int blinkDelay = 300;
const int resetBlinkDelay = 450;

// Used only when resetting settings
void (*resetFunc)(void) = 0;

//EEPROM for name and score
byte eepromOffset = 0;
byte currentEepromOffset = 0;

byte currentHighScorePosition = 0;

// Menus

bool isPrinted = false;
byte currentPage = 2; // 3

//HighScore menu
String menuDisplayItemType;

// HowToPlay & About submenus
const int resetAboutScrollTime = 400;
const int resetHowToPlayScrollTime = 200;
unsigned long scrollTimer = 0;
byte scrollCount = 0;
bool waitBeforeScroll = false;

//BrightnessMX setup variables
bool reverse = false;
byte row = 0;
byte col = 0;
const byte ledTurnDelay = 50;
unsigned long ledTimer = 0;

void displayMainMenu()
{
  printLine(0, "Main Menu:");
  switch (currentMainMenuIndex)
  {
  case 0:
    if (!gamePaused)
    {
      printLine(1, "1.Play game");
    }
    else
    {
      printLine(1, "1.Resume Game");
    }
    break;
  case 1:
    printLine(1, "2.High score");
    break;
  case 2:
    printLine(1, "3.Settings");
    break;
  case 3:
    printLine(1, "4.How to play");
    break;
  case 4:
    printLine(1, "5.About");
    break;
  }

  byte currentInputRead = readInput();
  String newStr;
  if (currentInputRead != lastInputRead)
  {
    lastInputRead = currentInputRead;
    switch (currentInputRead)
    {
    case up:
      if (pointerMode == POINTER_SCROLL)
        if (currentMainMenuIndex <= 0)
        {
          currentMainMenuIndex = mainMenuCount - 1;
        }
        else
        {
          currentMainMenuIndex--;
        }
      break;
    case down:
      if (pointerMode == POINTER_SCROLL)
      {
        if (currentMainMenuIndex >= mainMenuCount - 1)
        {
          currentMainMenuIndex = 0;
        }
        else
        {
          currentMainMenuIndex++;
        }
      }
      break;

    case right:
      if (pointerMode == POINTER_SELECT)
      {
        pointerMode = POINTER_SCROLL;
      }
      break;
    case left:

      if (pointerMode == POINTER_SCROLL)
      {
        pointerMode = POINTER_SELECT;
      }
      break;

    case enter:

      lastInputRead = none;
      if (pointerMode == POINTER_SELECT)
      {
        int lastOffset, newOffset;
        switch (currentMainMenuIndex)
        {
        case 0:

          screenStatus = STATUS_GAME;
          pointerMode = POINTER_NOTHING;
          if (!gamePaused)
          {
            bootGame();
          }
          else
          {
            gamePaused = false;
            lc.shutdown(0, false);
          }
          break;

        case 1:

          screenStatus = STATUS_HIGHSCORES;
          pointerMode = POINTER_NOTHING;
          lcd.clear();
          currentHighScorePosition = 0;
          break;

        case 2:
          screenStatus = STATUS_SETTINGS;
          pointerMode = POINTER_SCROLL;
          currentMenuSettingsIndex = 0;
          lcd.clear();
          break;
        case 3:
          screenStatus = STATUS_HOWTOPLAY;
          pointerMode = POINTER_NOTHING;
          lcd.clear();
          break;
        case 4:
          screenStatus = STATUS_ABOUT;
          pointerMode = POINTER_NOTHING;
          printLine(0, "   Madalin   Frincu  332   ");
          printLine(1, "www.github.com/ursawarlord/");
          for (int i = 0; i < 3; i++)
          {
            lcd.scrollDisplayRight();
          }
          scrollCount = 0;
          break;
        }
      }
    }
  }
}

void displayHowToPlay()
{
  // //Arduino UNO low memory. Aborting this menu
  // screenStatus = STATUS_MAINMENU;
  // pointerMode = POINTER_SCROLL;
  // currentMainMenuIndex = 3;

  if (!isPrinted)
  {
    switch (currentPage)
    {
    case 0:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.write("It is easy to play Tetris.");
      break;
    case 1:

      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.write("You need to use joystick & button.");
      break;
    case 2:

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.write("Near matrix you find the pause button.");
      break;
    case 3:

      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.write("Have fun and we wish you a high score.");
      break;
    }
    isPrinted = !isPrinted;
    scrollCount = 0;
    delay(900);
  }
  else
  {
    if (!waitBeforeScroll)
    {
      lcd.scrollDisplayLeft();
      waitBeforeScroll = true;
    }
    switch (currentPage)
    {
    case 0:
      if (scrollCount == 27 - 16)
      {
        delay(900);
        lcd.home();
        isPrinted = !isPrinted;
        currentPage++;
      }
      break;
    case 1:
      if (scrollCount == 35 - 16)
      {
        delay(900);
        lcd.home();
        isPrinted = !isPrinted;
        currentPage++;
      }
      break;
    case 2:
      if (scrollCount == 39 - 16)
      {
        delay(900);
        lcd.home();
        isPrinted = !isPrinted;
        currentPage++;
      }
      break;
    case 3:
      if (scrollCount == 39 - 16)
      {
        delay(900);
        screenStatus = STATUS_MAINMENU;
        pointerMode = POINTER_SCROLL;
        currentMainMenuIndex = 3;
        lcd.home();
        currentPage = 0;
      }
      break;
    }
  }

  byte currentInputRead = readInput();
  if (currentInputRead != lastInputRead && currentInputRead == enter)
  {
    lastInputRead = currentInputRead;
    screenStatus = STATUS_MAINMENU;
    pointerMode = POINTER_SCROLL;
    currentMainMenuIndex = 3;
    lcd.home();
  }
  if (millis() - scrollTimer > resetHowToPlayScrollTime)
  {
    scrollTimer = millis();
    scrollCount++;
    waitBeforeScroll = false;
  }
}

void displayAbout()
{
  // //Arduino UNO low memory. Aborting this menu
  // screenStatus = STATUS_MAINMENU;
  // pointerMode = POINTER_SCROLL;
  // currentMainMenuIndex = 4;

  if (scrollCount == 17 + 18) // 13+17+4
  {
    scrollCount = 0;
  }

  if (!waitBeforeScroll)
  {
    if (scrollCount < 17)
    {
      lcd.scrollDisplayLeft();
    }
    else if (scrollCount < 17 + 18 && scrollCount > 17)
    {
      lcd.scrollDisplayRight();
    }
    waitBeforeScroll = true;
  }

  byte currentInputRead = readInput();

  if (currentInputRead != lastInputRead && currentInputRead == enter)
  {
    lastInputRead = currentInputRead;
    screenStatus = STATUS_MAINMENU;
    pointerMode = POINTER_SCROLL;
    currentMainMenuIndex = 4;
    lcd.home();
  }

  if (millis() - scrollTimer > resetAboutScrollTime)
  {
    scrollTimer = millis();
    scrollCount++;
    waitBeforeScroll = false;
  }
}

void displayHighScores()
{
  highScoreRecords = readIntFromEEPROM(eepromHighScoreRecordsAddress);
  if (highScoreRecords > 3 || highScoreRecords < 0)
  {
    highScoreRecords = 0;
  }

  if (highScoreRecords == 0)
  {

    printLine(0, "NO HIGHSCORES");
    lcd.setCursor(0, 1);
    lcd.write("Returning.");
    for (int i = 0; i < 6; i++)
    {
      lcd.write(".");
      delay(500);
    }
    screenStatus = STATUS_MAINMENU;
    pointerMode = POINTER_SCROLL;
    currentMainMenuIndex = 1;
  }
  else
  {
    int currentInputRead = readInput();
    if (currentInputRead != lastInputRead)
    {
      lastInputRead = currentInputRead;
      switch (currentInputRead)
      {
      case up:
        if (currentHighScorePosition > 0)
        {
          currentHighScorePosition--;
        }
        else {
          currentHighScorePosition = highScoreRecords - 1;
        }
        break;
      case down:
        if (currentHighScorePosition < highScoreRecords - 1)
        {
          currentHighScorePosition++;
        }
        else
        {
          currentHighScorePosition = 0;
        }
        break;
      case enter:
        screenStatus = STATUS_MAINMENU;
        pointerMode = POINTER_SCROLL;
        currentMainMenuIndex = 1;
        break;
      }
    }
    String name1, name2, currentName;
    int currentScore;
    currentScore = readIntFromEEPROM(eepromHighScoreStartOffset + currentHighScorePosition * sizeof(int));
    byte length1, length2;

    if (currentHighScorePosition == 0)
    {
      readStringFromEEPROM(eepromPlayerNameStartOffset, &currentName);
    }
    else if (currentHighScorePosition == 1)
    {
      length1 = readStringFromEEPROM(eepromPlayerNameStartOffset, &name1);
      readStringFromEEPROM(eepromPlayerNameStartOffset + length1, &currentName);
    }
    else if (currentHighScorePosition == 2)
    {
      length1 = readStringFromEEPROM(eepromPlayerNameStartOffset, &name1);
      length2 = readStringFromEEPROM(eepromPlayerNameStartOffset + length1, &name2);
      readStringFromEEPROM(eepromPlayerNameStartOffset + length1 + length2, &currentName);
    }

    lcd.setCursor(0, 0);
    lcd.print("High scores ");
    lcd.setCursor(13, 0);
    if (currentHighScorePosition == 0)
    {
      lcd.print("1st");
    }
    else if (currentHighScorePosition == 1)
    {
      lcd.print("2nd");
    }
    else
    {
      lcd.print("3rd");
    }
    lcd.setCursor(0, 1);
    lcd.print(currentName);
    lcd.print(" Score");
    lcd.print(currentScore);
    lcd.print("          ");
  }
}

void displayContrastSettings()
{
  lcd.setCursor(0, 1);
  lcd.write((byte(2))); // left arrow

  lcd.setCursor(4, 1);
  lcd.write("CONTRAST");
  lcd.setCursor(15, 1);
  lcd.write(byte(3)); // right arrow

  lcd.setCursor(12, 0);
  lcd.print(map(currentContrast, 0, 150, 0, 100));
  lcd.print("%   ");

  int currentInputRead = readInput();
  //keep debouncer
  if (currentInputRead != lastInputRead && currentInputRead == enter)
  {
    lastInputRead = currentInputRead;
    screenStatus = STATUS_SETTINGS;
    pointerMode = POINTER_SCROLL;
    lcd.clear();
    currentMenuSettingsIndex = 2;
  }
  else
  {
    switch (currentInputRead)
    {
    case left:

      if (currentContrast > 0)
      {
        currentContrast -= 1;
      }
      analogWrite(contrastPin, currentContrast);

      delay(10);
      break;

    case right:
      if (currentContrast < 150)
      {
        currentContrast += 1;
      }
      analogWrite(contrastPin, currentContrast);
      delay(10);
      break;
    }
  }
}

void displayScreenBrightnessMenu()
{
  lcd.setCursor(0, 1);
  lcd.write((byte(2))); // left arrow

  lcd.setCursor(2, 1);
  lcd.print("brightnessLCD");
  lcd.setCursor(15, 1);
  lcd.write(byte(3)); // right arrow

  lcd.setCursor(12, 0);
  lcd.print(map(LCDBrightness, 800, 950, 0, 100));
  lcd.print("%   ");

  int currentInputRead = readInput();

  if (currentInputRead != lastInputRead && currentInputRead == enter)
  {
    lastInputRead = currentInputRead;
    screenStatus = STATUS_SETTINGS;
    pointerMode = POINTER_SCROLL;
    lcd.clear();
    currentMenuSettingsIndex = 3;
  }
  else
  {
    switch (currentInputRead)
    {
    case left:

      if (LCDBrightness > 800)
      {
        LCDBrightness -= 1;
        analogWrite(LCDBrightnessPin, LCDBrightness);
        delay(10);
      }
      break;
    case right:
      if (LCDBrightness < 950)
      {
        LCDBrightness += 1;
        analogWrite(LCDBrightnessPin, LCDBrightness);
        delay(10);
      }
      break;
    }
  }
}

void displayMatrixBrightnessMenu()
{
  lcd.setCursor(0, 1);
  lcd.write((byte(2))); // left arrow
  lcd.setCursor(2, 1);
  lcd.print("brightnessMX");
  lcd.setCursor(15, 1);
  lcd.write(byte(3)); // right arrow
  lcd.setCursor(12, 0);
  lcd.print(map(matrixBrightness, 1, 20, 0, 100));
  lcd.print("%   ");
  if (row < matrixSize && reverse == false)
  {
    if (millis() - ledTimer > ledTurnDelay)
    {
      ledTimer = millis();
      lc.setLed(0, col, row, true); // turns on LED at col, row
      col++;
    }
    if (col == matrixSize)
    {
      row++;
      col = 0;
    }
  }
  if ((row == matrixSize && col == 0))
  {
    reverse = !reverse;
    row = 0;
    col = 0;
  }
  if (row < matrixSize && reverse == true)
  {
    if (millis() - ledTimer > ledTurnDelay)
    {
      ledTimer = millis();
      lc.setLed(0, col, row, false); // turns on LED at col, row
      col++;
    }
    if (col == matrixSize)
    {
      row++;
      col = 0;
    }
  }
  int currentInputRead = readInput();
  // keep debouncer
  if (currentInputRead != lastInputRead && currentInputRead == enter)
  {
    lastInputRead = currentInputRead;
    lc.clearDisplay(0);
    reverse = false;
    row = 0;
    col = 0;
    screenStatus = STATUS_SETTINGS;
    pointerMode = POINTER_SCROLL;
    lcd.clear();
    currentMenuSettingsIndex = 4;
  }
  else
  {
    switch (currentInputRead)
    {
    case left:
      if (matrixBrightness > 1)
      {
        matrixBrightness -= 1;
      }
      lc.setIntensity(0, matrixBrightness);
      delay(100);
      break;

    case right:

      if (matrixBrightness < 20)
      {
        matrixBrightness += 1;
      }
      lc.setIntensity(0, matrixBrightness);
      delay(100);
      break;
    }
  }
}

void displaySettingsMenu()
{
  printLine(0, "Settings:");

  lcd.setCursor(2, 1);
  switch (currentMenuSettingsIndex)
  {
  case 0:
    lcd.write("Show name");
    break;
  case 1:
    lcd.write("Change name");
    break;
  case 2:
    lcd.write("Contrast");
    break;
  case 3:
    lcd.write("BrightnessLCD");
    break;
  case 4:
    lcd.write("BrightnessMX");
    break;
  case 5:
    lcd.write("System config");
    break;
  case 6:
    lcd.write("Reset Memory");
    break;
  case 7:
    lcd.write("Quit Settings");
    break;
  }
  lcd.write("      ");
  byte currentInputRead = readInput();
  String newStr;
  if (currentInputRead != lastInputRead)
  {
    lastInputRead = currentInputRead;
    switch (currentInputRead)
    {
    case up:
      if (pointerMode == POINTER_SCROLL)
        if (currentMenuSettingsIndex <= 0)
        {
          currentMenuSettingsIndex = settingsMenuCount - 1;
        }
        else
        {
          currentMenuSettingsIndex--;
        }
      break;
    case down:
      if (pointerMode == POINTER_SCROLL)
      {
        if (currentMenuSettingsIndex >= settingsMenuCount - 1)
        {
          currentMenuSettingsIndex = 0;
        }
        else
        {
          currentMenuSettingsIndex++;
        }
      }
      break;
    case right:
      if (pointerMode == POINTER_SELECT)
      {
        pointerMode = POINTER_SCROLL;
      }
      break;
    case left:

      if (pointerMode == POINTER_SCROLL)
      {
        pointerMode = POINTER_SELECT;
      }
      break;
    case enter:
      lastInputRead = none;
      if (pointerMode == POINTER_SELECT)
      {
        switch (currentMenuSettingsIndex)
        {
        case 0:
          screenStatus = STATUS_SHOWNAME;
          pointerMode = POINTER_NOTHING;
          lcd.clear();
          break;
        case 1:
          screenStatus = STATUS_CHANGENAME;
          pointerMode = POINTER_NOTHING;
          lcd.clear();
          break;
        case 2:
          screenStatus = STATUS_CONTRAST;
          pointerMode = POINTER_CONTRAST;
          lcd.clear();
          break;
        case 3:
          screenStatus = STATUS_BRIGHTNESSLCD;
          pointerMode = POINTER_CONTRAST;
          lcd.clear();
          break;
        case 4:
          screenStatus = STATUS_BRIGHTNESSMATRIX;
          pointerMode = POINTER_CONTRAST;
          lcd.clear();
          lc.clearDisplay(0);
          break;
        case 5:
          screenStatus = STATUS_CONFIGSYSTEM;
          pointerMode = POINTER_SCROLL;
          lcd.clear();
          break;
        case 6:
          screenStatus = STATUS_RESETMEMORY;
          pointerMode = POINTER_NOTHING;
          currentConfigSettingsIndex = 0;
          lcd.clear();
          break;
        case 7:
          screenStatus = STATUS_MAINMENU;
          pointerMode = POINTER_SCROLL;
          currentMainMenuIndex = 2;
        }
      }
    }
  }
}

void displayResetMemory()
{
  printLine(0, "RESETTING ALL");
  lcd.setCursor(0, 1);
  lcd.write("Loading.");
  for (int i = 1; i <= 6; i++)
  {
    for (int j = (eepromSize / 6) * (i - 1); j <= (eepromSize / 6) * i; j++)
    {
      EEPROM.write(j, 0);
    }
    lcd.write(".");
    delay(100);
  }
  // Assure we init() eeprom again
  appInitSuccessfully = OFF;
  writeIntIntoEEPROM(eepromAppInitSuccessfullyAddress, appInitSuccessfully);
  resetFunc();
}

void displayShowName()
{
  printLine(0, "Current name");
  printLine(1, currentUsername);
  byte currentInputRead = readInput();
  if (currentInputRead != lastInputRead && currentInputRead == enter)
  {
    lastInputRead = currentInputRead;
    screenStatus = STATUS_SETTINGS;
    pointerMode = POINTER_SCROLL;
    currentMainMenuIndex = 0;
    lcd.home();
  }
}

void displayChangeName()
{
  printLine(0, "Enter your name");
  printLine(1, "In serial");
  byte currentInputRead = readInput();
  inputString = "";
  if (currentInputRead != lastInputRead && currentInputRead == enter)
  {
    while (Serial.available() > 0)
    {
      incomingByte = (char)Serial.read();
      if (incomingByte == '\n')
      {
        stringComplete = true;
        currentUsername = inputString;
        writeStringToEEPROM(eepromCurrentUsernameStartOffset, currentUsername);
        break;
      }
      else
      {
        inputString += incomingByte;
      }
    }
    lastInputRead = currentInputRead;
    screenStatus = STATUS_SETTINGS;
    pointerMode = POINTER_SCROLL;
    currentMainMenuIndex = 1;
  }
}

void displaySystemConfig()
{
  printLine(0, " System config:");
  switch (currentConfigSettingsIndex)
  {
  case 0:
    lcd.setCursor(0, 1);
    lcd.print("ReverseJOY");
    if (reverseJoystick == false)
    {
      lcd.print(" >O I");
    }
    else
    {
      lcd.print(" O >I");
    }
    break;
  case 1:
    lcd.setCursor(0, 1);
    lcd.print("DIFF");
    if (difficultyValue == EASY)
    {
      lcd.print(" >ez med hd");
    }
    else if (difficultyValue == MEDIUM)
    {
      lcd.print(" ez >med hd");
    }
    else if (difficultyValue == HARD)
    {
      lcd.print(" ez med >hd");
    }
    break;
  case 2:
    printLine(1, "QUIT");
    break;
  }

  byte currentInputRead = readInput();
  if (currentInputRead != lastInputRead)
  {
    lastInputRead = currentInputRead;
    switch (currentInputRead)
    {
    case up:

      if (currentConfigSettingsIndex <= 0)
      {
        currentConfigSettingsIndex = configMenuCount - 1;
      }
      else
      {
        currentConfigSettingsIndex--;
      }
      break;
    case down:
      if (currentConfigSettingsIndex >= configMenuCount - 1)
      {
        currentConfigSettingsIndex = 0;
      }
      else
      {
        currentConfigSettingsIndex++;
      }
      break;
    case right:
      if (currentConfigSettingsIndex == 0)
      {
        reverseJoystick = !reverseJoystick;
        delay(400);
      }
      else if (currentConfigSettingsIndex == 1)
      {
        if (difficultyValue == EASY)
        {
          difficultyValue = MEDIUM;
          // lcd.print(" ez >med hd");
        }
        else if (difficultyValue == MEDIUM)
        {
          difficultyValue = HARD;
          // lcd.print(" ez >med hd");
        }
        else if (difficultyValue == HARD)
        {
          difficultyValue = EASY;
          // lcd.print(" ez med >hd");
        }
      }
      break;
    case left:
      if (currentConfigSettingsIndex == 0)
      {
        reverseJoystick = !reverseJoystick;
        delay(400);
      }
      else if (currentConfigSettingsIndex == 1)
      {
        if (difficultyValue == EASY)
        {
          difficultyValue = HARD;
        }
        else if (difficultyValue == MEDIUM)
        {
          difficultyValue = EASY;
        }
        else if (difficultyValue == HARD)
        {
          difficultyValue = MEDIUM;
        }
      }
      break;

    case enter:
      screenStatus = STATUS_SETTINGS;
      pointerMode = POINTER_SCROLL;
      currentMenuSettingsIndex = 0;
      lcd.clear();
      break;
    }
  }
}

void handleMenu()
{
  if (screenStatus == STATUS_MAINMENU)
  {
    displayMainMenu();
  }
  if (screenStatus == STATUS_SHOWNAME)
  {
    displayShowName();
  }
  if (screenStatus == STATUS_CHANGENAME)
  {
    displayChangeName();
  }
  if (screenStatus == STATUS_CONFIGSYSTEM)
  {
    displaySystemConfig();
  }
  if (screenStatus == STATUS_SETTINGS)
  {
    displaySettingsMenu();
  }

  else if (screenStatus == STATUS_HIGHSCORES)
  {
    displayHighScores();
  }
  else if (screenStatus == STATUS_HOWTOPLAY)
  {
    displayHowToPlay();
  }
  else if (screenStatus == STATUS_CONTRAST)
  {
    displayContrastSettings();
  }
  else if (screenStatus == STATUS_ABOUT)
  {
    displayAbout();
  }
  else if (screenStatus == STATUS_RESETMEMORY)
  {
    displayResetMemory();
  }
  else if (screenStatus == STATUS_BRIGHTNESSMATRIX)
  {
    displayMatrixBrightnessMenu();
  }
  else if (screenStatus == STATUS_BRIGHTNESSLCD)
  {
    displayScreenBrightnessMenu();
  }
  else if (screenStatus == STATUS_GAME)
  {
    int gameOver = runGame();
    if (gameOver == -1)
    {
      screenStatus = STATUS_MAINMENU;
      pointerMode = POINTER_SCROLL;
      currentMainMenuIndex = 0;
    }
  }

  // flashing menu pointer
  if (pointerMode != POINTER_NOTHING)
  {

    // this sequence doesnt blink
    if (screenStatus == STATUS_SETTINGS && pointerMode == POINTER_SCROLL)
    {
      lcd.setCursor(0, 1);
      lcd.print(currentMenuSettingsIndex + 1);
      lcd.write(".");
    }

    if (millis() - blinkTimer > resetBlinkDelay)
    {
      blinkTimer = millis();
    }
    if (millis() - blinkTimer < blinkDelay)
    {
      if (pointerMode == POINTER_SCROLL)
      {
        lcd.setCursor(15, 1);
        lcd.write((byte)1); // print down arrow
      }
      else if (pointerMode == POINTER_SELECT)
      {
        if (screenStatus == STATUS_MAINMENU)
        {
          lcd.setCursor(13, 1);
          lcd.write((byte)2); // print left arrow
        }
        else
        {
          lcd.setCursor(0, 1);
          lcd.write((byte)3); // print right arrow
          lcd.write(" ");
        }
      }
      else if (pointerMode == POINTER_EXIT)
      {
        lcd.setCursor(15, 1);
        lcd.write((byte)0); // print up arrow
      }
      else if (pointerMode == POINTER_CONTRAST)
      {
        lcd.setCursor(8, 0);
        lcd.write((byte)5); // 2 arrows
      }
    }
    else if (millis() - blinkTimer > blinkDelay)
    {
      if (pointerMode == POINTER_SCROLL)
      {
        lcd.setCursor(15, 1);
        lcd.write("   ");
      }
      else if (pointerMode == POINTER_SELECT)
      {
        if (screenStatus == STATUS_MAINMENU)
        {
          lcd.setCursor(13, 1);
          lcd.write("   ");
        }
        else
        {
          lcd.setCursor(0, 1);
          lcd.write("  ");
        }
      }
      else if (pointerMode == POINTER_CONTRAST)
      {
        lcd.setCursor(8, 0);
        lcd.write("    ");
      }
    }
  }
}
