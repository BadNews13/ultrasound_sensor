
#include "../../defines/defines_global.h"
#include <avr/interrupt.h>
#include "timer_2.h"
#include "../RTOS/rtos.h"

void Timer_2_ini(void)
{
	TCCR2A =	(0<<COM2A1)|	//	7 bit: Compare Output Mode
				(1<<COM2A0)|	//	6 bit: Compare Output Mode		/1 toggle в PWM
				(0<<COM2B1)|	//	5 bit: Compare Output Mode		
				(0<<COM2B0)|	//	4 bit: Compare Output Mode		/1 clear/set
				(0<<WGM21)|		//	1 bit: Waveform Generation Mode
				(1<<WGM20);		//	0 bit: Waveform Generation Mode
	
	TCCR2B =	(0<<FOC2A)|		//	7 bit: Force Output Compare A
				(0<<FOC2B)|		//	6 bit: Force Output Compare B
				(1<<WGM22)|		//	3 bit: Waveform Generation Mode
				(0<<CS22)|		//	2 bit: Clock Select
				(0<<CS21)|		//	1 bit: Clock Select
				(0<<CS20);		//	0 bit: Clock Select
	
	TCNT2 =		0x00;			//	“екущее значение счетчика
	
	OCR2A =		0xFF;			//	Output Compare Register A				//	значение до которого считает таймер (12) 100
	OCR2B = 	0b00000000;		//	Output Compare Register B				//	значение до которого считает таймер
	
	TIMSK2 =	(0<<OCIE2B)|	//	2 bit: Output Compare Match B Interrupt Enable						//	прерывание при сравнении с B
				(1<<OCIE2A)|	//	1 bit: Output Compare Match A Interrupt Enable						//	прерывание при сравнении с A
				(0<<TOIE2);		//	0 bit: Overflow Interrupt Enable									//	прерывание по переполнению
	
	TIFR2 =		(0<<OCF2B)|		//	2 bit: Output Compare Match Flag B
				(0<<OCF2A)|		//	1 bit: Output Compare Match Flag A
				(0<<TOV2);		//	0 bit: Overflow Flag
}


ISR(TIMER2_COMPA_vect)
{
	RTOS_ISR_INT();
}