
#include "measurement.h"
#include <avr/io.h>
//#include "interrupt0/interrupt0.h"


#define max_delay_interrupt 6000		//	������������ ����� �������� ���������� (60000 - ��� 10 ������)

int step = 0;							//	��� ������� ���������
int distance = 0;						//	����������
int out_pulse_count = 0;				//	������������ ��������
int interrupt_count = 0;				//	���������� ������� ����������
float time_count = 0;					//	����� �� �������
float time_interrupt = 0;				//	����� ������������ ����������



void measurement(void)
{
	switch (step)
	{
		
		case 0:							//	0-� ���: ������
		{
			distance = 0;				//	�������� ���������� ��������
			out_pulse_count = 0;		//	�������� ���������� ��������
			interrupt_count = 0;		//	�������� ���������� ��������
			time_count = 0;				//	�������� ���������� ��������		
			Timer0_ini_PWM();			//	�������������� ������, � ����� ������������ ���� OC0A
			step = 1;					//	������� � ���������� ���� ��� ��������� ������
			
			return;
		}
		
		case 1:							//	1-� ���: �������� ��������� � ������ �������
		{
			out_pulse_count++;			//	������� ����� ������������� �� 1 ������ ���������� ������ (4,9 �����������)
			
			if (out_pulse_count==39)	//	���� 10 ���������
			{
				Timer0_stop();			//	������������� ������, ������� ����������� ��� OC0A
				PORTD = (0<<PIND6);		//	��� OC0B ���� (��� �� ��� �� ������� � 1 ��� ������/�������� ���������� ������������)
				Timer0_ini_CTC();		//	�������������� ������ � ����� ������� �������
				Int0_ini_down();		//	�������������� ������� ����������
				step = 2;				//	������� � ���������� ���� ��� ��������� ������
			}
			return;
		}
		
		case 2:
		{
			time_count++;				//	������� �����
			
			// ���� ������� � ����������� ������ ����������, �� ������� ��� ��������� � ������� ���������
			if (time_count - time_interrupt > max_delay_interrupt)			
			{
				distance = ( ( 340 * time_interrupt ) / 2 ) / 10000;		//	��������� ��������� � �����������		/10000
				write_float(time_interrupt);								//	�������� �� ����� ���������
				step = 4;
			}
			
			return;
		}
		
		case 3:								//	2-� ���: �������� ����������
		{
			time_interrupt = time_count;	//	��������� ����� ����������
			change_interrup();				//	����������� ���������� 
			return;
		}
		
		case 4:							//	3-� ���: �������� ��� ������ ��� ����������
		{
			Timer0_stop();				//	������������� ������, ������� ����������� ��� OC0A
			Int0_disabled();			//	��������� ������� ����������
			step = 0;					//	������ ���� - 3-� ��� ��������, ������� � 0-��� ���� � ��������� ������ �������
			return;
		}
	}
}


int return_step_measurement()
{
	return step;
}

void set_step_3(void)
{
	step = 3;
}

void set_step_2(void)
{
	step = 2;
}

