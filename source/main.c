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
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void A2D_init(){
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
}


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
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
    /* Insert your solution below */
	A2D_init();
    while (1) {
	
    }
    return 1;
}
