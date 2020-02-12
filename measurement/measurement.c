
#include "measurement.h"
#include <avr/io.h>
#include <avr/eeprom.h>

//8-�� ��������� ���������� � EEPROM
uint8_t reference_distance EEMEM = 100;		//	��������� ���������

#define max_delay_interrupt 10000		//	������������ ����� �������� ���������� (60000 - ��� 10 ������)


uint8_t step = 0;						//	��� ������� ���������
uint16_t distance = 0;					//	����������
int out_pulse_count = 0;				//	������������ ��������
int interrupt_count = 0;				//	���������� ������� ����������

int time_count = 0;						//	����� �� �������
int time_interrupt = 0;					//	����� ������������ ����������

//float time_count = 0;					//	����� �� �������
//float time_interrupt = 0;				//	����� ������������ ����������

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
			time_count++;					//	������� ����� (��� ������ ���������� ������� �� ���� ��������)
			
			// ���� ������� � ����������� ������ ����������, �� ������� ��� ��������� � ������� ���������
			if (time_count - time_interrupt > max_delay_interrupt)			
			{
				distance = ( ( 340 * time_interrupt ) / 2 ) / 100;		//	[ ��������� = (�������� ����������� * �����) / 2 ]. ��������� ��������� � ����������� ���� /10000
//				write_float(time_interrupt);							//	�������� �� ����� ���������
				if (distance == 0) {distance = 1;}						//	���� ���� ����, ��� ��������� ��������� ���� ���� ��� �� ������
				change_data_type_for_uart(time_interrupt);				// ������� ����� �� 4 �����
/*
				putch_usart(uart_byte_1);
				putch_usart(uart_byte_2);
				putch_usart(uart_byte_3);
				putch_usart(uart_byte_4);
*/																	
			//	putch0(change_number_for_uart(uart_byte_1));
			//	putch0(change_number_for_uart(uart_byte_2));
			//	putch0(change_number_for_uart(uart_byte_3));
			//	putch0(change_number_for_uart(uart_byte_4));				
	
				change_data_type_for_uart(distance);				// ������� ����� �� 4 �����
/*				
				putch_usart(uart_byte_1);
				putch_usart(uart_byte_2);
				putch_usart(uart_byte_3);
				putch_usart(uart_byte_4);
*/				
			//	putch0(change_number_for_uart(uart_byte_1));
			//	putch0(change_number_for_uart(uart_byte_2));
			//	putch0(change_number_for_uart(uart_byte_3));
			//	putch0(change_number_for_uart(uart_byte_4));
				
				
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
				Timer0_stop();					//	������������� ������, ������� ����������� ��� OC0A
				Int0_disabled();				//	��������� ������� ����������
				step = 0;						//	������ ���� - 3-� ��� ��������, ������� � 0-��� ���� � ��������� ������ �������
			}
			return;
		}
	}
}


uint16_t get_distance(void)
{
	return distance;
}

uint8_t set_distance(uint8_t _value)
{
	eeprom_write_byte(&reference_distance,_value);
}

uint8_t distance_from_eeprom(void)
{
	return eeprom_read_byte(&reference_distance);
}

uint8_t return_sensor_status(void)
{
//	return sensor_status;
}


uint8_t return_step_measurement(void)
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

void change_data_type_for_uart(uint16_t data)
{
	// ������ 12 345 678;

	while(data>99999999)		//  ���� ����� ������� �������, �� ������ ������ �������� �����
		{
			data = data/10;
		}

	uart_byte_1 = data / 1000000;	//	12															//	123456/10000		=12
	uart_byte_2 = (data - uart_byte_1 * 1000000) / 10000;	//	34									//	(123456-(123456/10000)*10000)/100 = (123456-120000)/100 = 3456/100 = 34				
	uart_byte_3 = (data - uart_byte_1 * 1000000 - uart_byte_2 * 10000) / 100;	//	56
	uart_byte_4 = (data - uart_byte_1 * 1000000 - uart_byte_2 * 10000 - uart_byte_3 * 100);	//	78

//			12345678    -     12      * 1000000 -      34     * 10000 -     56      * 100 )
}
