
#include "../defines/defines_global.h"
#include "cmd_interp.h"
#include "../uart/uart.h"

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
0 byte - ��������� ���� (������ 0xAA)
1 byte - ����� ����������
2 byte - ����� �����������
3 byte - ����� ������ (��� ���������� ���� � ��� ����������� �����)
4 byte - �������
5 byte - ������
.
N byte - ����������� ����� (crc8)	(N = ����� ������ + 1)
*///////////////////////////////////////////////////////////////
//(����� ���������������� �� ������� � ������� �����, ����� �� ������ ����� ������������� ����������� �����)


// ������� ��� ����� � ������
void search_for_my_package(void)
{
	uint16_t tmp_rx_byte = get_byte();
	
	if (tmp_rx_byte == 0xFFFF)	{return;}										//	���� ����� ���, �� �������� 0xFFFF. ������ ������ ����� �� ����
	
//	0-� ���� (START_BYTE)
	if (sequence == 0)															//	���� ��� 0-� ����, ������� ����������
	{
		if (!(bit_is_set(flags_package_state, PACKET_START_BYTE_FLAG)))			//	���� �� ���������� ���� - ��������� ���� �������
		{
			if (tmp_rx_byte == PACKET_START_BYTE)								//	���� ���� ���� ��������� (����� 0xAA)
			{
				put_byte(0x00);//------------------��� �����
				sbit(flags_package_state, PACKET_START_BYTE_FLAG);				//	��������� ���� "������ ��������� ����"
				pack[START_BYTE] = tmp_rx_byte;									//	(�� ������� �� �����, �� ��� ��������� ������� ������� ���)
				sequence++;														//	�������� ���������� ��������� ����
				return;
			}
		}
	}

//	1-� ���� (ADDRESS)
	if (sequence == 1)																//	���� ��� 1-� ����, ������� ����������
	{
		if (bit_is_set(flags_package_state, PACKET_START_BYTE_FLAG))				//	����  ���������� ���� - ��������� ���� �������
		{
			if (tmp_rx_byte == return_main_rs485_address())							//  ���� � ���� ����� ����� ����� ����������
			{
				put_byte(0x01);//--------------��� �����
				sbit(flags_package_state, MAIN_ADDRESS_FLAG);						//	��������� ���� "� ������ ��� �����
				pack[ID_RECIPIENT] = tmp_rx_byte;									//	���������� ���� ���� � ����������� ����� �� ��������������� �����
				sequence++;															//	�������� ���������� ��������� ����
				return;
			}
			else
			{
				cbit(flags_package_state, PACKET_START_BYTE_FLAG);					//	�������� ���� "��������� ���� �������"
				sequence = 0;														//	�������� ������� ��������� ����
			}
		}
	}

//	��� ����� 0-�� � 1-��
	if (sequence > 1)																//	���� ��� 2-� ��� ������ ����
	{
		if (bit_is_set(flags_package_state, PACKET_START_BYTE_FLAG))				//	����  ���������� ���� "��������� ���� �������"
		{
			if (bit_is_set(flags_package_state, MAIN_ADDRESS_FLAG))					//	���� ���������� ���� "� ������ ����� ����� ����������"
			{
				put_byte(0x02);	//--------------��� �����
				if (sequence == 3) {sbit(flags_package_state, PACKET_LENGTH_FLAG);}	//	���� ��� 3-� ����, �� ��������� ���� "����� ������ ��������"}
				pack[sequence] = tmp_rx_byte;
				sequence++;															//	���������� � �������� �������
			}
		}
	}

//	��������� �������� �� �� ���� �����
	if (	bit_is_set(flags_package_state, PACKET_LENGTH_FLAG)		)				//	���� ���������� ���� "�������� ����� ������"
	{
		if (sequence == pack[LEN] + 1)
		{
			cbit(flags_package_state, PACKET_START_BYTE_FLAG);						//	�������� ���� "��������� ���� �������"
			cbit(flags_package_state, MAIN_ADDRESS_FLAG);							//	�������� ���� "� ������ ����� ����� ����������"
			sequence = 0;															//	�������� ������� ��������� ����
			
			if (	pack[sequence] == crc8(&pack[1], pack[LEN])		)				//	���� ����������� ����� ��������� � � ���������� � ������
			{
				sbit(flags_package_state, PACKET_READY_FLAG);						//	��������� ���� "����� ����� � ���������"
					uint8_t i = 0;
					while(i<pack[LEN] + 1) {put_byte(pack[i++]);}					//	��� �����
			}
		}
	}
}


uint8_t cmd_exec(uint8_t *pack)
{
	uint8_t funcres;
	funcres=0;
			
	uint8_t cmd_id = pack[CMD];								//	��������� �������
	uint8_t tmp_my_adr = pack[ID_RECIPIENT];				//	����� ���� �� ����� �� ������ (�.�. �� ������ ��� ������ ����� �������)
	pack[ID_RECIPIENT] = pack[ID_SENDER];					//	��������� ����� ����������
	pack[ID_SENDER] = tmp_my_adr;							//	��������� ����� ����������� (���)
	pack[LEN] = 5;

		switch (cmd_id)
		{
			case CMD_SENSOR_STATUS:								//	��������� ��������� �����
				while(!(return_step_measurement() == 0));		//	���� ���� ���������� ��������� ���������� (����� ������ ��������� (�������� ���� ��-�� ������ ������� ����� �� � ����� �������))
				measurement();									//	��������
				while((return_distance() == 0))					//	����, ���� � ���������� distance ��������� ��������� (1 ��� ������ ���������)
				pack[DATA_OFFSET] = CMD_SENSOR_STATUS;			//	��������� � ����� ������� �� ������� ��������
				pack[DATA_OFFSET+1] = return_sensor_status();		//	��������� ��������� � �����
				funcres = 0xFF;									//	������� ���������
				break;
		}
	
	pack[RES] = funcres;
	pack[CRC] = crc8(&pack[0], pack[LEN]);
	
	put_byte(0xAA);												//	��������� ��������� ����
	for(uint8_t i=1; i<=pack[LEN]+1; i++)	{put_byte(pack[i]);}	//	��������� �����
}

void send_crc8(void)
{
//������ ������ ��� ������
	
	pack[ID_RECIPIENT] = 0x0D;									//	����� ���������� ���������� 13
	pack[ID_SENDER]	= 0x00;										//	����� ���������� ����������� 00
	pack[LEN] = 0x04;											//	����� ������ 04 �����
	pack[CMD] = 0x04;											//	������� 04 (�������� ����������)
	pack[DATA_OFFSET] = 0x00;									//	������ ������������ � ������������� ����������
	pack[CRC] = crc8(&pack[1], pack[LEN]);				//	������ ����������� �����

	put_byte(0xAA);												//	��������� ��������� ����
	for(uint8_t i=1; i<=pack[LEN]+1; i++)	{put_byte(pack[i]);}	//	��������� �����

}