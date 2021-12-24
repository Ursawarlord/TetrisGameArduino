/*
 * song.h includes a fragment from Tetris theme 
 * (Korobeiniki) Robson Couto, 2019
 */

#include "utils.h"

#define EQUALIZE_DOWN 4
#define NOTE_F4 349 / EQUALIZE_DOWN
#define NOTE_A4 440 / EQUALIZE_DOWN
#define NOTE_B4 494 / EQUALIZE_DOWN
#define NOTE_C5 523 / EQUALIZE_DOWN
#define NOTE_D5 587 / EQUALIZE_DOWN
#define NOTE_E5 659 / EQUALIZE_DOWN

// change this to make the song slower or faster
static int tempo = 144;

// notes of the moledy followed by the duration.
// a 4 means a quarter note, 8 an eighteenth , 16 sixteenth, so on
// !!negative numbers are used to represent dotted notes,
// so -4 means a dotted quarter note, that is, a quarter plus an eighteenth!!
static byte melody[] = {
    //Based on the arrangement at https://www.flutetunes.com/tunes.php?id=192
    NOTE_E5,
    4,
    NOTE_B4,
    8,
    NOTE_C5,
    8,
    NOTE_D5,
    4,
    NOTE_C5,
    8,
    NOTE_B4,
    8,
    NOTE_A4,
    4,
    NOTE_A4,
    8,
    NOTE_C5,
    8,
    NOTE_E5,
    4,
    NOTE_D5,
    8,
    NOTE_C5,
    8,
    NOTE_B4,
    -4,
    NOTE_C5,
    8,
    NOTE_D5,
    4,
    NOTE_E5,
    4,
    NOTE_C5,
    4,
    NOTE_A4,
    4,
    NOTE_A4,
    8,
    NOTE_A4,
    4,
    NOTE_B4,
    8,
    NOTE_C5,
    8,
};

// sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
// there are two values per note (pitch and duration), so for each note there are four bytes
int notes = sizeof(melody) / sizeof(melody[0]) / 2;

// this calculates the duration of a whole note in ms (60s/tempo)*4 beats
int wholenote = (60000 * 4) / tempo;

int divider = 0, noteDuration = 0;
unsigned long delayTimer = 0;
int thisNote = 0;
bool newTone = false;
bool songIsPlaying = false;

void playSong() {
    if (!songIsPlaying) {
        return;
    }

    // loop the melody
    if (thisNote > notes * 2 - 2) {
        thisNote = 0;
    }

    if (thisNote < notes * 2 && !newTone) {
        divider = melody[thisNote + 1]; // calculates the duration of each note
        if (divider > 0) {
            noteDuration = (wholenote) / divider; // regular note, just proceed
        } else if (divider < 0) {
            noteDuration = (wholenote) / abs(divider); // dotted notes are represented with negative durations!!
            noteDuration *= 1.5; // increases the duration in half for dotted notes
        }
        tone(BUZZER, melody[thisNote], noteDuration * 0.8); // 0.8 because 0.2 is silence in-between tones
        newTone = true;
    }
    if (millis() - delayTimer > noteDuration) {
        delayTimer = millis();
        // stop the waveform generation before the next note.
        noTone(BUZZER);
        newTone = false;
        thisNote = thisNote + 2;
    }
}

void pauseSong() {
    songIsPlaying = false;
}
void resumeSong() {
    songIsPlaying = true;
    playSong();
}