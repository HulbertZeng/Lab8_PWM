 /* Author: Hulbert Zeng
 * Partner(s) Name (if applicable):  
 * Lab Section: 021
 * Assignment: Lab #8  Exercise #2
 * Exercise Description: [optional - include for your own benefit]
 *
 * I acknowledge all content contained herein, excluding template or example
 * code, is my own original work.
 *
 *  Demo Link: https://youtu.be/JgYul_pvD4A
 */ 
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
#include "timer.h"

void set_PWM(double frequency) {
    static double current_frequency;
    if(frequency != current_frequency) {
        if(!frequency) { TCCR3B &= 0x08; }
        else {TCCR3B |= 0x03; }

        if(frequency < 0.954) { OCR3A = 0xFFFF; }
        else if(frequency > 31250) { OCR3A = 0x0000; }
        else { OCR3A = (short)(8000000/ (128*frequency)) -1; }

        TCNT3 = 0;
        current_frequency = frequency;
    }
}

void PWM_on() {
    TCCR3A = (1<< COM3A0);
    TCCR3B = (1<< WGM32) | (1<<CS31) | (1<<CS30);
    set_PWM(0);
}

void PWM_off() {
    TCCR3A = 0x00;
    TCCR3B = 0x00;
}

enum OO_States { OO_SMStart, OO_Off, OO_On } OO_State;
enum US_States { US_SMStart, US_Wait, US_Up, US_Down } US_State;

double notes[8] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};
unsigned char prev = 1;
unsigned char i = 0;
unsigned char on = 0x00;

void OnOff() {
     unsigned char power = (~PINA) & 0x04;
     switch(OO_State) {
          case OO_SMStart:
              OO_State = OO_Off;
              break;
          case OO_Off:
              PWM_off();
              if(power) {
                  PWM_on();
                  set_PWM(notes[i]);
                  OO_State = OO_On;
              }
              break;
          case OO_On:
              if(power) {
                  PWM_off();
                  OO_State = OO_Off;
              } else if(i != prev) {
                  PWM_on();
                  set_PWM(notes[i]);
                  prev = i;
              }
              break;
          default:
              OO_State = OO_SMStart;
              break;
     }
}

void UpDown() {
     unsigned char up = (~PINA) & 0x01;
     unsigned char down = (~PINA) & 0x02;
     switch(US_State) {
          case US_SMStart:
              US_State = US_Wait;
              break;
          case US_Wait:
              if(down) {
                  if(i > 0) {
                      --i;
                  }
                  US_State = US_Down;
              } else if(up) {
                  if(i < 7) {
                      ++i;
                  }
                  US_State = US_Up;
              } else {
                  US_State = US_Wait;
              }
              break;
          case US_Up:
              if(!up) {
                  US_State = US_Wait;
              }
              break;
          case US_Down:
              if(!down) {
                  US_State = US_Wait;
              }
              break;
          default:
              US_State = US_SMStart;
              break;
     }
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    //TimerSet(100);
    //TimerOn();
    OO_State = OO_SMStart;
    US_State = US_SMStart;
    /* Insert your solution below */
    while (1) {
        OnOff();
        UpDown();
    }
    return 1;
}
