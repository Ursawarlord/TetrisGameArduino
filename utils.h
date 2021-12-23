#pragma once
#ifndef UTILS_H
#define UTILS_H

#include "myLedControl.h"
#include "registerIO.h"
#include <EEPROM.h>
#include "myLiquidCrystal.h"

typedef char int_8;
typedef byte uint_8;
#define BUZZER A4
#define UNUSED_ANALOG A3 // for random seed
#define BLOCK_COUNT (sizeof(blocks) / sizeof(blocks[0]))

#define STATUS_GAME 701
#define STATUS_HIGHSCORES 702
#define STATUS_SETTINGS 703
#define STATUS_HOWTOPLAY 704
#define STATUS_ABOUT 705
#define STATUS_MAINMENU 706
#define STATUS_SHOWNAME 707
#define STATUS_CHANGENAME 708
#define STATUS_CONTRAST 709
#define STATUS_RESETMEMORY 710
#define STATUS_BRIGHTNESSMATRIX 711
#define STATUS_BRIGHTNESSLCD 712
#define STATUS_CONFIGSYSTEM 713
#define POINTER_SCROLL 720
#define POINTER_SELECT 721
#define POINTER_EXIT 722
#define POINTER_NOTHING 723
#define POINTER_CONTRAST 724


const byte rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int dinPin = 12;
const int clockPin = 11;
const int loadPin = 13;

const int LCDBrightnessPin = 10;
int LCDBrightness;

const byte matrixSize = 8;
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);
byte matrixBrightness;

const int eepromSize = 1024;

const byte eepromHighScoreStartOffset = 0;
const byte eepromPlayerNameStartOffset = 80;
byte highScoreRecords = 0;
const byte eepromHighScoreRecordsAddress = 8;

const byte eepromScreenBrightnessAddress = 10;
const byte eepromContrastAddress = 12;
const byte eepromMatrixBrightnessAddress = 14;
const byte eepromReverseJoystickAddress = 16;
const byte eepromDifficultyAddress = 18;
const byte eepromAppInitSuccessfullyAddress = 20;

const byte eepromCurrentUsernameStartOffset = 500;
String currentUsername;

//read Serial Monitor for Change name
String inputString = "";
char incomingByte = 0;
bool stringComplete = false;

//Arduino pins attached to joystick
const int buttonPin = 8;
const int joystickPinX = A0;
const int joystickPinY = A1;
const int pauseButtonPin = A2;

bool gamePaused = false;

//Joystick values
#define up 0
#define right 1
#define down 2
#define left 3
#define enter 4
#define none 5
#define pause 6
bool reverseJoystick = false;
int lastInputRead = none;

//Menu
int screenStatus;
int pointerMode;
byte mainMenuCount = 5;
byte settingsMenuCount = 8;
byte configMenuCount = 3;
int currentMainMenuIndex;
int currentMenuSettingsIndex;
int currentConfigSettingsIndex;

// Difficulty
#define EASY 60
#define MEDIUM 61
#define HARD 62
byte difficultyValue;

#define OFF 102
#define ON  103
byte appInitSuccessfully;

unsigned long lastPress = 0;
int debounceDelay = 800;

const int maxThreshold = 650;
const int minThreshold = 350;

int contrastPin = 9;
unsigned int currentContrast;

void printLine(int line, String text)
{
  lcd.setCursor(0, line);

  int spaceRemaining = 16 - text.length();

  for (int i = 0; i < text.length(); i++)
  {
    lcd.print(text[i]);
  }
  // lcd.print(text);
  for (int i = 0; i < 16; i++)
  {
    lcd.print(" ");
  }
}



int writeStringToEEPROM(int addrOffset, const String &strToWrite)
{
  byte len = strToWrite.length();
  EEPROM.update(addrOffset, len);
  for (int i = 0; i < len; i++)
  {
    EEPROM.update(addrOffset + 1 + i, strToWrite[i]);
  }
  return  1 + len;
}

int writeStringToEEPROM2(int addrOffset, const String &strToWrite)
{
  byte len = strToWrite.length();
  EEPROM.update(addrOffset, len);
  for (int i = 0; i < len; i++)
  {
    EEPROM.update(addrOffset + 1 + i, strToWrite[i]);
  }
  return  addrOffset + 1 + len;
}



int readStringFromEEPROM(int addrOffset, String *strToRead)
{
  int newStrLen = EEPROM.read(addrOffset);
  char data[newStrLen + 1];
  for (int i = 0; i < newStrLen; i++)
  {
    data[i] = EEPROM.read(addrOffset + 1 + i);
  }
   data[newStrLen] = '\0';
  *strToRead = String(data);
  return  1 + newStrLen;
}


