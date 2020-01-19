
#include <avr/interrupt.h>
#include "timer2.h"

#include <avr/interrupt.h>
int out_pulse_count_test_2 = 0;
	
void Timer2_ini_PWM(void)
{
/*Таймер для генерации импульсов на ноге PIND4 																*
PWM PhaseCorrect mode																						*
prescaler 1																									*
верх таймера 100																							*
При 16Mhz - инверсия OC0A будет происходить с частотой ~82kHz (следовательно импульсы - ~41kHz)				*
*/

//Переделал в CTC-mode т.к. для OC0B отсутствует режим работы toggle в PWM-mode. если делать под один датчик, то можно вернуть PWM и использовать OC0A с последующей перенастройкой ноги для компаратора

/*
	TCCR2A=(1<<WGM21);
	TCCR2B=(1<<CS22)|(1<<CS20);
	TCNT2=0x00;
	OCR2A=0xB3;
	OCR2B=0x00;
	TIMSK2=(1<<OCIE2A);
*/



out_pulse_count_test_2=0;

	cli();
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
				(1<<CS20);		//	0 bit: Clock Select
	
	TCNT2 =		0x00;			//	Текущее значение счетчика
	
	OCR2A =		0b01100100;		//	Output Compare Register A				//	значение до которого считает таймер (12) 100
	OCR2B = 	0b00011100;		//	Output Compare Register B				//	значение до которого считает таймер
	
	TIMSK2 =	(0<<OCIE2B)|	//	2 bit: Output Compare Match B Interrupt Enable						//	прерывание при сравнении с B
				(1<<OCIE2A)|	//	1 bit: Output Compare Match A Interrupt Enable						//	прерывание при сравнении с A
				(0<<TOIE2);		//	0 bit: Overflow Interrupt Enable									//	прерывание по переполнению
	
	TIFR2 =		(0<<OCF2B)|		//	2 bit: Output Compare Match Flag B
				(0<<OCF2A)|		//	1 bit: Output Compare Match Flag A
				(0<<TOV2);		//	0 bit: Overflow Flag
	sei();
}

ISR(TIMER2_COMPA_vect)
{	
	PORTD ^= (1<<PIND4);
	pulses2_for_test();

}

void Timer2_stop(void)
{
	
	TCCR2B =	(0<<CS22)|		//	2 bit: Clock Select
				(0<<CS21)|		//	1 bit: Clock Select
				(0<<CS20);		//	0 bit: Clock Select
				
	TCCR2A =	(0<<COM2A1)|	//	7 bit: Compare Output Mode
				(0<<COM2A0)|	//	6 bit: Compare Output Mode		/1
				(0<<COM2B1)|	//	5 bit: Compare Output Mode
				(0<<COM2B0);	//	4 bit: Compare Output Mode
			
	TIMSK2 =	(0<<OCIE2B)|	//	2 bit: Output Compare Match B Interrupt Enable						//	прерывание при сравнении с B
				(0<<OCIE2A)|	//	1 bit: Output Compare Match A Interrupt Enable						//	прерывание при сравнении с A
				(0<<TOIE2);		//	0 bit: Overflow Interrupt Enable
				
}

void pulses2_for_test(void)
{
	
	out_pulse_count_test_2++;				//	счетчик будет увеличиваться на 1 каждое прерывание таймер (4,9 микросекунд)
	
	if (out_pulse_count_test_2>21)		//	ждем 10 импульсов
	{
		Timer2_stop();				//	останавливаем таймер, который переключает пин OC0A

	}
	
}