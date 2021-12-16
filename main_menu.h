#include "utils.h"
#include "game.h"

unsigned long blinkTimer;
const int blinkDelay = 300;
const int resetBlinkDelay = 450;

int eepromOffset = 0;
int currentEepromOffset = 0;

int highScoreRecords = 0;
int currentHighScorePosition = 0;

// Menus
String mainMenuText[] = {"1.Play game", "2.High score", "3.Change name", "4.Contrast", "5.About", "6.Brightnss#1", "7.Brightnss#2"};
int mainMenuCount = sizeof(mainMenuText) / sizeof(mainMenuText[0]);
String screenStatus;
String pointerMode;
// Current menu and item to be displayed
int currentMenuIndex;
// Used to check joystick state
int lastJoyRead = none;
String highScorePlayerName[5];
unsigned long highScoresArray[5];
String menuDisplayItemType;

const int resetScrollTime = 150;
unsigned long scrollTimer = 0;
int scrollCount = 0;
bool goScroll = false;



void displayMainMenu()
{
  printLine(0, "Main Menu:");
  printLine(1, mainMenuText[currentMenuIndex]);

  int currentJoyRead = readJoystick();
  String newStr;
  if (currentJoyRead != lastJoyRead)
  {
    lastJoyRead = currentJoyRead;
    printLine(1, mainMenuText[currentMenuIndex]);
    
    switch (currentJoyRead)
    {
    case up:
      if (pointerMode == "scroll")
        if (currentMenuIndex <= 0)
        {
          currentMenuIndex = mainMenuCount - 1;
        }
        else
        {
          currentMenuIndex--;
        }
      break;
    case down:
      if (pointerMode == "scroll")
      {
        if (currentMenuIndex >= mainMenuCount - 1)
        {
          currentMenuIndex = 0;
        }
        else
        {
          currentMenuIndex++;
        }
      }
      break;

    case right:
      if (pointerMode == "select")
      {
        pointerMode = "scroll";
      }
      break;
    case left:

      if (pointerMode == "scroll")
      {
        pointerMode = "select";
      }
      break;

    case enter:
      
      lastJoyRead = none;
      if (pointerMode == "select")
      {

        switch (currentMenuIndex)
        {
        case 0:
        {
          screenStatus = "game";
          pointerMode = "noPointer";
          bootGame();
          break;
        }
        case 1:
        {
          screenStatus = "highScores";
          pointerMode = "scroll";
          lcd.clear();
          int lastOffset = eepromOffset;
          
          for (int i = 0; i < highScoreRecords; i++)
          {
            int newOffset = readStringFromEEPROM(lastOffset, &newStr);
            lastOffset = newOffset;
            highScorePlayerName[i] = newStr;
            char c = newStr[2];
            highScoresArray[i] = newStr.length() * 250 / 25 + (int)c; // experimental
          }
          menuDisplayItemType = "playerName";
          currentHighScorePosition =0;
          break;
        }
        case 2:
        { //screenStatus = "nameSettings";
          break;
        }
        case 3:
        {
          screenStatus = "changeContrast";
          pointerMode = "contrastScroll";
          lcd.clear();
          break;
        }
        case 4:
        {
          screenStatus = "about";
          pointerMode = "noPointer";
          // lcd.begin(16, 2);           ");
          printLine(0, "  Madalin   Frincu   332");
          printLine(1, "www.github.com/ursawarlord/");
          scrollCount = 0;
          break;
        }
        case 5:
        {
          screenStatus = "brightnessScreen";
          pointerMode = "contrastScroll";
          lcd.clear();
          break;
        }
        case 6:
        {
          screenStatus = "brightnessMatrix";
          pointerMode = "contrastScroll";
          lcd.clear();
          lc.clearDisplay(0);
          break;
        }
        }
      }
    }
  }
}

void highScoreMemoryInit()
{
  String player1 = "MADALIN";
  String player2 = "ANDREI";

  byte addrOffsetPlayer1 = writeStringToEEPROM(currentEepromOffset, player1);
  highScoreRecords++;
  currentEepromOffset = addrOffsetPlayer1;

  writeStringToEEPROM(currentEepromOffset, player2);
  highScoreRecords++;
}

void displayAbout()
{

  if (millis() - scrollTimer > resetScrollTime)
  {
    scrollTimer = millis();
    scrollCount++;
    goScroll = true;
  }

  if (scrollCount > 13 + 17 + 4)
  {
    scrollCount = 0;
  }

  if (goScroll)
  {
    if (scrollCount < 13)
    {
      lcd.scrollDisplayLeft();
    }
    else if (scrollCount < 13 + 17 && scrollCount > 13)
    {
      lcd.scrollDisplayRight();
    }
    else if (scrollCount < 13 + 17 + 4 && scrollCount > 13 + 17)
    {
      lcd.scrollDisplayLeft();
    }
    goScroll = false;
  }

  int currentJoyRead = readJoystick();
  if (currentJoyRead != lastJoyRead)
  {
    lastJoyRead = currentJoyRead;
    switch (currentJoyRead)
    {
    case enter:
      screenStatus = "mainMenu";
      pointerMode = "scroll";
      currentMenuIndex = 4;
      lcd.home();
      break;
    }
  }
}

