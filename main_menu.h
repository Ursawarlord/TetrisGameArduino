#include "utils.h"
#include "tetris_game.h"


//Arduino pins attached to joystick
const byte joystickButtonPin = 9;
const byte joystickPinX = A1;
const byte joystickPinY = A0;

//Joystick values
#define up    0
#define right 1
#define down  2
#define left  3
#define enter 4
#define none  5





const int waitTime = 300;
unsigned long blinkTimer;
const int resetBlinkTimer = 350;


int eepromOffset = 0;
byte currentOffset = 0;

int highScoreRecords = 0;


int writeStringToEEPROM(int addrOffset, const String &strToWrite)
{
  byte len = strToWrite.length();
  EEPROM.update(addrOffset, len);
  for (int i = 0; i < len; i++)
  {
    EEPROM.update(addrOffset + 1 + i, strToWrite[i]);
  }
  return addrOffset + 1 + len;
}

int readStringFromEEPROM(int addrOffset, String *strToRead)
{
  int newStrLen = EEPROM.read(addrOffset);
  char data[newStrLen + 1];
  for (int i = 0; i < newStrLen; i++)
  {
    data[i] = EEPROM.read(addrOffset + 1 + i);
  }
  data[newStrLen] = '\0'; // !!! NOTE !!! Remove the space between the slash "/" and "0" (I've added a space because otherwise there is a display bug)
  *strToRead = String(data);
  return addrOffset + 1 + newStrLen;
}



int readJoystick() {
  int output = none;
  // read all joystick values
  int X_Axis = analogRead(joystickPinX);     // read the x axis value
  int Y_Axis = analogRead(joystickPinY);     // read the y axis value
  // Y_Axis = map(Y_Axis, 0, 1023, 1023, 0);      // invert the input from the y axis so that pressing the stick forward gives larger values
  int SwitchValue = digitalRead(joystickButtonPin);  // read the state of the switch
  SwitchValue = map(SwitchValue, 0, 1, 1, 0);  // invert the input from the switch to be high when pressed

  if (SwitchValue == 1) {
    output = enter;
  } else if (X_Axis >= 900) {
    output = right;
  } else if (X_Axis <= 100) {
    output = left;
  } else if (Y_Axis >= 900) {
    output = up;
  } else if (Y_Axis <= 100) {
    output = down;
  }
  return output;
}


// Menus
String mainMenuText[] = {"1.Play game", "2.High score", "3.Change name", "4. Contrast",  "5.About"};
int mainMenuCount = sizeof (mainMenuText) / sizeof (mainMenuText [0]);

String currentMenuTitle = "mainMenu";

String currentPointMode = "scroll"; // option 0 is scroll | option 1 is select



// Current menu and item to be displayed
int currentMenuIndex;
// Used to check joystick state
int lastJoyRead;


void print_line(int line, String text) {
  lcd.setCursor(0, line);
  lcd.print("               ");
  lcd.setCursor(0, line);
  lcd.print(text);

}

void moveUpMainMenu() {
  if (currentMenuIndex <= 0) {
    currentMenuIndex = mainMenuCount - 1;
  } else {
    currentMenuIndex--;
  }
}

void moveDownMainMenu() {
  if (currentMenuIndex >= mainMenuCount - 1) {
    currentMenuIndex = 0;
  } else {
    currentMenuIndex++;
  }
}



void moveRightMainMenu() {
  if (currentPointMode == "select")
    currentPointMode = "scroll";
}
void moveLeftMainMenu() {
  if (currentPointMode == "scroll")
    currentPointMode = "select";
}

String highScorePlayersArray[15];
unsigned long highScoresArray[15];
String displayMode = "name"; // switch name or score display

void launchHighScoreMenu() {
  currentMenuTitle = "highScores";
  currentPointMode = "scroll";
  lcd.clear();

  int lastOffset = eepromOffset;
  String newStr;
  for (int i = 0; i < 15; i++) {
    lastOffset  = readStringFromEEPROM(lastOffset, &newStr);
    highScorePlayersArray[i] = newStr;
    highScoresArray[i] = newStr.length() * 250 / 25; // experimental
  }
  currentMenuIndex = 0;
}




