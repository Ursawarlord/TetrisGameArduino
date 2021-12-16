#include "utils.h"
#include "game.h"

unsigned long blinkTimer;
const int blinkDelay = 300;
const int resetBlinkDelay = 350;

int eepromOffset = 0;
int currentEepromOffset = 0;

int highScoreRecords = 0;
int currentHighScorePosition = 0;

// Menus
String mainMenuText[] = {"1.Play game", "2.High score", "3.Change name", "4.Contrast", "5.About"};// "6.Brightness #1", "7.Brightness #2"};
int mainMenuCount = sizeof(mainMenuText) / sizeof(mainMenuText[0]);
String screenStatus;
String pointerMode;
// Current menu and item to be displayed
int currentMenuIndex;
// Used to check joystick state
int lastJoyRead = none;
String highScorePlayerName[15];
unsigned long highScoresArray[15];
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
  Serial.println(currentJoyRead);
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
          screenStatus = "brightnessMatrix";
          pointerMode = "contrastScroll";
          lcd.clear();
          break;
        }
        case 6:
        {
          screenStatus = "brightnessScreen";
          pointerMode = "contrastScroll";
          lcd.clear();
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
      // printInLine(0,String(currentContrast), 12);
      break;
    }
    case right:
    {
      if(currentContrast < 300) {
      currentContrast += 10; }
      analogWrite(contrastPin, currentContrast);
      // lcd.print(currentContrast);
      // printInLine(0, String(currentContrast), 12);
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
  lcd.write("BRIGHTNESS #1");
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
      // printInLine(0,String(currentContrast), 12);
      break;
    }
    case right:
    {
      if(currentContrast < 300) {
      currentContrast += 10; }
      analogWrite(contrastPin, currentContrast);
      // lcd.print(currentContrast);
      // printInLine(0, String(currentContrast), 12);
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

void displayMatrixBrightnessMenu() {

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
// else if  (screenStatus == "brightnessMatrix") {
//     displayMatrixBrightnessMenu();
// }
// else if (screenStatus == "brightnessScreen") {
//     displayScreenBrightnessMenu();
// }

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