/*	Author: terry
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

// Demo: https://drive.google.com/open?id=1IJ2hKEaueR3U9L88YIWI7l9Y14Oi809A
#include <avr/io.h>
#include "io.h"
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn(){
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;

	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff(){
	TCCR1B = 0x00;
}

void TimerISR(){
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect){
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0){
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M){
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

unsigned int tmpB = 0;
unsigned char tmpA = 0x00;
unsigned char one = 0x01;
unsigned int i = 0;
unsigned int j = 0;
enum states{start, r, a0p, a1p, doublep}state;
void tick(){
	switch(state){
		case start:
			state = r;
			tmpB = 0;
			break;
		case r:
			if(tmpA == 0x01){
				state = a0p;
				if(tmpB < 9){
					tmpB+=1;
				}
			}
			else if(tmpA == 0x02){
				state = a1p;
				if(tmpB > 0){
					tmpB-=1;
				}
			} 
			else if(tmpA == 0x03){
				state = doublep;
			}	
			else{
				state = r;
			}
			break;
		case a0p:
			i++;
			if(i == 10){
				if(tmpB < 9){
					tmpB+=1;
				}
				i = 0;
			}
			if(tmpA == 0x00){
				state = r;
			}
			else if(tmpA == 0x03){
				state = doublep;
			}
			else{
				state = a0p;
			}
			break;
		case a1p:
			j++;
			if(j == 10){
				if(tmpB > 0){
					tmpB-=1;
				}
				j = 0;
			}
			if(tmpA == 0x03){
				state = doublep;
			}
			else if(tmpA == 0x00){
				state = r;
			}
			else{
				state = a1p;
			}
			break;
		case doublep:
			if(tmpA == 0x00){
				state = r;
				tmpB = 0x00;
			}
			else{
				state = doublep;
			}
			break;
		default:
			break;
	}
	LCD_Cursor(1);
	LCD_WriteData(tmpB + '0');
}
int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;
    LCD_init();
    
    TimerSet(100);
    TimerOn();
    state = start;
    /* Insert your solution below */
    while (1) {
	tmpA = ~PINA & 0x03;
	tick();
	while(!TimerFlag){}
	TimerFlag = 0;
    }
    return 1;
}