void launchAboutMenu () {
  currentMenuTitle = "about";

  currentPointMode = "noFlashes";
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("                                  ");
  lcd.setCursor(0, 1);
  lcd.print("                                 ");
  lcd.clear(); // resets cursor to upper-left

  lcd.print("Madalin Frincu 332 ");
  lcd.setCursor(0, 1);
  lcd.print("www.github.com/ursawarlord/");
  delay(1000);


}


void launchTetrisGame () {
  currentMenuTitle = "game";

  currentPointMode = "noFlashes";
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("                                  ");
  lcd.setCursor(0, 1);
  lcd.print("                                 ");
  lcd.clear(); // resets cursor to upper-left

  tetris_setup();

}



void moveEnterMainMenu() {
  if (currentPointMode == "select")  {
    switch (currentMenuIndex) {
      case 0:
        launchTetrisGame();
        break;
      case 1:
        launchHighScoreMenu();
        break;
      case 2:
        //currentMenuTitle = "nameSettings";
        break;
      case 3:
        //currentMenuTitle = "changeContrast";
        break;
      case 4:
        launchAboutMenu();
        break;
      default:
        break;
    }
  }
}


void displayMainMenu() {
  lcd.setCursor(0, 0);
  lcd.print("Main Menu:");
  lcd.setCursor(0, 1);
  lcd.print(mainMenuText[currentMenuIndex]);
  byte currentJoyRead = readJoystick();

  if (currentJoyRead != lastJoyRead) {
    lastJoyRead = currentJoyRead;
    // clear line and print values to lcd
    print_line(1, mainMenuText[currentMenuIndex]);

    switch (currentJoyRead) {
      case up: {
          if (currentPointMode == "scroll")
            moveUpMainMenu();
          break;
        }
      case down: {
          if (currentPointMode == "scroll")
            moveDownMainMenu();
          break;
        }
      case right:
        moveRightMainMenu();
        break;
      case left:
        moveLeftMainMenu();
        break;
      case enter: {
          moveEnterMainMenu();
          break;
        }
      default: break;
    }


  }

}





void setupMainMenu() {
  lcd.begin(16, 2);

  String player1 = "MADALIN";
  String player2 = "ANDREI";


  byte addrOffsetPlayer1 = writeStringToEEPROM(eepromOffset, player1);
  highScoreRecords++;

  writeStringToEEPROM(addrOffsetPlayer1, player2);
  highScoreRecords++;



  // set up joy pins
  pinMode(joystickButtonPin, INPUT_PULLUP);


  static byte upArrowChar[8] = {
    0b00100,
    0b01110,
    0b11111,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b00100
  };

  static byte downArrowChar[8] = {
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b11111,
    0b01110,
    0b00100
  };

  static byte leftArrowChar[8] = {
    B00000,
    B00100,
    B01000,
    B11111,
    B01000,
    B00100,
    B00000,
    B00000
  };

  static byte rightArrowChar[8] = {
    B00000,
    B00100,
    B00010,
    B11111,
    B00010,
    B00100,
    B00000,
    B00000
  };


  lcd.createChar(0, upArrowChar); // create a new custom character (index 0)
  lcd.createChar(1, downArrowChar); // create a new custom character (index 1)
  lcd.createChar(2, leftArrowChar); // create a new custom character (index 2)
  lcd.createChar(3, rightArrowChar); // create a new custom character (index 3)

  // Init vars
  currentMenuIndex = 0;
  lastJoyRead = none;
}

void launchMainMenu() {

  setupMainMenu();
  currentMenuTitle = "mainMenu";
  currentPointMode = "scroll";
  currentMenuIndex = 0;
  lcd.clear();
}






unsigned long scrollTimer = 0;
byte resetScrollTimer = 150;
byte scrollCount = 0;
bool goScroll = false;

const byte resetTimer = 150;


