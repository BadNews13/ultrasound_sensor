
#include "../defines/defines_global.h"
#include "cmd_interp.h"
#include "../uart/uart.h"
#include <avr/eeprom.h>
#include <avr/wdt.h>

uint8_t pack[RX_BUFFER_SIZE];		//	������ ��� �������� ������
	
//	������������ ��� ����������� ������ ������ �� ������ ������ ������
uint8_t flags_package_state = 0;				//	������ �����
uint8_t sequence = 0;							//	���������� ������������� �� ������ ����
#define  PACKET_START_BYTE_FLAG		0			//	���� "������ ��������� ����"
#define  MAIN_ADDRESS_FLAG			1			//	���� "� ������ ��� �����"
#define  PACKET_LENGTH_FLAG			2			//	���� "����� ������ ��������"
#define  PACKET_READY_FLAG			3			//	���� "����� ����� � ���������"
//	==================================================================





/*///////////////////////////////////////////////////////////////
��������� ������:
0 byte - ����� ������
1 byte - ����� ����������
2 byte - ����� �����������
3 byte - �����
4 byte - �������
5 byte - ������
.
N byte - ����������� ����� (crc8)	(N = ����� ������ + 1)
*///////////////////////////////////////////////////////////////
//(����� ���������������� �� ������� � ������� �����, ����� �� ������ ����� ������������� ����������� �����)




uint8_t cmd_exec(uint8_t *pack)
{
	uint8_t funcres;
	funcres=0;
			
	uint8_t cmd_id = pack[CMD_BYTES_DATA_OFFSET];				//	��������� �������
	uint8_t tmp_my_adr = pack[CMD_BYTES_RECEIVER];				//	����� ���� �� ����� �� ������ (�.�. �� ������ ��� ������ ����� �������)
	pack[CMD_BYTES_RECEIVER] = pack[CMD_BYTES_SENDER];			//	��������� ����� ����������
	pack[CMD_BYTES_SENDER] = tmp_my_adr;						//	��������� ����� ����������� (���)
	cbit(pack[CMD_BYTES_FLAGS],CMD_FLAGS_BITS_NETWORK);			//	��������� ���� ACK

		switch (cmd_id)
		{
			case CMD_SENSOR_STATUS:										//	��������� ��������� �����
				while(!(return_step_measurement() == 0));				//	���� ���� ���������� ��������� ���������� (����� ������ ��������� (�������� ���� ��-�� ������ ������� ����� �� � ����� �������))
				measurement();											//	��������
				while((return_distance() == 0))							//	����, ���� � ���������� distance ��������� ��������� (1 ��� ������ ���������)
				pack[CMD_BYTES_DATA_OFFSET+1] = return_sensor_status();	//	��������� ��������� � ����� (������� �� �������. ������ ����� � ��������� ����)

				break;
				
			case CMD_SENSOR_SET_DISTANCE:
				set_distance(pack[CMD_BYTES_DATA_OFFSET+1]);				//	������� ��������� �������� ��������� � eeprom
				if(distance_from_eeprom()==pack[CMD_BYTES_DATA_OFFSET+1])	//	������� ��������� �������� ��������� �� eeprom
				{
					pack[CMD_BYTES_DATA_OFFSET+1] = CMD_IS_DONE;			//	������������� ��������� ������� ������ � �������� ����� (������� �� �������. ������ ����� � ��������� ����)
				}
			
			case CMD_SENSOR_RESET:
				{
					wdt_disable();
					wdt_enable(WDTO_15MS);
					while (1) {}
				}
		}
		
		
		
		
}
