
#include "measurement.h"
#include <avr/io.h>
//#include "interrupt0/interrupt0.h"


#define max_delay_interrupt 6000			//	������������ ����� �������� ���������� (60000 - ��� 10 ������)

int step = 0;								//	��� ������� ���������
int distance = 0;							//	����������
int out_pulse_count = 0;					//	������������ ��������
int interrupt_count = 0;					//	���������� ������� ����������

int time_count = 0;						//	����� �� �������
int time_interrupt = 0;					//	����� ������������ ����������

//float time_count = 0;						//	����� �� �������
//float time_interrupt = 0;					//	����� ������������ ����������

	char uart_byte_1 = 0;											
	char uart_byte_2 = 0;					
	char uart_byte_3 = 0;
	char uart_byte_4 = 0;


void measurement(void)
{
	switch (step)
	{
		
		case 0:								//	0-� ���: ������
		{
			distance = 0;					//	�������� ���������� ��������
			out_pulse_count = 0;			//	�������� ���������� ��������
			interrupt_count = 0;			//	�������� ���������� ��������
			time_count = 0;					//	�������� ���������� ��������		
			Timer0_ini_PWM();				//	�������������� ������, � ����� ������������ ���� OC0A
			step = 1;						//	������� � ���������� ���� ��� ��������� ������
			
			return;
		}
		
		case 1:								//	1-� ���: �������� ��������� � ������ �������
		{
			out_pulse_count++;				//	������� ����� ������������� �� 1 ������ ���������� ������ (4,9 �����������)
			
			if (out_pulse_count==39)		//	���� 10 ���������
			{
				Timer0_stop();				//	������������� ������, ������� ����������� ��� OC0A
				PORTD = (0<<PIND6);			//	��� OC0B ���� (��� �� ��� �� ������� � 1 ��� ������/�������� ���������� ������������)
				Timer0_ini_CTC();			//	�������������� ������ � ����� ������� �������
				Int0_ini();					//	�������������� ������� ����������
				step = 2;					//	������� � ���������� ���� ��� ��������� ������
			}
			return;
		}
		
		case 2:
		{
			time_count++;					//	������� �����
			
			// ���� ������� � ����������� ������ ����������, �� ������� ��� ��������� � ������� ���������
			if (time_count - time_interrupt > max_delay_interrupt)			
			{
				distance = ( ( 340 * time_interrupt ) / 2 ) / 10000;		//	[ ��������� = (�������� ����������� * �����) / 2 ]. ��������� ��������� � ����������� ���� /10000
//				write_float(time_interrupt);								//	�������� �� ����� ���������

				change_data_type_for_uart(time_interrupt);				// ������� ����� �� 4 �����
				putch_usart(uart_byte_1);
				putch_usart(uart_byte_2);
				putch_usart(uart_byte_3);
				putch_usart(uart_byte_4);
								
				change_data_type_for_uart(distance);				// ������� ����� �� 4 �����
				putch_usart(uart_byte_1);
				putch_usart(uart_byte_2);
				putch_usart(uart_byte_3);
				putch_usart(uart_byte_4);
				step = 4;
				
							Timer0_stop();					//	������������� ������, ������� ����������� ��� OC0A
							Int0_disabled();				//	��������� ������� ����������
							step = 0;						//	������ ���� - 3-� ��� ��������, ������� � 0-��� ���� � ��������� ������ �������

			}
			return;
		}
		
		case 3:								//	2-� ���: �������� ����������
		{
			time_interrupt = time_count;	//	��������� ����� ����������	
				
			//	���� ���������� �� �����������
			interrupt_count++;
			if (interrupt_count>10)
			{
				step = 4;
			}
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

void change_data_type_for_uart(int data)
{
	// ������ 12 345 678;
	
	uart_byte_1 = data / 1000000;	//	12												//	123456/10000		=12
	uart_byte_2 = (data - uart_byte_1 * 1000000) / 10000;	//	34							//	(123456-(123456/10000)*10000)/100 = (123456-120000)/100 = 3456/100 = 34				
	uart_byte_3 = (data - uart_byte_1 * 1000000 - uart_byte_2 * 10000) / 100;			//	56
	uart_byte_4 = (data - uart_byte_1 * 1000000 - uart_byte_2 * 10000 - uart_byte_3 * 100);		//78

//			12345678    -     12      * 1000000 -      34     * 10000 -     56      * 100 )
}