#pragma once
#ifndef UTILS_H
#define UTILS_H


#include <LedControl.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>


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


#endif
