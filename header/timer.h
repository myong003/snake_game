#include <avr/interrupt.h>

volatile unsigned char TimerFlag = 0; //TimerISR() sets this to 1. C programmer should clear to 0.

//Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; //Start count from here, down to 0. default 1ms.
unsigned long _avr_timer_cntcurr = 0; //Current internal count of 1ms ticks.

void TimerOn(){
	//AVR timer/counter controller register TCCR1
	TCCR1B = 0x0B; //bit3 = 0: CTC mode (clear timer on compare)
			//bit2bit1bit0 = 011: pre-scaler /64
			//00001011: 0x0B
			//So, 8MHz clock or 125,000 ticks/s
			//TCNT1 register will count at 125,000 ticks/s
	
	//AVR output compare register OCR1A
	OCR1A = 125; //Timer interrupt will be generated when TCNT1==OCR1A
			// Want a 1ms tick, 0.001s * 125,000 = 125
			// When TCNT1==125, 1ms has passed
	
	//AVR timer interrupt mask register
	TIMSK1 = 0x02; //bit1: OCIE1A -- enables compare match interrupt

	//Initialize avr counter
	TCNT1 = 0;
	
	_avr_timer_cntcurr = _avr_timer_M;
	//TimerISR will be called every _avr_timer_cntcurr ms
	
	//Enable global interrupts
	SREG |= 0x80;
}

void TimerOff(){
	TCCR1B = 0x00;
}

void TimerISR(){
	TimerFlag = 1;
}

void TimerSet(unsigned long M){
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

ISR(TIMER1_COMPA_vect){
	// CPU autmatically calls when TCNT1 == OCR1
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0){
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}


