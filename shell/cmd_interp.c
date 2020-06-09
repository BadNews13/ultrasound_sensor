
#include "../defines/defines_global.h"
#include "cmd_interp.h"
#include "../uart/uart.h"
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include "../mirf/mirf.h"
#include "../crc/crc8.h"
#include "../sensor/measurement/measurement.h"


/*///////////////////////////////////////////////////////////////
��������� ������:
0 byte - ����� ������
1 byte - ����� ����������
2 byte - �����
3 byte - �����
4 byte - ����� (���������� ����� ���� 0)
5 byte - ����� �����������
6 byte - ������� �������
7 byte - �����
8 byte - �������
9 byte - ������/����������
.
N byte - ����������� ����� (crc8)	(N = ����� ������ + 1)
*///////////////////////////////////////////////////////////////

uint8_t main_adress = 0;

uint8_t results_for_pc[max_end_device][BYTES_FOR_SAFE_RESULT];		//	������ ��� �������� ������������ ������� (���� �� ������� ACK)

uint8_t number_of_the_generated_pack = 0;							//	����� ���������������� ������

static uint8_t	new_pack[MAX_PACK_LENGTH];									//	����� ��� �������� ����������� ������


/////////	������ ��������� �������	/////////

void adr_shift_back(uint8_t *package_pointer)
{
	//�������� ������ �����
	uint8_t temp_byte = package_pointer[CMD_BYTES_ADDR_5];
	for (uint8_t i = CMD_BYTES_ADDR_5; i<CMD_BYTES_ADDR_1; i--)		{package_pointer[i]=package_pointer[i-1];}
	package_pointer[CMD_BYTES_ADDR_1] = temp_byte;
}

void put_pack_back(uint8_t *package_pointer)
{
		//	�������� ������� �������� ������� � ���� ������� ������ (� ����� ���� �� nRF24L01)
		#ifdef MIRF_ENABLED
			MIRF_SET_TX();
			mirf_write(package_pointer);	//	�������� ������
			MIRF_SET_RX();
			for(uint8_t i = 0; i<package_pointer[0]; i++)	{put_byte(package_pointer[i]);}		//	�������� ����� �� uart
		#else
//			for(uint8_t i = 0; i<package_pointer[0]; i++)	{put_byte(package_pointer[i]);}		//	�������� ����� �� uart
		#endif
}


uint8_t processing_pack(uint8_t *package_pointer)			//	�������� ����������� ������
{
//for (uint8_t i=0; i<package_pointer[0]; i++)	{put_byte(package_pointer[i]);}		//	������� � uart ��� �������� (������� ����� �������)
	if (main_adress == 0x00)	{processing_get_name(package_pointer);}		//	���� �� �� ����� �����, �� �������� ������� ��������� ������
	else
	{
		RTOS_DeleteTask(blink);									//	�������� �������
		if(package_pointer[CMD_BYTES_ADDR_1] == main_adress)	//	���� ����� � ������ ���
		{
put_byte(0xA2);
			prepare_answear_pack(package_pointer);				//	���������� �������� ����� (�� ������� �����)
			switch (package_pointer[CMD_BYTES_COMMAND])			//	���������� ������ ������
			{
				case CMD_SENSOR_STATUS:							//	������ ��������� ������������ ����� (������/��������)
				{
put_byte(0x21);
put_byte(package_pointer[CMD_BYTES_COMMAND+1]);
					switch (PARAMETER)	//PARAMETER package_pointer[CMD_BYTES_COMMAND+1]  
					{
						case SENSOR_STATUS:	// STATUS
						{
put_byte(0xA3);
							start_measurement();						//	��������� ���������
							RTOS_SetTask(send_status,3,0);				//	����� �������� ��������� (��� �������� 1 - ������������ FF)
							break;
						}
						case SENSOR_BLINK:
						{
put_byte(0xA4);
							blink();
							break;
						}
					}
					break;
				}
				//	������ ������ ��������� �� �������
				case CMD_SENSOR_GET_DISTANCE:	{break;}	// �������� � ��������� ���������	(���������� � ��)
				//	���������� ��������� �������� ���������� ������������ �������
				case CMD_SENSOR_SET_DISTANCE:	{break;}	//	�������� � eeprom ���������� ��������
					
				case CMD_RESET:							//	������������� ����������
				{
					/*
					adr_shift_back(package_pointer);
					package_pointer[CMD_BYTES_DATA_OFFSET+1] = CMD_IS_DONE;
					package_pointer[CMD_BYTES_DATALEN]++;
					package_pointer[CMD_BYTES_CRC] = crc8(package_pointer, package_pointer[CMD_BYTES_DATALEN]-1);
					put_pack_back(package_pointer);				//��������� ����� � ���������� ����
					reset_device();		//	��������� �����, ������ ��� ��� ������ �� �������� ���� ��������������
					*/
					break;
				}
			}
		}
	}
}






