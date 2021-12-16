#pragma once
#ifndef UTILS_H
#define UTILS_H

// #include <LedControl.h>
#include "myLedControl.h"

#include "registerIO.h"

#include <EEPROM.h>
// #include <LiquidCrystal.h>
#include "myLiquidCrystal.h"

#include "Arduino.h"
#include <avr/io.h>
#include <util/delay.h>



// PORTB // pin 8 -12
// PORTD // pin 0-7
// PORTC // pin A0-A5


// DDRB // pin 8 -12
// DDRD // pin 0-7
// DDRC // pin A0-A5




typedef char int_8;
typedef byte uint_8;



const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define BUZZER 13

#define JOY_X A1
#define JOY_Y A0
#define JOY_SW 9

#define UNUSED_ANALOG 0 // for random seed

#define BLOCK_COUNT (sizeof(blocks) / sizeof(blocks[0]))

const int dinPin = 12;
const int clockPin = 11;
const int loadPin = 10;

LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);


const int eepromHighScoreStartOffset = 0;

//Arduino pins attached to joystick
const int joystickButtonPin = 8;
const int joystickPinX = A1;
const int joystickPinY = A0;


//Joystick values
#define up 0
#define right 1
#define down 2
#define left 3
#define enter 4
#define none 5


unsigned long lastPress = 0;
int debounceDelay = 250;

const int maxThreshold = 650;
const int minThreshold = 350;

int contrastPin = 9;
unsigned int currentContrast = 100;




void printLine(int line, String text)
{
  lcd.setCursor(0, line);

  int spaceRemaining = 16 - text.length();

  for(int i=0; i<text.length();i++) {
    lcd.print(text[i]);
  }
  // lcd.print(text);
  for(int i=0;i<16;i++) {
    lcd.print(" ");
  }
}

int printInLine(int line, String text, int startingPos) {
  lcd.setCursor(0,line);
  for(int i=startingPos;i<startingPos+text.length();i++)
    lcd.write(text[i]);

  return startingPos+text.length();
}



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



int readJoystick()
{
  int output = none;
  // read all joystick values
  int X_Axis = analogRead(joystickPinX); // read the x axis value

  int Y_Axis = analogRead(joystickPinY); // read the y axis value


  // Y_Axis = map(Y_Axis, 0, 1023, 1023, 0);      // invert the input from the y axis so that pressing the stick forward gives larger values
  int SwitchValue = readPin(joystickButtonPin); // digitalRead(joystickButtonPin); // read the state of the switch
  

  SwitchValue = map(SwitchValue, 0, 1, 1, 0);       // invert the input from the switch to be high when pressed


  
  if (SwitchValue == 1 && millis() - lastPress > debounceDelay)
  {
    lastPress = millis();
    output = enter;
    
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



#endif

//9358 bytes (29%)
//1225 bytes (59%)