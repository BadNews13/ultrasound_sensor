
#include <avr/interrupt.h>
#include "timer0.h"
//#include "interrupt0/interrupt0.h"
	
void Timer0_ini_PWM(void)
{
/*Таймер для генерации импульсов на PWM ноге																*
PWM PhaseCorrect mode																						*
prescaler 1/8																								*
верх таймера 0x0C																							*
При 16Mhz - инверсия OC0A будет происходить с частотой ~82kHz (следовательно импульсы - ~41kHz)				*
*/

//Переделал в CTC-mode т.к. для OC0B отсутствует режим работы toggle в PWM-mode. если делать под один датчик, то можно вернуть PWM и использовать OC0A с последующей перенастройкой ноги для компаратора

	cli();
	TCCR0A =	(0<<COM0A1)|	//	7 bit: Compare Output Mode
				(1<<COM0A0)|	//	6 bit: Compare Output Mode		/1 toggle в PWM
				(0<<COM0B1)|	//	5 bit: Compare Output Mode		
				(0<<COM0B0)|	//	4 bit: Compare Output Mode		/1 clear/set
				(0<<WGM01)|		//	1 bit: Waveform Generation Mode
				(1<<WGM00);		//	0 bit: Waveform Generation Mode
	
	TCCR0B =	(0<<FOC0A)|		//	7 bit: Force Output Compare A
				(0<<FOC0B)|		//	6 bit: Force Output Compare B
				(1<<WGM02)|		//	3 bit: Waveform Generation Mode
				(0<<CS02)|		//	2 bit: Clock Select
				(1<<CS01)|		//	1 bit: Clock Select
				(0<<CS00);		//	0 bit: Clock Select
	
	TCNT0 =		0x00;			//	Текущее значение счетчика
	
	OCR0A =		0b00011000;		//	Output Compare Register A				//	значение до которого считает таймер (12)
	OCR0B = 	0b00000000;		//	Output Compare Register B				//	значение до которого считает таймер
	
	TIMSK0 =	(0<<OCIE0B)|	//	2 bit: Output Compare Match B Interrupt Enable						//	прерывание при сравнении с B
				(1<<OCIE0A)|	//	1 bit: Output Compare Match A Interrupt Enable						//	прерывание при сравнении с A
				(0<<TOIE0);		//	0 bit: Overflow Interrupt Enable									//	прерывание по переполнению
	
	TIFR0 =		(0<<OCF0B)|		//	2 bit: Output Compare Match Flag B
				(0<<OCF0A)|		//	1 bit: Output Compare Match Flag A
				(0<<TOV0);		//	0 bit: Overflow Flag
	sei();
}

void Timer0_ini_CTC(void)
{
/*Таймер для отсчета времени																										*
CTC mode																															*
prescaler 1/8																														*
верх таймера 0x02																													*
16Mhz / 8 = 2 000 000 Hz; 2 000 000 Hz / 0x02 = 1 000 000 Hz ;  1 000 000 Hz = 1 us (милисекунд)									*
(Но при измерении осцилографом ноги переключаемой в прерваниях максимальная частота не больше - 203 kHz, т.е. 4,9 микросекунды)		*
*/
	cli();
	TCCR0A =	(0<<COM0A1)|	//	7 bit: Compare Output Mode
				(0<<COM0A0)|	//	6 bit: Compare Output Mode		/1
				(0<<COM0B1)|	//	5 bit: Compare Output Mode
				(0<<COM0B0)|	//	4 bit: Compare Output Mode
				(1<<WGM01)|		//	1 bit: Waveform Generation Mode
				(0<<WGM00);		//	0 bit: Waveform Generation Mode
		
	TCCR0B =	(0<<FOC0A)|		//	7 bit: Force Output Compare A
				(0<<FOC0B)|		//	6 bit: Force Output Compare B
				(0<<WGM02)|		//	3 bit: Waveform Generation Mode
				(0<<CS02)|		//	2 bit: Clock Select
				(0<<CS01)|		//	1 bit: Clock Select
				(1<<CS00);		//	0 bit: Clock Select
		
	TCNT0 =		0x00;			//	Текущее значение счетчика
		
	OCR0A =		0b00000010;		//	Output Compare Register A				//	значение до которого считает таймер
	OCR0B = 	0b00000000;		//	Output Compare Register B				//	значение до которого считает таймер
		
	TIMSK0 =	(0<<OCIE0B)|	//	2 bit: Output Compare Match B Interrupt Enable						//	прерывание при сравнении с B
				(1<<OCIE0A)|	//	1 bit: Output Compare Match A Interrupt Enable						//	прерывание при сравнении с A
				(0<<TOIE0);		//	0 bit: Overflow Interrupt Enable									//	прерывание по переполнению
		
	TIFR0 =		(0<<OCF0B)|		//	2 bit: Output Compare Match Flag B
				(0<<OCF0A)|		//	1 bit: Output Compare Match Flag A
				(0<<TOV0);		//	0 bit: Overflow Flag
	sei();
}

ISR(TIMER0_COMPA_vect)
{	
	if (return_step_measurement() == 3)
	{
		set_step_2();
	}
	
	if ((return_step_measurement() == 1) | (return_step_measurement() == 2))
	{
		measurement();
	}

}

void Timer0_stop(void)
{
	TCCR0B =	(0<<CS02)|		//	2 bit: Clock Select
				(0<<CS01)|		//	1 bit: Clock Select
				(0<<CS00);		//	0 bit: Clock Select
				
	TCCR0A =	(0<<COM0A1)|	//	7 bit: Compare Output Mode
				(0<<COM0A0)|	//	6 bit: Compare Output Mode		/1
				(0<<COM0B1)|	//	5 bit: Compare Output Mode
				(0<<COM0B0);	//	4 bit: Compare Output Mode
}