/////////	������ ����������� ������ �� �������	/////////

void reset_device(void)
{
	wdt_disable();
	wdt_enable(WDTO_15MS);
	while (1) {}
}

/////////	����� ����������� ������ �� �������		/////////

void return_ACK_error(uint8_t *package_pointer)
{
	return 0;
}

void prepare_answear_pack(uint8_t *package_pointer)
{
	//	������ ������� �� �������, ��������� �����
	new_pack[CMD_BYTES_DATALEN] = 0x0B;										//	0 byte: ����� ������ (���������� �� ���� ������)
	new_pack[CMD_BYTES_ADDR_1] = package_pointer[CMD_BYTES_ADDR_5];			//	1 byte: ��������� ����� ���� �� ���� ������ ����� � ���� ����
	new_pack[CMD_BYTES_ADDR_2] = package_pointer[CMD_BYTES_ADDR_4];			//	2 byte: ������� ������ ������ �������� �� ������
	new_pack[CMD_BYTES_ADDR_3] = ROUTE_END;									//	3 byte: ���� ���� ����� �� ���������. ��� �� �������
	new_pack[CMD_BYTES_ADDR_4] = ROUTE_END;									//	4 byte: ���� ���� ����� ��������� � 0
	new_pack[CMD_BYTES_ADDR_5] = package_pointer[CMD_BYTES_ADDR_1];			//	5 byte: ����������� �����
	new_pack[CMD_BYTES_COUNT_PACK] = package_pointer[CMD_BYTES_COUNT_PACK];	//	6 byte: ����� ������ (�� ������ PC)
	sbit(new_pack[CMD_BYTES_FLAGS],CMD_FLAGS_PACK);							//	7 byte: ������ ���� "�����"
	sbit(new_pack[CMD_BYTES_FLAGS],CMD_FLAGS_ACK_FLAG);						//	7 byte: ������ ���� "ACK"
	new_pack[CMD_BYTES_COMMAND] = CMD_SENSOR_STATUS;						//	8 byte: ���������� ������� �� ������� ��������	
	new_pack[CMD_BYTES_COMMAND+1] = package_pointer[CMD_BYTES_COMMAND+1];	//	9 byte: ��������� ����� (0xFF - ����������)
	new_pack[package_pointer[CMD_BYTES_DATALEN]-1] = 0x00;					//	10 byte: CRC
}

uint8_t get_main_adress(void)		{return main_adress;}
void set_main_adress(uint8_t adr)	{main_adress = adr;}

void blink (void)
{
	static uint8_t light = RED;
	
	switch (light)
	{
		case RED:
		{
			PORTC &= ~(1<<5);	//	��������� �������
			PORTC |= (1<<4);	//	�������� �������
			light = GREEN;
			break;
		}
		case GREEN:
		{
			PORTC &= ~(1<<4);	//	��������� �������
			PORTC |= (1<<5);	//	�������� �������
			light = RED;
			break;
		}
	}
	
	RTOS_SetTask(blink,20,0);	
}


void send_status(void)
{
	//	������ ������ ����������� �����
	if(the_result_is_ready())	{new_pack[CMD_BYTES_COMMAND+1] = get_result();}	//	������� ��������� � ������������ �����
	new_pack[new_pack[CMD_BYTES_DATALEN]-1] = crc8(&new_pack[0],new_pack[CMD_BYTES_DATALEN]-1);	//	��������� crc
	put_pack_back(new_pack);							//	�������� �����
put_byte(0xCD);
//	for(uint8_t i = 0; i<new_pack[0]; i++)	{put_byte(new_pack[i]);}
}

