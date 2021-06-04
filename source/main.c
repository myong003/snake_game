/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include "timer.h"
#include "PWM.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void A2D_init(){
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
}

typedef struct _task{
	signed char state;
	unsigned long int period;
	unsigned long int elapsedTime;
	int (*TickFct)(int);
}task;

unsigned char button1;
unsigned char button2;
enum GameStates{Game_start, Game_play, Game_end};
int TickGame(int state){
	switch (state){
		case Game_start:
			break;
		case Game_play:
			break;
		case Game_end:
			break;
		default:
			state = Game_start;
			break;
	}
	return state;
}

unsigned short input;
enum Demo_States{shift};
int Demo_Tick(int state){
	static unsigned char pattern = 0x80;
	static unsigned char row = 0xFE;

	switch(state){
		case shift:
			break;
		default:
			state = shift;
	}
	switch(state){
		case shift:
			input = ADC;
			if (input != 0){
				PORTB = 0x01;
			}
			else{
				PORTB = 0x00;
			}

			if (row == 0xEF && pattern == 0x01){
				pattern = 0x80;
				row = 0xFE;
			}
			else if(pattern == 0x01){
				pattern = 0x80;
				row = (row << 1) | 0x01;
			}
			else{
				pattern >>= 1;
			}
			break;
		default:
			break;
	}
	PORTC = pattern;
	PORTD = row;
	return state;
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
    /* Insert your solution below */
	A2D_init();
	TimerSet(300);
	TimerOn();
	int state = -1;
    while (1) {
	state = Demo_Tick(state);
	while (!TimerFlag);
	TimerFlag = 0;
    }
    return 1;
}
