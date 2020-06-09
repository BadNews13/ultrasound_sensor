
#include "measurement.h"
#include <avr/io.h>
#include <avr/eeprom.h>
#include "../timer0/timer0.h"
#include "../../interrupt/interrupt.h"
#include "../../defines/defines_global.h"

uint8_t ready = 0;			//	���� - ��������� �����
uint8_t place = 0;			//	��������� �����


//8-�� ��������� ���������� � EEPROM
uint8_t reference_distance EEMEM = 100;		//	��������� ���������


long distance = 0;					//	��������� �� �������
int interrupt_count = 0;			//	���������� ������� ���������� �� �������
int times_interrupt[3];				//	������ ��� �������� ����� ������� �� ������� ��������� ������� ���������� �� �������

	char uart_byte_1 = 0;											
	char uart_byte_2 = 0;					
	char uart_byte_3 = 0;
	char uart_byte_4 = 0;


uint16_t get_distance(void)
{
	uint16_t dist = (uint16_t)distance;
	return dist;
}

uint8_t set_distance(uint8_t _value)
{
	eeprom_write_byte(&reference_distance,_value);
}

uint8_t distance_from_eeprom(void)
{
	return eeprom_read_byte(&reference_distance);
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


void start_measurement(void)
{
	PORTC &= ~(1<<5);		//	��������� �������
	PORTC &= ~(1<<4);		//	��������� �������
	
	ready = 0;
	Timer0_stop();			//	���� ������ �������� ���������, �� ������ �������� �����������. ���� ��������� ����������
	
	EIMSK =	(1<<INT1)|		// ��������� ���������� �� INT1	(PD3)
			(0<<INT0);		// ��������� ���������� �� INT0	(PD2)		//	�� �������

	for(uint8_t i = 0; i < 3; i++)	{times_interrupt[i] = 0;}	//	������ ������ ������� ����������
	distance = 0;												//	�������� ���������� ��������
	Timer0_ini_PWM();											//	�������������� ������, � ����� ������������ ���� OC0A
}

void calculate_distance (void)
{
	TIMER_0_STOP;	//	��������� ������
/*		
		put_byte(change_number_for_uart(times_interrupt[0]));	//	���������� ��� ��������� ���������� ����������
		put_byte(change_number_for_uart(times_interrupt[1]));	//	����������� ���������� �����
		put_byte(change_number_for_uart(times_interrupt[2]));	//	���������� ����� (���� ����)
*/
	EIMSK =	(1<<INT1)|		// ��������� ���������� �� INT1	(PD3)
			(0<<INT0);		// ��������� ���������� �� INT0	(PD2)		//	�� �������

	// � ������� ������������ ������ ��� ���������
	if(times_interrupt[2]<20)		//���� ������ ���������� ������ ������ ������ ��� �� 20 �����, �� ������
	{
		PORTC &= ~(1<<4);	//	��������� �������
		PORTC |= (1<<5);	//	�������� �������
		
		PORTC &= ~(1<<1);	//	��������� ������� �� ������
		PORTC |= (1<<0);	//	�������� ������� �� ������
		
		place = TAKEN;
		
			//	������ ���������� �� �������
			distance = 0;
			distance = 33*times_interrupt[2];
			distance = distance*1024;
			distance = distance/16000;
			distance = distance/2;
	}
	else
	{
		PORTC &= ~(1<<5);	//	��������� �������
		PORTC |= (1<<4);	//	�������� �������
		
		
		PORTC &= ~(1<<0);	//	��������� ������� �� ������
		PORTC |= (1<<1);	//	�������� ������� �� ������	
				
		place = FREE;
	}
	interrupt_count = 0;
	ready = 1;		// �������� ���� - ��������� �����
	
//	uint8_t temp_byte = (char)distance;		
//	put_byte(change_number_for_uart(temp_byte));
}

void save_tik(int current_tik)		{times_interrupt[interrupt_count++] = current_tik;}

uint8_t get_result(void)			{return place;}		//	�������� ��������� ������������ �����
	
uint8_t the_result_is_ready(void)	{return ready;}		//	���������� ��������� �������� ���������