void writeIntIntoEEPROM(int address, int number)
{
  EEPROM.write(address, number >> 8);
  EEPROM.write(address + 1, number & 0xFF);
}

int readIntFromEEPROM(int address)
{
  return (EEPROM.read(address) << 8) + EEPROM.read(address + 1);
}

byte upArrowChar[8] = {
    0b00100,
    0b01110,
    0b11111,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b00100};

byte scrollArrowChar[8] = {
    0b00100,
    0b01110,
    0b11111,
    0b00100,
    0b00100,
    0b11111,
    0b01110,
    0b00100};

byte downArrowChar[8] = {
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b11111,
    0b01110,
    0b00100};

byte leftArrowChar[8] = {
    B00000,
    B00100,
    B01000,
    B11111,
    B01000,
    B00100,
    B00000,
    B00000};

byte rightArrowChar[8] = {
    B00000,
    B00100,
    B00010,
    B11111,
    B00010,
    B00100,
    B00000,
    B00000};

byte horizontalArrowsChar[8] = {
    B01000,
    B11111,
    B01000,
    B00000,
    B00000,
    B00010,
    B11111,
    B00010};

int readInput()
{
  int output = none;
  // read all joystick values
  int X_Axis = analogRead(joystickPinX);      // read the x axis value
  int Y_Axis = analogRead(joystickPinY);      // read the y axis value
  int buttonValue = registerReadPin(buttonPin);   // read the state of the button
  buttonValue = map(buttonValue, 0, 1, 1, 0); // invert the input from the button to be high when pressed
  if(reverseJoystick) {
    X_Axis = map(X_Axis, 0, 1024, 1024, 0);
  }
  int pauseButtonValue = registerReadPin(pauseButtonPin);
  pauseButtonValue = map(pauseButtonValue, 0, 1, 1, 0); // invert the input from the button to be high when pressed

  if (buttonValue == 1 && millis() - lastPress > debounceDelay)
  {
    lastPress = millis();
    output = enter;
  }
  else if (pauseButtonValue == 1 && millis() - lastPress > debounceDelay)
  {
    lastPress = millis();
    output = pause;
  }
  else if (X_Axis >= maxThreshold)
  {
    output = right;
  }
  else if (X_Axis <= minThreshold)
  {
    output = left;
  }
  else if (Y_Axis >= maxThreshold)
  {
    output = up;
  }
  else if (Y_Axis <= minThreshold)
  {
    output = down;
  }
  return output;
}


void initDefaults() {
  String defaultName = "PLAYER";
  writeIntIntoEEPROM(eepromDifficultyAddress, EASY);
  writeIntIntoEEPROM(eepromMatrixBrightnessAddress, 2);
  writeIntIntoEEPROM(eepromContrastAddress, 100);
  writeIntIntoEEPROM(eepromScreenBrightnessAddress, 950);
  writeIntIntoEEPROM(eepromReverseJoystickAddress, 0);
  writeIntIntoEEPROM(eepromAppInitSuccessfullyAddress, ON);
  writeStringToEEPROM(eepromCurrentUsernameStartOffset, defaultName);
}

