
#include "../defines/defines_global.h"
#include "cmd_interp.h"
#include "../uart/uart.h"
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include "../mirf/mirf.h"
#include "../crc/crc8.h"


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

uint8_t main_485_adress = 0x0F;										//	����������� ����� (������ 15)

uint8_t sensors_is_live[max_end_device/8];							//	������ ���������� �������� (1 ��� - 1 ������) (����� ���� � ������� = ������ �������)
uint8_t sensors_status[max_end_device/8];							//	������ ����������� �������� (1 ��� - 1 ������) (����� ���� � ������� = ������ �������)
uint8_t adr_sensor = 0;												//	����� ������ ������� ��� ������� ���������� � ������ �������
uint8_t last_adr_dead_sensor = 0;									//	����� ������ ������� ��� ������� ���������� � �������� �������
uint8_t	time_last_request = 0;										//	����� ����� ��� ������ ��������� ������
uint8_t wait_sensor_answer = 0;										//	���� "������ ����� �� �������"

uint8_t results_for_pc[max_end_device][BYTES_FOR_SAFE_RESULT];		//	������ ��� �������� ������������ ������� (���� �� ������� ACK)
uint8_t count_sensors = 0;											//	���-�� ������������ ���������
uint8_t number_of_the_generated_pack = 0;							//	����� ���������������� ������

uint8_t	new_pack[MAX_PACK_LENGTH];									//	����� ��� �������� ����������� ������




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
		#ifdef MIRF_ENABLED
			mirf_write(package_pointer);														//	�������� ����� �� �����
		#else
			for(uint8_t i = 0; i<package_pointer[0]; i++)	{put_byte(package_pointer[i]);}		//	�������� ����� �� uart
		#endif
}


uint8_t porcessing_pack(uint8_t *package_pointer)			//	�������� ����������� ������
{
	if(package_pointer[CMD_BYTES_ADDR_1]==15)			//	���� ����� � ������ ���
	{
		switch (package_pointer[CMD_BYTES_COMMAND])		//	���������� ������ ������
		{
			case CMD_SENSOR_STATUS:					//	������ ��������� ������������ ����� (������/��������)
			{
				// ���������
				// ��������� ��������� (������/��������)
			}
			
			case CMD_SENSOR_GET_DISTANCE:			//	������ ������ ��������� �� �������
			{
				// ���������
				// ��������� ���������	(���������� � ��)
			}

			case CMD_SENSOR_SET_DISTANCE:			//	���������� ��������� �������� ���������� ������������ �������
			{
				//	�������� � eeprom ���������� ��������
			}

			case CMD_RESET:							//	������������� ����������
			{
				adr_shift_back(package_pointer);
				package_pointer[CMD_BYTES_DATA_OFFSET+1] = CMD_IS_DONE;
				package_pointer[CMD_BYTES_DATALEN]++;
				package_pointer[CMD_BYTES_CRC] = crc8(package_pointer, package_pointer[CMD_BYTES_DATALEN]-1);
				put_pack_back(package_pointer);				//��������� ����� � ���������� ����
				reset_device();		//	��������� �����, ������ ��� ��� ������ �� �������� ���� ��������������
			}
		}
	}
}



uint8_t calculate_datalen(uint8_t *package_pointer)
{
	//	������ �����������, �� ���� ���
	for(uint8_t i = MAX_PACK_LENGTH; i>MIN_PACK_LENGTH-1; i--)
	{
		if(!(package_pointer[i] == 0x00))	{return i+1;}
	}
	return 0x00;	//	���� �� �� ������ ����� ��������� �� �������������� ����� �� ����������� ����� 1 ���� CRC, �� ������ 0
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