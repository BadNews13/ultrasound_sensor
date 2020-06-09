#include "../../defines/defines_global.h"

#define  F_CPU 16000000UL

#include <avr/interrupt.h>
#include "timer0.h"
#include "../measurement/measurement.h"

#include "../../shell/cmd_interp.h"
#include "../../my_name/get_name/get_name.h"

uint8_t timer_mode = 0;			//	�����, ���������� � ����� ������ � ������� ������
int 	tik = 0;				//	���-�� ����� ������� (�� ����� 380 000/1024 = 371 ��� ��������� 4 ������ �� �������)


	
void Timer0_ini_PWM(void)
{
//	������ ��� ��������� ��������� �� PWM ����								*
//	PWM PhaseCorrect mode													*
//	no prescaler															*
//	TOP = OCRA = 100 (0x64)													*
//	For 16Mhz: toggle OC0A = 160 000kHz (pulses = toggle OC0A / 4 = 40kHz)	*

//��������� � CTC-mode �.�. ��� OC0B ����������� ����� ������ toggle � PWM-mode. ���� ������ ��� ���� ������, �� ����� ������� PWM � ������������ OC0A � ����������� �������������� ���� ��� �����������

	TCCR0A =	(0<<COM0A1)|	//	7 bit: Compare Output Mode
				(1<<COM0A0)|	//	6 bit: Compare Output Mode		/1 toggle � PWM
				(0<<COM0B1)|	//	5 bit: Compare Output Mode		
				(0<<COM0B0)|	//	4 bit: Compare Output Mode		/1 clear/set
				(0<<WGM01)|		//	1 bit: Waveform Generation Mode
				(1<<WGM00);		//	0 bit: Waveform Generation Mode
	
	TCCR0B =	(0<<FOC0A)|		//	7 bit: Force Output Compare A
				(0<<FOC0B)|		//	6 bit: Force Output Compare B
				(1<<WGM02)|		//	3 bit: Waveform Generation Mode
				(0<<CS02)|		//	2 bit: Clock Select
				(0<<CS01)|		//	1 bit: Clock Select
				(1<<CS00);		//	0 bit: Clock Select
	
	TCNT0 =		0x00;			//	������� �������� ��������
	
	OCR0A =		100;			//	Output Compare Register A
	OCR0B = 	0;				//	Output Compare Register B
	
	TIFR0 =		(1<<OCF0B)|		//	2 bit: Output Compare Match Flag B	//	���������� �����
				(1<<OCF0A)|		//	1 bit: Output Compare Match Flag A
				(1<<TOV0);		//	0 bit: Overflow Flag
	
	TIMSK0 =	(0<<OCIE0B)|	//	2 bit: Output Compare Match B Interrupt Enable						//	���������� ��� ��������� � B
				(1<<OCIE0A)|	//	1 bit: Output Compare Match A Interrupt Enable						//	���������� ��� ��������� � A
				(0<<TOIE0);		//	0 bit: Overflow Interrupt Enable									//	���������� �� ������������
	
//	TCCR0B |=	(0<<CS02)|(0<<CS01)|(1<<CS00);		// start

	timer_mode = PWM;
	tik = 0;
}

void Timer0_ini_CTC(void)
{
/*
������ ��� ������� ������� (������� ���� ������)	*
CTC mode											*
prescaler 1/1024									*
���� ������� 0xFF									*
*/
	TCCR0A =	(0<<COM0A1)|	//	7 bit: Compare Output Mode
				(0<<COM0A0)|	//	6 bit: Compare Output Mode		/1
				(0<<COM0B1)|	//	5 bit: Compare Output Mode
				(0<<COM0B0)|	//	4 bit: Compare Output Mode
				(1<<WGM01)|		//	1 bit: Waveform Generation Mode
				(0<<WGM00);		//	0 bit: Waveform Generation Mode
		
	TCCR0B =	(0<<FOC0A)|		//	7 bit: Force Output Compare A
				(0<<FOC0B)|		//	6 bit: Force Output Compare B
				(0<<WGM02)|		//	3 bit: Waveform Generation Mode
				(1<<CS02)|		//	2 bit: Clock Select
				(0<<CS01)|		//	1 bit: Clock Select
				(1<<CS00);		//	0 bit: Clock Select
		
	TCNT0 =		0x00;			//	������� �������� ��������
		
	OCR0A =		0x01;			//	Output Compare Register A				//	�������� �� �������� ������� ������
	OCR0B = 	0x00;			//	Output Compare Register B				//	�������� �� �������� ������� ������
	
	TIFR0 =		(1<<OCF0B)|		//	2 bit: Output Compare Match Flag B
				(1<<OCF0A)|		//	1 bit: Output Compare Match Flag A
				(1<<TOV0);		//	0 bit: Overflow Flag
		
	TIMSK0 =	(0<<OCIE0B)|	//	2 bit: Output Compare Match B Interrupt Enable						//	���������� ��� ��������� � B
				(1<<OCIE0A)|	//	1 bit: Output Compare Match A Interrupt Enable						//	���������� ��� ��������� � A
				(0<<TOIE0);		//	0 bit: Overflow Interrupt Enable									//	���������� �� ������������
				
//	TCCR0B |=	(1<<CS02)|(0<<CS01)|(1<<CS00);		// start
	
	timer_mode = CTC;
	tik = 0;
}

ISR(TIMER0_COMPA_vect)
{	
	switch (timer_mode)							//	� ����������� �� ���� � ����� ������ ������� ������
	{
		case PWM:								//	����� ��������� �������������� �����
		{
			tik++;								//	������� ����� ������������� �� 1 � ������ ���������� ������� (4,9 �����������)
			if (tik > 20)						//	���� 10 ���������
			{
				TIMER_0_STOP;					//	������������� ������, ������� ����������� ��� OC0A
//				Timer0_stop();
				PORTD = (0<<PIND6);				//	��� OC0B ���� (����� ��� �� ������� � 1 ��� ������/�������� ���������� ������������)		
				Timer0_ini_CTC();				//	��������� ������ � ������ CTC ��� ������� �������
				
				EIMSK =	(1<<INT1)|		// ��������� ���������� �� INT1	(PD3)
						(1<<INT0);		// ��������� ���������� �� INT0	(PD2)		//	�� �������
			}
			break;
		}
		case CTC:								//	����� ������� �������
		{
			tik++;								//	�������� ���������� �����
			if(tik > 350)	{calculate_distance();}		//	������ 4 ������ ������ �� �����������, ������ ��� ��� ����� ����� (��� 4 ������ - 387 878 �����)
			break;
		}
	}
}

//	������� ���� ����� �������� ����� TIMER_0_STOP
void Timer0_stop(void)
{
	TCCR0B =	(0<<CS02)|		//	2 bit: Clock Select
				(0<<CS01)|		//	1 bit: Clock Select
				(0<<CS00);		//	0 bit: Clock Select
				
	TIMSK0 =	(0<<OCIE0B)|	//	2 bit: Output Compare Match B Interrupt Enable						//	���������� ��� ��������� � B
				(0<<OCIE0A)|	//	1 bit: Output Compare Match A Interrupt Enable						//	���������� ��� ��������� � A
				(0<<TOIE0);		//	0 bit: Overflow Interrupt Enable									//	���������� �� ������������

}

int get_tik(void)	{return tik;}	//	��� �������� �������� �������� ���� ������� � ������ ������� �������
	