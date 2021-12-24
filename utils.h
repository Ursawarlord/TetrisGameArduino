/*
 * utils.h contains much of the variables
 * of the TetrisGameArduino project 
 * and has utility functions for hardware
 * such as input and eeprom I/O
 */ 

#pragma once
#ifndef UTILS_H
#define UTILS_H

#include "myLedControl.h"
#include "registerIO.h"
#include <EEPROM.h>
#include "myLiquidCrystal.h"


#define BUZZER A3
#define UNUSED_ANALOG A4 // for random seed
#define BLOCK_COUNT (sizeof(blocks) / sizeof(blocks[0]))

/* 
 * Status values for LCD display
 * Pointer values for menu experience
 */
#define STATUS_GAME 101
#define STATUS_HIGHSCORES 102
#define STATUS_SETTINGS 103
#define STATUS_HOWTOPLAY 104
#define STATUS_ABOUT 105
#define STATUS_MAINMENU 106
#define STATUS_SHOWNAME 107
#define STATUS_CHANGENAME 108
#define STATUS_CONTRAST 109
#define STATUS_RESETMEMORY 110
#define STATUS_BRIGHTNESSMATRIX 111
#define STATUS_BRIGHTNESSLCD 112
#define STATUS_CONFIGSYSTEM 113
#define POINTER_SCROLL 120
#define POINTER_SELECT 121
#define POINTER_EXIT 122
#define POINTER_NOTHING 123
#define POINTER_CONTRAST 124

//Joystick values
#define up 0
#define right 1
#define down 2
#define left 3
#define enter 4
#define none 5
#define pause 6

// LCD display
const byte rs = 7,
    en = 6,
    d4 = 5,
    d5 = 4,
    d6 = 3,
    d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const int LCDBrightnessPin = 10;
const int contrastPin = 9;
int LCDBrightness;
int currentContrast;


//Arduino pins attached to joystick
const int joystickPinX = A0;
const int joystickPinY = A1;

// Dedicated hardware buttons
const int buttonPin = 8;
const int pauseButtonPin = A2;

// 8x8 LED Matrix
const byte matrixSize = 8;
const int dinPin = 12;
const int clockPin = 11;
const int loadPin = 13;
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);
byte matrixBrightness;

/* 
 * EEPROM address starting offsets for String/INT arrays 
 */
const byte eepromHighScoreStartOffset = 0;
const byte eepromPlayerNameStartOffset = 80;
const byte eepromCurrentUsernameStartOffset = 220;

// Variables
const int eepromSize = 256;
bool gamePaused = false;
byte highScoreRecords = 0;
String currentUsername;
String defaultUsername = "PLAYER";
int reverseJoystick = 0;
int lastInputRead = none;

// Variables for menu
const byte mainMenuCount = 5;
const byte settingsMenuCount = 8;
const byte configMenuCount = 3;
byte screenStatus;
byte pointerMode;
byte currentMainMenuIndex;
byte currentMenuSettingsIndex;
byte currentConfigSettingsIndex;

/*
 * EEPROM address for single variable 
 */
const byte eepromHighScoreRecordsAddress = 8;
const byte eepromScreenBrightnessAddress = 10;
const byte eepromContrastAddress = 12;
const byte eepromMatrixBrightnessAddress = 14;
const byte eepromReverseJoystickAddress = 16;
const byte eepromDifficultyAddress = 18;
const byte eepromAppInitSuccessfullyAddress = 20;

// Difficulty
#define EASY 60
#define MEDIUM 61
#define HARD 62
byte difficultyValue;

// Initialization default values in EEPROM for variables
#define OFF 202
#define ON 203
byte appInitSuccessfully;

// Used in readInput() and game handleInput()
const int divideTime = 100; // helps to not use unsigned long for timers and delays>100
unsigned int lastPress = 0;
int debounceDelay = 800;
const int maxThreshold = 650;
const int minThreshold = 350;

void printLine(int line, String text) {
    lcd.setCursor(0, line);
    int spaceRemaining = 16 - text.length();
    for (int i = 0; i < text.length(); i++) {
        lcd.print(text[i]);
    }
    for (int i = 0; i < 16; i++) {
        lcd.print(" ");
    }
}

/* 
 *  EEPROM I/O Functions 
 */
int writeStringToEEPROM(int addrOffset, const String & strToWrite) {
    byte len = strToWrite.length();
    EEPROM.update(addrOffset, len);
    for (int i = 0; i < len; i++) {
        EEPROM.update(addrOffset + 1 + i, strToWrite[i]);
    }
    return 1 + len;
}
int readStringFromEEPROM(int addrOffset, String * strToRead) {
    int newStrLen = EEPROM.read(addrOffset);
    char data[newStrLen + 1];
    for (int i = 0; i < newStrLen; i++) {
        data[i] = EEPROM.read(addrOffset + 1 + i);
    }
    data[newStrLen] = '\0';
    * strToRead = String(data);
    return 1 + newStrLen;
}
void writeIntIntoEEPROM(int address, int number) {
    EEPROM.write(address, number >> 8);
    EEPROM.write(address + 1, number & 0xFF);
}
int readIntFromEEPROM(int address) {
    return (EEPROM.read(address) << 8) + EEPROM.read(address + 1);
}

