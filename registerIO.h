#ifndef REGISTERIO_H
#define REGISTERIO_H

void setInputPullupPin(int pin) {
  if(0 <= pin && pin <= 7) {
    DDRD = ~(1 << pin) & PORTD;
    PORTD = (1 << pin ) | PORTD;
  }
  else if(8 <= pin && pin <= 13) {
    DDRB = ~(1 << (pin-8) ) & PORTB;
    PORTB = (1 << (pin-8) ) | PORTB;
  }
  else if(A0 <= pin && pin <= A5) {
    DDRC = ~(1 << (pin-A0) ) & PORTC;
    PORTC = (1 << (pin-A0) ) | PORTC;
  }
}

void setOutputPin(int pin) {
  if(0 <= pin && pin <= 7) {
    DDRD = (1 << pin ) | DDRD;
  }
  else if(8 <= pin && pin <= 13) {
    DDRB = (1 << (pin-8) ) | DDRB;
  }
  else if(A0 <= pin && pin <= A5) {
    DDRC = (1 << (pin-A0) ) | DDRC;
  }
}

int readPin(int pin) {
  if(0 <= pin && pin <= 7) {
    return (PIND & (1 << pin)) >> pin;
  }
  else if(8 <= pin && pin <= 13) {
     return (PINB & (1 << (pin-8) )) >> (pin-8);
  }
  else if(A0 <= pin && pin <= A5) {
     return (PINC & (1 << (pin-A0) )) >> (pin-A0);
  }
}

void writeHighToDigitalPin(int pin) {
   if(0 <= pin && pin <= 7) {
    PORTD = (1 << pin ) | PORTD;
  }
  else if(8 <= pin && pin <= 13) {
    PORTB = (1 << (pin-8) ) | PORTB;
  }
//  else if(A0 <= pin && pin <= A5) {
//    DDRC = (1 << (pin-A0) ) | DDRC;
//  }
}

void writeLowToDigitalPin(int pin) {
  if(0 <= pin && pin <= 7)
  {
    PORTD = ~(1 << pin) & PORTD;
  }
  else if(8 <= pin && pin <= 13) {
    PORTB = ~(1 << (pin-8) ) & PORTB;
  }
//  else if(A0 <= pin && pin <= A5) {
//    DDRC = (1 << (pin-A0) ) | DDRC;
//  }
}

#endif