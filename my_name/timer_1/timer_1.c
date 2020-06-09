
#include <avr/interrupt.h>
#include "timer_1.h"

// этот таймер нужен для числа TCNT1, котрое считается с момента подачи питания и несовпадает с чмслом другими устройствами

void Timer_1_ini(void)
{
	TCCR1A =	(0<<COM1A1)|	//	7 bit: Compare Output Mode
				(0<<COM1A0)|	//	6 bit: Compare Output Mode
				(0<<COM1B1)|	//	5 bit: Compare Output Mode
				(0<<COM1B0)|	//	4 bit: Compare Output Mode
				(0<<WGM11)|		//	1 bit: Waveform Generation Mode
				(0<<WGM10);		//	0 bit: Waveform Generation Mode
		
	TCCR1B =	(0<<ICNC1)|		//	7 bit: Input Capture Noise Canceler
				(0<<ICES1)|		//	6 bit: Input Capture Edge Select
				(0<<WGM13)|		//	4 bit: Waveform Generation Mode
				(1<<WGM12)|		//	3 bit: Waveform Generation Mode
				(0<<CS12)|		//	2 bit: Clock Select
				(0<<CS11)|		//	1 bit: Clock Select
				(1<<CS10);		//	0 bit: Clock Select
				
	TCCR1C =	(0<<FOC1A)|		//	7 bit: Force Output Compare A
				(0<<FOC1B);		//	6 bit: Force Output Compare B
		
	TCNT1H =	0x00;			//	Текущее значение счетчика (старший разряд 15:8)
	TCNT1L =	0x00;			//	Текущее значение счетчика (младший разряд 7:0)
	
	OCR1AH =	0x00;			//	Output Compare Register A	(старший разряд 15:8)	//	значение до которого считает таймер
	OCR1AL =	0xFF;			//	Output Compare Register A	(младший разряд 7:0)	//	значение до которого считает таймер
	
	OCR1BH =	0x00;			//	Output Compare Register B	(старший разряд 15:8)	//	значение до которого считает таймер
	OCR1BL = 	0x00;			//	Output Compare Register B	(младший разряд 7:0)	//	значение до которого считает таймер
		
	TIMSK1 =	(0<<ICIE1)|		//	5 bit: Timer/Counter1, Input Capture Interrupt Enable
				(0<<OCIE1B)|	//	2 bit: Output Compare Match B Interrupt Enable						//	прерывание при сравнении с B
				(0<<OCIE1A)|	//	1 bit: Output Compare Match A Interrupt Enable						//	прерывание при сравнении с A
				(0<<TOIE1);		//	0 bit: Overflow Interrupt Enable									//	прерывание по переполнению
		
	TIFR1 =		(0<<ICF1)|		//	5 bit: Timer/Counter1, Input Capture Flag
				(0<<OCF1B)|		//	2 bit: Output Compare Match Flag B
				(0<<OCF1A)|		//	1 bit: Output Compare Match Flag A
				(0<<TOV1);		//	0 bit: Overflow Flag
}