void displayHighScores()
{
  lcd.setCursor(0, 0);
  lcd.print("High scores:");
  if (currentHighScorePosition == 0)
  {
    lcd.setCursor(15, 0);
    lcd.print("x");
  }

  int currentPos = 0;
  if (menuDisplayItemType == "playerName") {
      lcd.setCursor(0,1);
      lcd.print(String(currentHighScorePosition+1));
      lcd.print(".");
      lcd.print(highScorePlayerName[currentHighScorePosition]);
      currentPos += String(currentHighScorePosition+1).length() + 1 + highScorePlayerName[currentHighScorePosition].length();
      for(int i=currentPos;i<15;i++)
        lcd.print(" ");
  }
  else if (menuDisplayItemType == "playerScore")
  {
      
      lcd.setCursor(0,1);
      lcd.print("Score:");
      lcd.print(highScoresArray[currentHighScorePosition]);
      for(int i=7+5;i<15;i++)
        lcd.print(" ");
  }
  int currentJoyRead = readJoystick();
  if (currentJoyRead != lastJoyRead)
  {
    lastJoyRead = currentJoyRead;

    // clear line and print values to lcd
    

    switch (currentJoyRead)
    {
    case up:
    {
      if (currentHighScorePosition == 0 && menuDisplayItemType == "playerName")
        pointerMode = "exit";
      else
      {
        if (menuDisplayItemType == "playerScore")
        {
          menuDisplayItemType = "playerName";
        }
        else if (menuDisplayItemType == "playerName")
        {
          currentHighScorePosition--;
          menuDisplayItemType = "playerScore";
        }
      }
      break;
    }
    case down:
    {
      if (pointerMode == "scroll")
      {
        if (currentHighScorePosition >= highScoreRecords - 1 && menuDisplayItemType == "playerScore")
        {
          currentHighScorePosition = 0;
          menuDisplayItemType = "playerName";
        }
        else
        {
          if (menuDisplayItemType == "playerScore")
          {
            currentHighScorePosition++;
            menuDisplayItemType = "playerName";
          }
          else if (menuDisplayItemType == "playerName")
          {
            menuDisplayItemType = "playerScore";
          }
        }
      }
      else if (pointerMode == "exit")
      {
        pointerMode = "scroll";
      }
      break;
    }
    case enter:
    {
      if (pointerMode == "exit")
      {
        screenStatus = "mainMenu";
        pointerMode = "scroll";
        currentMenuIndex = 1;
      }
      break;
    }
    }
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

  int currentJoyRead = readJoystick();
  if (currentJoyRead != lastJoyRead)
  {
    lastJoyRead = currentJoyRead;
    switch (currentJoyRead)
    {
       case enter:
      {
        screenStatus = "mainMenu";
        pointerMode = "scroll";
        currentMenuIndex = 4;
        break;
      }
    case left:
    {
     
      if(currentContrast > 10) {
      currentContrast -= 10; }
      analogWrite(contrastPin, currentContrast);
      lcd.setCursor(12,0);
      lcd.print(currentContrast);
      lcd.print("   ");
      break;
    }
    case right:
    {
      if(currentContrast < 300) {
      currentContrast += 10; }
      analogWrite(contrastPin, currentContrast);
      lcd.setCursor(12,0);
      lcd.print(currentContrast);
      lcd.print("   ");
      break;
    }
    case up:
    {
      screenStatus = "mainMenu";
      pointerMode = "scroll";
      currentMenuIndex = 4;
      break;
    }
   
    }
  }
}

void displayScreenBrightnessMenu() {
  lcd.setCursor(0, 1);
  lcd.write((byte(2))); // left arrow

  lcd.setCursor(2, 1);
  lcd.print("BRIGHTNESS #1");
  lcd.setCursor(15, 1);
  lcd.write(byte(3)); // right arrow

  int currentJoyRead = readJoystick();
  if (currentJoyRead != lastJoyRead)
  {
    lastJoyRead = currentJoyRead;
    switch (currentJoyRead)
    {
       case enter:
      {
        screenStatus = "mainMenu";
        pointerMode = "scroll";
        currentMenuIndex = 4;
        break;
      }
    case left:
    {
     
      if(screenBrightness > 770) {
      screenBrightness -= 10; }
      analogWrite(screenBrightnessPin, screenBrightness);
       lcd.setCursor(12,0);
      lcd.print(screenBrightness);
      lcd.print("   ");
      break;
    }
    case right:
    {
      if(screenBrightness < 770) {
      screenBrightness += 10; }
      analogWrite(contrastPin, currentContrast);
       lcd.setCursor(12,0);
      lcd.print(screenBrightness);
      lcd.print("   ");
      break;
    }
    case up:
    {
      screenStatus = "mainMenu";
      pointerMode = "scroll";
      currentMenuIndex = 4;
      break;
    }
   
    }
  }
}

bool reverse = false;
int row = 0;
  int col = 0;

void displayMatrixBrightnessMenu() {
  lc.clearDisplay(0);                   // clear screen
  
  lcd.setCursor(0, 1);
  lcd.write((byte(2))); // left arrow
  
  lcd.setCursor(2, 1);
  lcd.print("BRIGHTNESS #2");
  lcd.setCursor(15, 1);
  lcd.write(byte(3)); // right arrow


  

  if(row < matrixSize && reverse == false) {
      
      if(millis() - ledTimer > ledTurnDelay) {
        ledTimer = millis();
        lc.setLed(0, col, row, true); // turns on LED at col, row
        col++;
      }
      if(col == matrixSize) {
        row++;
        col = 0;
      }
  }
  
  // Serial.println(row);
  Serial.println(col);
  if( (row == matrixSize && col == 0) ) {
    reverse = !reverse;
    row = 0;
    col = 0;
  }

  if(row < matrixSize && reverse == true) {
      
      if(millis() - ledTimer > ledTurnDelay) {
        ledTimer = millis();
        lc.setLed(0, col, row, false); // turns on LED at col, row
        col++;
      }
      if(col == matrixSize) {
        row++;
        col = 0;
      }
  }
  



 
  int currentJoyRead = readJoystick();
  if (currentJoyRead != lastJoyRead)
  {
    lastJoyRead = currentJoyRead;
    switch (currentJoyRead)
    {
       case enter:
      {
        screenStatus = "mainMenu";
        pointerMode = "scroll";
        currentMenuIndex = 6;
        break;
      }
    case left:
    {
     
      if(matrixBrightness > 1) {
      matrixBrightness -= 1; }
      lc.setIntensity(0,matrixBrightness);
       lcd.setCursor(12,0);
      lcd.print(matrixBrightness);
      lcd.print("   ");
      break;
    }
    case right:
    {
      if(matrixBrightness < 20) {
      matrixBrightness += 1; }
      lc.setIntensity(0,matrixBrightness);
       lcd.setCursor(12,0);
      lcd.print(matrixBrightness);
      lcd.print("   ");
      break;
    }
    case up:
    {
      screenStatus = "mainMenu";
      pointerMode = "scroll";
      currentMenuIndex = 6;
      break;
    }
   
    }
  }
}

void handleMenu()
{

  

  if (screenStatus == "mainMenu")
  {
    displayMainMenu();
  }
  else if (screenStatus == "highScores")
  {
    displayHighScores();
  }
  else if (screenStatus == "changeContrast")
  {
    displayContrastSettings();
  }
  else if (screenStatus == "about")
  {
    displayAbout();
  }
else if  (screenStatus == "brightnessMatrix") {
    displayMatrixBrightnessMenu();
}
if (screenStatus == "brightnessScreen") {
    displayScreenBrightnessMenu();
}

  if (screenStatus == "game")
  {
    int gameOver = runGame();
    if (gameOver == -1)
    {
      screenStatus = "mainMenu";
      pointerMode = "scroll";
      currentMenuIndex = 0;
    }
  }

  // flashing pointer
  if (pointerMode != "noPointer")
  {
    if (millis() - blinkTimer > resetBlinkDelay)
    {
      blinkTimer = millis();
    }

    if (millis() - blinkTimer < blinkDelay)
    {
      if (pointerMode == "scroll")
      {
        
          lcd.setCursor(15, 1);
          lcd.write((byte)1); // print down arrow
      }
      else if (pointerMode == "select")
      {
        lcd.setCursor(13, 1);
        lcd.write((byte)2); // print left arrow
      }
      else if (pointerMode == "exit")
      {
        lcd.setCursor(15, 1);
        lcd.write((byte)0); // print up arrow
      }
      else if (pointerMode == "contrastScroll")
      {
        lcd.setCursor(8, 0);
        lcd.write((byte)5); // 2 arrows
      }
    }
    if (millis() - blinkTimer > blinkDelay)
    {
      if (pointerMode == "scroll")
      {
        lcd.setCursor(15, 1);
        lcd.write("   ");
      }
      else if (pointerMode == "select")
      {
        lcd.setCursor(13, 1);
        lcd.write("   ");
      }
      else if (pointerMode == "contrastScroll")
      {
        lcd.setCursor(8, 0);
        lcd.write("    ");
      }
    }
  }
}