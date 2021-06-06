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
unsigned char i;
int snakePos[40][2];
unsigned char snakeLength;
unsigned char gameOver = 1;

unsigned char fruitEaten = 0;
unsigned char fruitCol = 0;
unsigned char fruitRow = 0;
void generateFruit(){
	fruitRow = rand() % numRows;
	fruitCol = rand() % numCols;
	i=0;
	while (i < snakeLength){
		if (fruitRow == snakePos[i][0] && fruitCol == snakePos[i][1]){
			i=0;
			fruitRow = rand() % numRows;
			fruitCol = rand() % numCols;
		}
		else{
			i++;
		}
	}
}
enum fruitStates{Fruit_start, Fruit_wait};
int TickFruit(int state){
	switch(state){
		case Fruit_start:
			if (!gameOver){
				state = Fruit_wait;
				fruitEaten = 0;
			}
			break;
		case Fruit_wait:
			if (fruitEaten){
				generateFruit();
				fruitEaten = 0;
			}
			break;
		default:
			state = Fruit_start;
			break;
	}
	return state;
}

unsigned char checkCollision(int nextRow, int nextCol){
	if (nextRow == -1 || nextRow == numRows){
		return 1;
	}
	if (nextCol == -1 || nextCol == numCols){
		return 1;
	}
	for (i=0; i < snakeLength; i++){
		if (nextRow == snakePos[i][0] && nextCol == snakePos[i][1]){
			return 1;
		}
	}
	return 0;
}

unsigned char direction = 4; //0 = up, 1 = down, 2 = right, 3 = left, 4 = not started;
unsigned char currRow;
unsigned char currCol;
unsigned char lastRow;
unsigned char lastCol;
enum snakeStates{Snake_start, Snake_move, Snake_end}state;
int TickSnake(int state){
	switch(state){
		case Snake_start:
			if (!gameOver){
				state = Snake_move;
			}
			break;
		case Snake_move:
			lastRow = snakePos[snakeLength-1][0];
			lastCol = snakePos[snakeLength-1][1];
			for (i=snakeLength-1; i > 0; i--){ //rightshift array
				snakePos[i][0] = snakePos[i-1][0];
				snakePos[i][1] = snakePos[i-1][1];
			}
			currRow = snakePos[0][0];
			currCol = snakePos[0][1];
			if (direction == 0){
				if(checkCollision(currRow-1, currCol)){
					state = Snake_end;
				}
				else{
					if (currRow == fruitRow+1 && currCol == fruitCol){
						fruitEaten = 1;
					}
					snakePos[0][0]--; //move up
				}
			}
			else if (direction == 1){
				if (checkCollision(currRow+1, currCol)){
					state = Snake_end;
				}
				else{
					if (currRow == fruitRow-1 && currCol == fruitCol){
						fruitEaten = 1;
					}
					snakePos[0][0]++; //move down
				}
			}
			else if(direction == 2){
				if (checkCollision(currRow, currCol+1)){
					state = Snake_end;
				}
				else{
					if (currRow == fruitRow && currCol == fruitCol-1){
						fruitEaten = 1;
					}
					snakePos[0][1]++; //move right
				}
			}
			else if(direction == 3){
				if (checkCollision(currRow, currCol-1)){
					state = Snake_end;
				}
				else{
					if (currRow == fruitRow && currCol == fruitCol+1){
						fruitEaten = 1;
					}
					snakePos[0][1]--; //move left
				}
			}
			if (fruitEaten){
				snakePos[snakeLength][0] = lastRow;
				snakePos[snakeLength][1] = lastCol;
				snakeLength++;
			}
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
			snakePos[0][0] = 2;
			snakePos[0][1] = 3;
			break;
		case Snake_move:
			break;
		case Snake_end:
			gameOver = 1;
			break;
	}
	return state;
}

unsigned char moveUp;
unsigned char moveDown;
unsigned char moveLeft;
unsigned char moveRight;
unsigned char buttonPressed;
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
	switch (state){
		case Move_wait:
			buttonPressed = 0;
			break;
		case Move_press:
			buttonPressed = 1;
			break;
	}
	return state;
}