/*
 * Input function for joystick, button & pause button
 */
int readInput() {
    int output = none;
    // read all joystick values
    int X_Axis = analogRead(joystickPinX); // read the x axis value
    int Y_Axis = analogRead(joystickPinY); // read the y axis value
    int buttonValue = registerReadPin(buttonPin); // read the state of the button
    buttonValue = map(buttonValue, 0, 1, 1, 0); // invert the input from the button to be high when pressed
    if (reverseJoystick) {
        X_Axis = map(X_Axis, 0, 1024, 1024, 0);
    }
    int pauseButtonValue = registerReadPin(pauseButtonPin);
    pauseButtonValue = map(pauseButtonValue, 0, 1, 1, 0); // invert the input from the button to be high when pressed

    if (buttonValue == 1 && (millis()/divideTime - lastPress) > debounceDelay/divideTime) {
        lastPress = millis()/divideTime;
        output = enter;
    } else if (pauseButtonValue == 1 && (millis()/divideTime - lastPress) > debounceDelay/divideTime) {
        lastPress = millis()/divideTime;
        output = pause;
    } else if (X_Axis >= maxThreshold) {
        output = right;
    } else if (X_Axis <= minThreshold) {
        output = left;
    } else if (Y_Axis >= maxThreshold) {
        output = up;
    } else if (Y_Axis <= minThreshold) {
        output = down;
    }
    return output;
}

/*
 * Used to bring EEPROM memory 
 * to default values
 */
void initDefaults() {
    writeIntIntoEEPROM(eepromDifficultyAddress, EASY);
    writeIntIntoEEPROM(eepromMatrixBrightnessAddress, 2);
    writeIntIntoEEPROM(eepromContrastAddress, 100);
    writeIntIntoEEPROM(eepromScreenBrightnessAddress, 950);
    writeIntIntoEEPROM(eepromReverseJoystickAddress, 0);
    writeIntIntoEEPROM(eepromAppInitSuccessfullyAddress, ON);
    writeStringToEEPROM(eepromCurrentUsernameStartOffset, defaultUsername);
}

/*
 * Function for trying to save a score 
 * in top 3 HighScores located in EEPROM
 */
