
#include <avr/interrupt.h>
#include "timer_1.h"

// ���� ������ ����� ��� ����� TCNT1, ������ ��������� � ������� ������ ������� � ����������� � ������ ������� ������������

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
		
	TCNT1H =	0x00;			//	������� �������� �������� (������� ������ 15:8)
	TCNT1L =	0x00;			//	������� �������� �������� (������� ������ 7:0)
	
	OCR1AH =	0x00;			//	Output Compare Register A	(������� ������ 15:8)	//	�������� �� �������� ������� ������
	OCR1AL =	0xFF;			//	Output Compare Register A	(������� ������ 7:0)	//	�������� �� �������� ������� ������
	
	OCR1BH =	0x00;			//	Output Compare Register B	(������� ������ 15:8)	//	�������� �� �������� ������� ������
	OCR1BL = 	0x00;			//	Output Compare Register B	(������� ������ 7:0)	//	�������� �� �������� ������� ������
		
	TIMSK1 =	(0<<ICIE1)|		//	5 bit: Timer/Counter1, Input Capture Interrupt Enable
				(0<<OCIE1B)|	//	2 bit: Output Compare Match B Interrupt Enable						//	���������� ��� ��������� � B
				(0<<OCIE1A)|	//	1 bit: Output Compare Match A Interrupt Enable						//	���������� ��� ��������� � A
				(0<<TOIE1);		//	0 bit: Overflow Interrupt Enable									//	���������� �� ������������
		
	TIFR1 =		(0<<ICF1)|		//	5 bit: Timer/Counter1, Input Capture Flag
				(0<<OCF1B)|		//	2 bit: Output Compare Match Flag B
				(0<<OCF1A)|		//	1 bit: Output Compare Match Flag A
				(0<<TOV1);		//	0 bit: Overflow Flag
}