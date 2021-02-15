 /* Author: Hulbert Zeng
 * Partner(s) Name (if applicable):  
 * Lab Section: 021
 * Assignment: Lab #8  Exercise #1
 * Exercise Description: [optional - include for your own benefit]
 *
 * I acknowledge all content contained herein, excluding template or example
 * code, is my own original work.
 *
 *  Demo Link: https://youtu.be/KSedcQVFoTA
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

enum PWM_States { PWM_SMStart, PWM_Silent, PWM_Tone1, PWM_Tone2, PWM_Tone3 } PWM_State;
unsigned char old = 0x00;
unsigned char button;

void TickFct() {
    button = (~PINA) & 0x07;
    switch(PWM_State) {
        case PWM_SMStart:
            PWM_State = PWM_Silent;
            break;
        case PWM_Silent:
            if (old != button) {
                old = button;
                if (button == 0x01){
                    PWM_State = PWM_Tone1;
                } else if (button == 0x02){
                    PWM_State = PWM_Tone2;
                } else if (button == 0x04){
                    PWM_State = PWM_Tone3;
                } else {
                    PWM_off();
                    PWM_State = PWM_Silent;
                }
            } else {
                PWM_State = PWM_Silent;
            }
            break;
        case PWM_Tone1:
            PWM_on();
            set_PWM(261.63);
            PWM_State = PWM_Silent;
            break;
        case PWM_Tone2:
            PWM_on();
            set_PWM(293.66);
            PWM_State = PWM_Silent;
            break;
        case PWM_Tone3:
            PWM_on();
            set_PWM(329.63);
            PWM_State = PWM_Silent;
            break;
        default:
            PWM_State = PWM_SMStart;
            break;
    }
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF;
    TimerSet(100);
    TimerOn();
    PWM_State = PWM_SMStart;
    /* Insert your solution below */
    while (1) {
        TickFct();
    }
    return 1;
}