int saveHighScore(const String & playerName, const unsigned int & score) {
    int place = 0;
    int score1, score2, score3;
    String name1, name2, name3;
    int length1, length2, length3, length;
    highScoreRecords = readIntFromEEPROM(eepromHighScoreRecordsAddress);
    if (highScoreRecords > 3 || highScoreRecords < 0) {
        highScoreRecords = 0;
    }

    switch (highScoreRecords) {
    case 0: {
        writeIntIntoEEPROM(eepromHighScoreStartOffset, score);
        writeStringToEEPROM(eepromPlayerNameStartOffset, playerName);
        place = 1;
        String d;
        readStringFromEEPROM(eepromPlayerNameStartOffset, & d);
        break;
    }
    case 1: {
        score1 = readIntFromEEPROM(eepromHighScoreStartOffset);
        if (score > score1) {
            writeIntIntoEEPROM(eepromHighScoreStartOffset, score);
            writeIntIntoEEPROM(eepromHighScoreStartOffset + sizeof(int), score1);
            readStringFromEEPROM(eepromPlayerNameStartOffset, & name1);
            length1 = writeStringToEEPROM(eepromPlayerNameStartOffset, playerName);
            writeStringToEEPROM((eepromPlayerNameStartOffset + length1), name1);
            place = 1;
        } else {
            writeIntIntoEEPROM(eepromHighScoreStartOffset + sizeof(int), score);
            length1 = readStringFromEEPROM(eepromPlayerNameStartOffset, & name1);
            writeStringToEEPROM(eepromPlayerNameStartOffset + length1, playerName);
            place = 2;
        }
        break;
    }
    case 2: {
        score1 = readIntFromEEPROM(eepromHighScoreStartOffset);
        score2 = readIntFromEEPROM(eepromHighScoreStartOffset + sizeof(int));
        if (score > score1) {
            writeIntIntoEEPROM(eepromHighScoreStartOffset, score);
            writeIntIntoEEPROM(eepromHighScoreStartOffset + sizeof(int), score1);
            writeIntIntoEEPROM(eepromHighScoreStartOffset + 2 * sizeof(int), score2);
            length1 = readStringFromEEPROM(eepromPlayerNameStartOffset, & name1);
            readStringFromEEPROM(eepromPlayerNameStartOffset + length1, & name2);
            length = writeStringToEEPROM(eepromPlayerNameStartOffset, playerName);
            writeStringToEEPROM(eepromPlayerNameStartOffset + length, name1);
            writeStringToEEPROM(eepromPlayerNameStartOffset + length + length1, name2);
            place = 1;
        } else if (score > score2) {
            writeIntIntoEEPROM(eepromHighScoreStartOffset + sizeof(int), score);
            writeIntIntoEEPROM(eepromHighScoreStartOffset + 2 * sizeof(int), score2);
            length1 = readStringFromEEPROM(eepromPlayerNameStartOffset, & name1);
            readStringFromEEPROM(eepromPlayerNameStartOffset + length1, & name2);
            length = writeStringToEEPROM(eepromPlayerNameStartOffset + length1, playerName);
            writeStringToEEPROM(eepromPlayerNameStartOffset + length1 + length, name2);
            place = 2;
        } else {
            writeIntIntoEEPROM(eepromHighScoreStartOffset + 2 * sizeof(int), score);
            length1 = readStringFromEEPROM(eepromPlayerNameStartOffset, & name1);
            length2 = readStringFromEEPROM(eepromPlayerNameStartOffset + length1, & name2);
            writeStringToEEPROM(eepromPlayerNameStartOffset + length1 + length2, playerName);
            place = 3;

        }
        break;
    }
    case 3: {
        score1 = readIntFromEEPROM(eepromHighScoreStartOffset);
        score2 = readIntFromEEPROM(eepromHighScoreStartOffset + sizeof(int));
        score3 = readIntFromEEPROM(eepromHighScoreStartOffset + 2 * sizeof(int));
        if (score > score1) {
            writeIntIntoEEPROM(eepromHighScoreStartOffset, score);
            writeIntIntoEEPROM(eepromHighScoreStartOffset + sizeof(int), score1);
            writeIntIntoEEPROM(eepromHighScoreStartOffset + 2 * sizeof(int), score2);
            length1 = readStringFromEEPROM(eepromPlayerNameStartOffset, & name1);
            length2 = readStringFromEEPROM(eepromPlayerNameStartOffset + length1, & name2);
            length = writeStringToEEPROM(eepromPlayerNameStartOffset, playerName);
            writeStringToEEPROM(eepromHighScoreStartOffset + length, name1);
            writeStringToEEPROM(eepromHighScoreStartOffset + length + length1, name2);
            place = 1;
        } else if (score > score2) {
            writeIntIntoEEPROM(eepromHighScoreStartOffset + sizeof(int), score);
            writeIntIntoEEPROM(eepromHighScoreStartOffset + 2 * sizeof(int), score2);
            length1 = readStringFromEEPROM(eepromPlayerNameStartOffset, & name1);
            readStringFromEEPROM(eepromPlayerNameStartOffset + length1, & name2);
            length = writeStringToEEPROM(eepromPlayerNameStartOffset + length1, playerName);
            writeStringToEEPROM(eepromHighScoreStartOffset + length1 + length, name2);
            place = 2;
        } else if (score > score3) {
            writeIntIntoEEPROM(eepromHighScoreStartOffset + 2 * sizeof(int), score);
            length1 = readStringFromEEPROM(eepromPlayerNameStartOffset, & name1);
            length2 = readStringFromEEPROM(eepromPlayerNameStartOffset + length1, & name2);
            writeStringToEEPROM(eepromPlayerNameStartOffset + length1 + length2, playerName);
            place = 3;
        } else {
            // Report if we didn't save
            return -1;
        }
        break;
    }
    }
    // Report that we have saved a new high score
    if (highScoreRecords < 3) {
        highScoreRecords++;
    }
    writeIntIntoEEPROM(eepromHighScoreRecordsAddress, highScoreRecords);
    return place;
}

/* 
 * 8x5-pixels special characters used in LCD display for menu
 */ 
byte upArrowChar[8] = {
    0b00100,
    0b01110,
    0b11111,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b00100
};
byte scrollArrowChar[8] = {
    0b00100,
    0b01110,
    0b11111,
    0b00100,
    0b00100,
    0b11111,
    0b01110,
    0b00100
};
byte downArrowChar[8] = {
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b11111,
    0b01110,
    0b00100
};
byte leftArrowChar[8] = {
    B00000,
    B00100,
    B01000,
    B11111,
    B01000,
    B00100,
    B00000,
    B00000
};
byte rightArrowChar[8] = {
    B00000,
    B00100,
    B00010,
    B11111,
    B00010,
    B00100,
    B00000,
    B00000
};
byte horizontalArrowsChar[8] = {
    B01000,
    B11111,
    B01000,
    B00000,
    B00000,
    B00010,
    B11111,
    B00010
};
byte downSign[8] = {
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B10001,
    B01010,
    B00100
};

#endif