#include "utils.h"
#include "game.h"

unsigned long blinkTimer;
const int blinkDelay = 300;
const int resetBlinkDelay = 350;

int_8 eepromOffset = 0;
int_8 currentOffset = 0;

int_8 highScoreRecords = 0;
int_8 currentHighScorePosition = 0;

// Menus
String mainMenuText[] = {"1.Play game", "2.High score", "3.Change name", "4. Contrast", "5.About"};
int_8 mainMenuCount = sizeof(mainMenuText) / sizeof(mainMenuText[0]);
String screenStatus;
String pointerMode;
// Current menu and item to be displayed
int_8 currentMenuIndex;
// Used to check joystick state
int_8 lastJoyRead = none;
String highScorePlayerName[15];
unsigned long highScoresArray[15];
String menuDisplayItemType;

const int resetScrollTime = 150;
unsigned long scrollTimer = 0;
int_8 scrollCount = 0;
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
      if (pointerMode == "select")
      {
        switch (currentMenuIndex)
        {
        case 0:
          screenStatus = "game";
          pointerMode = "noPointer";
          bootGame();
          break;
        case 1:
          screenStatus = "highScores";
          pointerMode = "scroll";
          lcd.clear();
          int lastOffset = eepromOffset;
          for (int i = 0; i < 15; i++)
          {
            lastOffset = readStringFromEEPROM(lastOffset, &newStr);
            highScorePlayerName[i] = newStr;
            highScoresArray[i] = newStr.length() * 250 / 25; // experimental
          }
          currentMenuIndex = 0;
          break;
        case 2:
          //screenStatus = "nameSettings";
          break;
        case 3:
          //screenStatus = "changeContrast";
          break;
        case 4:
          screenStatus = "about";
          pointerMode = "noPointer";
          // lcd.begin(16, 2);
          lcd.setCursor(0, 0);
          lcd.print("Madalin Frincu 332 ");
          lcd.print("                                  ");
          lcd.setCursor(0, 1);
          lcd.print("www.github.com/ursawarlord/");
          lcd.print("                                  ");
          break;
        }
      }
      break;
    }
  }
}

void highScoreMemoryInit()
{
  String player1 = "MADALIN";
  String player2 = "ANDREI";

  byte addrOffsetPlayer1 = writeStringToEEPROM(eepromOffset, player1);
  highScoreRecords++;
  eepromOffset = addrOffsetPlayer1;

  writeStringToEEPROM(eepromOffset, player2);
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
      currentMenuIndex = 0;
      lcd.home();
      break;
    }
  }
}

void moveUpHighScore()
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
}

void moveDownHighScore()
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
}

void displayHighScores()
{
  lcd.setCursor(0, 0);
  lcd.print("High scores:");
  if (currentHighScorePosition == 0)
  {
    lcd.setCursor(13, 0);
    lcd.print("x");
  }

  byte currentJoyRead = readJoystick();
  if (currentJoyRead != lastJoyRead)
  {

    // clear line and print values to lcd
    if (menuDisplayItemType == "playerName")
      printLine(1, String(currentHighScorePosition + 1) + "." + highScorePlayerName[currentHighScorePosition]);
    else if (menuDisplayItemType == "playerScore")
    {
      printLine(1, "Score: " + String(highScoresArray[currentHighScorePosition]));
    }

    lastJoyRead = currentJoyRead;
    switch (currentJoyRead)
    {
    case up:
    {
      moveUpHighScore();
      break;
    }
    case down:
    {
      moveDownHighScore();
      break;
    }
    case enter:
    {
      if (pointerMode == "exit")
      {
        screenStatus = "mainMenu";
        pointerMode = "scroll";
        currentMenuIndex = 0;
      }
      break;
    }
    default:
      break;
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
  else if (screenStatus == "about")
  {
    displayAbout();
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
        if (screenStatus == "mainMenu")
        {
          lcd.setCursor(15, 1);
        }
        else
        {
          lcd.setCursor(13, 1);
        }
        lcd.write((byte)1); // print down arrow
      }
      else if (pointerMode == "select")
      {
        lcd.setCursor(13, 1);
        lcd.write((byte)2); // print left arrow
      }
      else if (pointerMode == "exit")
      {
        lcd.setCursor(13, 1);
        lcd.write((byte)0); // print up arrow
      }
    }
    if (millis() - blinkTimer > blinkDelay)
    {
      if (pointerMode == "scroll")
      {
        if (screenStatus == "mainMenu")
        {
          lcd.setCursor(15, 1);
        }
        else
        {
          lcd.setCursor(13, 1);
        }
        lcd.write("   ");
      }
      else if (pointerMode == "select")
      {
        lcd.setCursor(13, 1);
        lcd.write("   ");
      }
    }
  }
}