int saveHighScore(const String &playerName, const unsigned int &score)
{
  int place = 0;
  int score1, score2, score3;
  String name1, name2, name3;
  int length1, length2, length3, length;
  highScoreRecords = readIntFromEEPROM(eepromHighScoreRecordsAddress);
  if (highScoreRecords > 3 || highScoreRecords < 0)
  {
    highScoreRecords = 0;
  }

  switch (highScoreRecords)
  {
  case 0: {
    writeIntIntoEEPROM(eepromHighScoreStartOffset, score);
    writeStringToEEPROM(eepromPlayerNameStartOffset, playerName);
    place = 1;
    String d;
    readStringFromEEPROM(eepromPlayerNameStartOffset, &d);
    Serial.println(d);
    
    break;
  }
  case 1:{
    Serial.println("ALO");
    score1 = readIntFromEEPROM(eepromHighScoreStartOffset);
    if (score > score1)
    {
      writeIntIntoEEPROM(eepromHighScoreStartOffset, score);
      writeIntIntoEEPROM(eepromHighScoreStartOffset + sizeof(int), score1);
      readStringFromEEPROM(eepromPlayerNameStartOffset, &name1);
      length1 = writeStringToEEPROM(eepromPlayerNameStartOffset, playerName);
      writeStringToEEPROM( (eepromPlayerNameStartOffset + length1), name1);
      place = 1;
    }
    else
    {
      writeIntIntoEEPROM(eepromHighScoreStartOffset + sizeof(int), score);
      length1 = readStringFromEEPROM(eepromPlayerNameStartOffset, &name1);
      writeStringToEEPROM(eepromPlayerNameStartOffset + length1, playerName);
      place = 2;
    }
    break;}
  case 2:{
    score1 = readIntFromEEPROM(eepromHighScoreStartOffset);
    score2 = readIntFromEEPROM(eepromHighScoreStartOffset + sizeof(int));
    if (score > score1)
    {
      writeIntIntoEEPROM(eepromHighScoreStartOffset, score);
      writeIntIntoEEPROM(eepromHighScoreStartOffset + sizeof(int), score1);
      writeIntIntoEEPROM(eepromHighScoreStartOffset + 2 * sizeof(int), score2);
      length1 = readStringFromEEPROM(eepromPlayerNameStartOffset, &name1);
      readStringFromEEPROM(eepromPlayerNameStartOffset+length1, &name2);
      length  = writeStringToEEPROM(eepromPlayerNameStartOffset, playerName);
      writeStringToEEPROM(eepromPlayerNameStartOffset+length, name1);
      writeStringToEEPROM(eepromPlayerNameStartOffset+length+length1, name2);
      Serial.println(eepromPlayerNameStartOffset+length+length1);
      place = 1;
    }
    else if (score > score2)
    {
      writeIntIntoEEPROM(eepromHighScoreStartOffset + sizeof(int), score);
      writeIntIntoEEPROM(eepromHighScoreStartOffset + 2 * sizeof(int), score2);
      length1 = readStringFromEEPROM(eepromPlayerNameStartOffset, &name1);
      readStringFromEEPROM(eepromPlayerNameStartOffset+length1, &name2);
      length  = writeStringToEEPROM(eepromPlayerNameStartOffset+length1, playerName);
      writeStringToEEPROM(eepromPlayerNameStartOffset+length1+length, name2);
      place = 2;
    }
    else
    {
      writeIntIntoEEPROM(eepromHighScoreStartOffset + 2 * sizeof(int), score);
      length1 = readStringFromEEPROM(eepromPlayerNameStartOffset, &name1); 
      length2 = readStringFromEEPROM(eepromPlayerNameStartOffset+length1, &name2);
      writeStringToEEPROM(eepromPlayerNameStartOffset+length1+length2, playerName);
       place = 3;

    }
    break;
  }
  case 3:{
    score1 = readIntFromEEPROM(eepromHighScoreStartOffset);
    score2 = readIntFromEEPROM(eepromHighScoreStartOffset + sizeof(int));
    score3 = readIntFromEEPROM(eepromHighScoreStartOffset + 2 * sizeof(int));
    if (score > score1)
    {
      writeIntIntoEEPROM(eepromHighScoreStartOffset, score);
      writeIntIntoEEPROM(eepromHighScoreStartOffset + sizeof(int), score1);
      writeIntIntoEEPROM(eepromHighScoreStartOffset + 2 * sizeof(int), score2);
      length1 = readStringFromEEPROM(eepromPlayerNameStartOffset, &name1);
      length2 = readStringFromEEPROM(eepromPlayerNameStartOffset+length1, &name2);
      length =  writeStringToEEPROM(eepromPlayerNameStartOffset, playerName);
      writeStringToEEPROM(eepromHighScoreStartOffset+length, name1);
      writeStringToEEPROM(eepromHighScoreStartOffset+length+length1, name2); 
      place = 1;     
    }
    else if (score > score2)
    {
      writeIntIntoEEPROM(eepromHighScoreStartOffset + sizeof(int), score);
      writeIntIntoEEPROM(eepromHighScoreStartOffset + 2 * sizeof(int), score2);
      length1 = readStringFromEEPROM(eepromPlayerNameStartOffset, &name1);
      readStringFromEEPROM(eepromPlayerNameStartOffset+length1, &name2);
      length = writeStringToEEPROM(eepromPlayerNameStartOffset+length1, playerName);
      writeStringToEEPROM(eepromHighScoreStartOffset+length1+length, name2); 
      place = 2;
    }
    else if (score > score3)
    {
      writeIntIntoEEPROM(eepromHighScoreStartOffset + 2 * sizeof(int), score);
      length1 = readStringFromEEPROM(eepromPlayerNameStartOffset, &name1);
      length2 = readStringFromEEPROM(eepromPlayerNameStartOffset+length1, &name2);
      writeStringToEEPROM(eepromPlayerNameStartOffset+length1+length2,playerName);
      place = 3;
    }
    else
    {
      // Report if we didn't save the score
      return -1;
    }
    break;
  }
  }
  // Report that we have saved the score
  if(highScoreRecords < 3) {
    highScoreRecords++;
  }
  writeIntIntoEEPROM(eepromHighScoreRecordsAddress, highScoreRecords);
  return place;
}

#endif
