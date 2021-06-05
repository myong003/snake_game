/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <stdlib.h>
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

unsigned char numRows = 5;
unsigned char numCols = 8;
int snakePos[40][2];
unsigned char snakeLength;
unsigned char direction = 1; //0 = up, 1 = down, 2 = right, 3 = left;
unsigned char gameOver = 0;
enum snakeStates{Snake_start, Snake_move, Snake_eat, Snake_end}state;
int TickSnake(int state){
	switch(state){
		case Snake_start:
			state = Snake_move;
			break;
		case Snake_move:
			for (unsigned char i=snakeLength-1; i > 0; i--){ //rightshift array
				snakePos[i][0] = snakePos[i-1][0];
				snakePos[i][1] = snakePos[i-1][1];
			}
			if (direction == 0){
				if (snakePos[0][0] == 0){
					state = Snake_end;
				}
				else{
					snakePos[0][0]--; //move up
				}
			}
			else if (direction == 1){
				if (snakePos[0][0] == numRows - 1){
					state = Snake_end;
				}
				else{
					snakePos[0][0]++;; //move down
				}
			}
			else if(direction == 2){
				if (snakePos[0][1] == numCols){
					state = Snake_end;
				}
				else{
					snakePos[0][1]++; //move right
				}
			}
			else if(direction == 3){
				if (snakePos[0][1] == 0){
					state = Snake_end;
				}
				else{
					snakePos[0][1]--; //move left
				}
			}
			break;
		case Snake_eat:
			break;
		case Snake_end:
			break;
		default:
			state = Snake_start;
			break;
	}
	switch(state){
		case Snake_start:
			snakeLength = 1;
			snakePos[0][0] = 0;
			snakePos[0][1] = 0;
			direction = 1;
			gameOver = 0;
			break;
		case Snake_move:
			break;
		case Snake_eat:
			break;
		case Snake_end:
			gameOver = 1;
			break;
		default:
			snakeLength = 1;
			snakePos[0][0] = 0;
			snakePos[0][1] = 0;
			direction = 1;
			break;
	}
	return state;
}

unsigned char moveUp;
unsigned char moveDown;
unsigned char moveLeft;
unsigned char moveRight;
enum moveStates{Move_wait, Move_press};
int TickMove(int state){
	moveUp = ~PINA & 0x01;
	moveDown = ~PINA & 0x02;
	moveRight = ~PINA & 0x04;
	moveLeft = ~PINA & 0x08;
	switch(state){
		case Move_wait:
			if (moveUp){
				direction = 0;
				state = Move_press;
			}
			else if (moveDown){
				direction = 1;
				state = Move_press;
			}
			else if (moveRight){
				direction = 2;
				state = Move_press;
			}
			else if (moveLeft){
				direction = 3;
				state = Move_press;
			}
			break;
		case Move_press:
			if (!moveUp && !moveDown && !moveLeft && !moveRight){
				state = Move_wait;
			}
			break;
		default:
			state = Move_wait;
			break;
	}
	return state;
}

unsigned char pattern = 0x00;
unsigned char row = 0xFF;
unsigned char one = 0x01;
unsigned char zero = 0xFE;
unsigned char countPos = 0;
enum LightStates{Light_on, Light_end};
int TickLights(int state){
	switch (state){
		case Light_on:
			if (gameOver){
				state = Light_end;
			}
			pattern = 0x00;
			row = 0xFF;
			for (unsigned char i=0; i < snakeLength; i++){
				one = 0x80;
				zero = 0xFE;
				countPos = snakePos[i][0];
				while (countPos > 0){
					zero = zero << 1;
					zero += 1;
					countPos--;
				}
				row &= zero;
				pattern |= one >> snakePos[i][1];
			}
			break;
		case Light_end:
			pattern = 0xFF;
			row = 0xEE;
			break;
		default:
			state = Light_on;
	}
	PORTC = pattern;
	PORTD = row;
	return state;
}

enum GameStates{Game_start, Game_play, Game_end};
int TickGame(int state){
	switch (state){
		case Game_start:
			state = Game_play;
			break;
		case Game_play:
			break;
		case Game_end:
			break;
		default:
			state = Game_start;
			break;
	}
	switch (state){
		case Game_start:
			break;
		case Game_play:
			break;
		case Game_end:
			break;
	}
	return state;
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
    /* Insert your solution below */
	unsigned int GCD = 100;
	TimerSet(GCD);
	TimerOn();
	static task task1, task2, task3;
	task *tasks[] = {&task1, &task2, &task3};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	const char start = -1;

	task1.state = start;
	task1.period = 700;
	task1.elapsedTime = task1.period;
	task1.TickFct = &TickSnake;

	task2.state = start;
	task2.period = 100;
	task2.elapsedTime = task2.period;
	task2.TickFct = &TickMove;

	task3.state = start;
	task3.period = 700;
	task3.elapsedTime = task3.period;
	task3.TickFct = &TickLights;

    while (1) {
	for (unsigned char i=0; i < numTasks; i++){
		if (tasks[i]->elapsedTime == tasks[i]->period){
			tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
			tasks[i]->elapsedTime = 0;
		}
		tasks[i]->elapsedTime += GCD;
	}
	while (!TimerFlag);
	TimerFlag = 0;
    }
    return 1;
}