unsigned char pattern = 0x00;
unsigned char row = 0xFF;
unsigned char one = 0x01;
unsigned char zero = 0xFE;
unsigned char j;
unsigned int counter;
enum LightStates{Light_wait, Light_waitRelease, Light_row0, Light_row1, Light_row2, Light_row3, Light_row4, Light_end};
int TickLights(int state){	
	switch (state){
		case Light_wait:
			if (buttonPressed){
				state = Light_waitRelease;
			}
			break;
		case Light_waitRelease:
			if (!buttonPressed){
				state = Light_row0;
				srand(counter);
				gameOver = 0;
				generateFruit();
			}
			break;
		case Light_row0:
			if (gameOver){
				state = Light_end;
			}
			else{
				state = Light_row1;
			}
			break;
		case Light_row1:
			if (gameOver){
				state = Light_end;
			}
			else{
				state = Light_row2;
			}
			break;
		case Light_row2:
			if (gameOver){
				state = Light_end;
			}
			else{
				state = Light_row3;
			}
			break;
		case Light_row3:
			if (gameOver){
				state = Light_end;
			}
			else{
				state = Light_row4;
			}
			break;
		case Light_row4:
			if (gameOver){
				state = Light_end;
			}
			else{
				state = Light_row0;
			}
			break;
		case Light_end:
			break;
		default:
			state = Light_wait;
			break;
	}
	switch(state){
		case Light_wait:
			one = 0x80;
			zero = 0xFE;
			pattern = 0x00;
			row = 0xFF;
			pattern |= one >> snakePos[0][1];
			for (j=0; j < snakePos[0][0]; j++){
				zero = zero << 1;
				zero++;
			}
			row &= zero;
			counter = 0;
			break;
		case Light_waitRelease:
			counter++;
			break;
		case Light_row0:
			pattern = 0x00;
			row = 0xFE;
			one = 0x80;
			if (fruitRow == 0){
				pattern |= one >> fruitCol;
			}
			for (j=0; j < snakeLength; j++){
				if (snakePos[j][0] == 0){
					one = 0x80;
					pattern |= one >> snakePos[j][1];
				}
			}
			break;
		case Light_row1:
			pattern = 0x00;
			row = 0xFD;
			one = 0x80;
			if (fruitRow == 1){
				pattern |= one >> fruitCol;
			}
			for (j=0; j < snakeLength; j++){
				if (snakePos[j][0] == 1){
					one = 0x80;
					pattern |= one >> snakePos[j][1];
				}
			}
			break;
		case Light_row2:
			pattern = 0x00;
			row = 0xFB;
			one = 0x80;
			if (fruitRow == 2){
				pattern |= one >> fruitCol;
			}
			for (j=0; j < snakeLength; j++){
				if (snakePos[j][0] == 2){
					one = 0x80;
					pattern |= one >> snakePos[j][1];
				}
			}
			break;
		case Light_row3:
			pattern = 0x00;
			row = 0xF7;
			one = 0x80;
			if (fruitRow == 3){
				pattern |= one >> fruitCol;
			}
			for (j=0; j < snakeLength; j++){
				if (snakePos[j][0] == 3){
					one = 0x80;
					pattern |= one >> snakePos[j][1];					}
			}
			break;
		case Light_row4:
			pattern = 0x00;
			row = 0xEF;
			one = 0x80;
			if (fruitRow == 4){
				pattern |= one >> fruitCol;
			}
			for (j=0; j < snakeLength; j++){
				if (snakePos[j][0] == 4){
					one = 0x80;
					pattern |= one >> snakePos[j][1];
				}
			}
			break;
		case Light_end:
			pattern = 0xFF;
			row = 0xEE;
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
	unsigned int GCD = 1;
	TimerSet(GCD);
	TimerOn();
	static task task1, task2, task3, task4;
	task *tasks[] = {&task1, &task2, &task3, &task4};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	const char start = -1;

	task1.state = start;
	task1.period = 500;
	task1.elapsedTime = task1.period;
	task1.TickFct = &TickSnake;

	task2.state = start;
	task2.period = 1;
	task2.elapsedTime = task2.period;
	task2.TickFct = &TickMove;

	task3.state = start;
	task3.period = 500;
	task3.elapsedTime = task3.period;
	task3.TickFct = &TickFruit;

	task4.state = start;
	task4.period = 1;
	task4.elapsedTime = task4.period;
	task4.TickFct = &TickLights;

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
