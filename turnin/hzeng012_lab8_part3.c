 /* Author: Hulbert Zeng
 * Partner(s) Name (if applicable):  
 * Lab Section: 021
 * Assignment: Lab #8  Exercise #3
 * Exercise Description: [optional - include for your own benefit]
 *
 * I acknowledge all content contained herein, excluding template or example
 * code, is my own original work.
 *
 *  Demo Link: https://youtu.be/-AQPMk-bWXc
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

enum MU_States { MU_SMStart, MU_Off, MU_Play, MU_Buffer } MU_State;

// Star Wars theme song melody
double melody[16] = { 261.63, 392.00, 349.23, 329.63, 293.66, 523.25, 392.00, 349.23, 329.63, 293.66, 523.25, 392.00, 349.23, 329.63, 349.23, 293.66 };
unsigned char i = 0;

void music() {
    unsigned char on = (~PINA) & 0x01;
    switch(MU_State) {
        case MU_SMStart:
            MU_State = MU_Off;
            break;
        case MU_Off:
            if(on) { MU_State = MU_Play; }
            break;
        case MU_Play:
            if(i < 16) {
                PWM_on();
                set_PWM(melody[i]);
                ++i;
            } else {
                i = 0;
                PWM_off();
                MU_State = MU_Buffer;
            }
            break;
        case MU_Buffer:
            if(!on) { MU_State = MU_Off; }
            break;
        default:
            MU_State = MU_SMStart;
            break;
    }
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    TimerSet(312.5);
    TimerOn();
    MU_State = MU_SMStart;
    /* Insert your solution below */
    while (1) {
        while(!TimerFlag);
        music();
        TimerFlag = 0;
    }
    return 1;
}