void displayAbout() {

  if (millis() - scrollTimer > resetTimer) {
    scrollTimer = millis();
    scrollCount++;
    goScroll = true;
  }

  if (scrollCount > 13 + 17 + 4) {
    scrollCount = 0;
  }

  if (goScroll) {
    goScroll = false;
    if (scrollCount < 13) {
      lcd.scrollDisplayLeft();
    }
    else if (scrollCount < 13 + 17 && scrollCount > 13) {
      lcd.scrollDisplayRight();
    }
    else if (scrollCount < 13 + 17 + 4 && scrollCount > 13 + 17 ) {
      lcd.scrollDisplayLeft();
    }
  }


  byte currentJoyRead = readJoystick();
  if (currentJoyRead != lastJoyRead) {
    lastJoyRead = currentJoyRead;
    switch (currentJoyRead) {
      case enter: {
          launchMainMenu();
          break;
        }
      default: break;
    }
  }
}




byte currentHighScorePosition = 0;

void moveUpHighScore() {
  if (currentHighScorePosition == 0 && displayMode == "name")
    currentPointMode = "exit";
  else {
    if (displayMode == "score") {
      displayMode = "name";
    }
    else if (displayMode == "name") {
      currentHighScorePosition--;
      displayMode = "score";
    }
  }
}

void moveDownHighScore() {

  if (currentPointMode == "scroll") {
    if (currentHighScorePosition >= highScoreRecords - 1 && displayMode == "score") {
      currentHighScorePosition = 0;
      displayMode = "name";
    } else {
      if (displayMode == "score") {
        currentHighScorePosition++;
        displayMode = "name";
      }
      else if (displayMode == "name") {
        displayMode = "score";
      }
    }
  }
  else if (currentPointMode == "exit") {
    currentPointMode = "scroll";
  }
}



void displayHighScores() {
  lcd.setCursor(0, 0);
  lcd.print("High scores:");
  if (currentHighScorePosition == 0) {
    lcd.setCursor(13, 0);
    lcd.print("x");
  }



  byte currentJoyRead = readJoystick();
  if (currentJoyRead != lastJoyRead) {

    // clear line and print values to lcd
    if (displayMode == "name")
      print_line(1, String(currentHighScorePosition + 1) + "." + highScorePlayersArray[currentHighScorePosition]);
    else if (displayMode == "score") {
      print_line(1, "Score: " + String(highScoresArray[currentHighScorePosition]));
    }

    lastJoyRead = currentJoyRead;
    switch (currentJoyRead) {
      case up: {
          moveUpHighScore();
          break;
        }
      case down: {
          moveDownHighScore();
          break;
        }
      case enter: {
          if (currentPointMode == "exit")  {
            launchMainMenu();
          }
          break;
        }
      default: break;
    }



  }


}



void main_menu_loop() {


  if (currentMenuTitle == "mainMenu") {
    displayMainMenu();

  }
  else if (currentMenuTitle == "highScores") {
    displayHighScores();
  }
  else if (currentMenuTitle == "about") {
    displayAbout();
  }
  else if (currentMenuTitle == "game") {
    int gameOver = tetris_loop();
    if (gameOver == -1) {
      launchMainMenu();
    }
  }


  // flash the text
  if (currentPointMode != "noFlashes") {
    if (millis() - blinkTimer < waitTime) {
      if (currentPointMode == "scroll") {
        if (currentMenuTitle == "mainMenu") {
          lcd.setCursor(15, 1);
        }
        else {
          lcd.setCursor(13, 1);
        }
        lcd.write((byte)1);  // print down arrow
      }
      else if (currentPointMode == "select") {
        lcd.setCursor(13, 1);
        lcd.write((byte)2);  // print left arrow
      }
      else if (currentPointMode == "exit") {
        lcd.setCursor(13, 1);
        lcd.write((byte)0);  // print up arrow
      }
    }
  }

  // flash the text again
  if (currentPointMode != "noFlashes") {

    if (millis() - blinkTimer > resetBlinkTimer) {
      blinkTimer = millis();
    }

    if (millis() - blinkTimer > waitTime) {
      if (currentPointMode == "scroll" ) {
        if (currentMenuTitle == "mainMenu") {
          lcd.setCursor(15, 1);
        }
        else {
          lcd.setCursor(13, 1);
        }
        lcd.write("   ");
      }
      else if (currentPointMode == "select") {
        lcd.setCursor(13, 1);
        lcd.write("   ");
      }
    }
  }


}
