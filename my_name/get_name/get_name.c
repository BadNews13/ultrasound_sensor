/*
 * get_name.c
 *
 * Created: 06.05.2020 15:30:15
 *  Author: bad_n
 */ 
#include "get_name.h"
#include "../timer_1/timer_1.h"
#include "../../shell/cmd_interp.h"
#include "../../crc/crc8.h"
#include <util/delay.h>
#include "../../RTOS_assembly/rtos/rtos.h"

uint8_t zero_pack[3];				//	����� �� 3 �����
uint8_t count_zero_pack = 0x00;		//	������� �������
uint8_t skip_pack = 1;				//	���� - ���������� ��������� ���� ������ (�� ��������� - 1 - ����������)
uint8_t flag_delay_done = 0;		//	���� - ����� �������

uint8_t wait_send_zero_pack = 0;	//	������� ����� ���� ����� ����� ���������

void put_zero_pack(void)
{
	zero_pack[0] = 0x03;								//	�����
	zero_pack[1] = ++count_zero_pack;					//	����� ������
	zero_pack[2] = crc8(&zero_pack[0],zero_pack[0]-1);	//	crc8
put_byte(0xC0);
	put_pack_back(zero_pack);	//	� ��������� �� ����� �������� ���������� 12 ���� ��-�� ������������� �����. �� �� uart ������ 3
	wait_send_zero_pack = 0;
//for(uint8_t i = 0; i<zero_pack[0]; i++)	{put_byte(zero_pack[i]);}		//	�������� ����� �� uart
}


void processing_get_name(uint8_t *pack)	//	� ����������� �� ����������� ������ ������ ��� ������
{
//for(uint8_t i = 0; i<pack[0]; i++)	{put_byte(pack[i]);}
put_byte(0xFF);
	
	if (pack[0] == 0x03)	//	���� ������ ����� ��� �����, �� ��� ����� �� ������� ������ �������
	{
put_byte(0xD0);
		if (pack[1] > count_zero_pack)		//	���� �� �������� �������
		{
			RTOS_DeleteTask(put_zero_pack);	//	������� ������ �������� �������� ������
			wait_send_zero_pack = 0;		//	�������� ���� "������� ����� �������� ����� ����� ���������"
			skip_pack = 1;					//	�������� ���� "��������� ������������"
put_byte(0xDD);
		}	
	}
	
	if(pack[0]>0x03)
	{
		if (pack[CMD_BYTES_ADDR_5] == MASTER)			//	���� ����� ������� ������
		{
			switch (pack[CMD_BYTES_COMMAND])		//	������� �������
			{
				case CMD_SENSOR_STATUS:				//	���� ������ ����������� status ���������� ��� ������a
				{
put_byte(0x21);		
					if(wait_send_zero_pack == 0)	//	���� ������� ����� �� �������� ����� ����� ���������
					{
						//	���� ��������� ����������� �� ���������� � ����� ����� � ������� (�������, ��� �� ����������)
						if(skip_pack == 0)
						{
							RTOS_SetTask(put_zero_pack,TCNT1,0);
							wait_send_zero_pack = 1;	//	��������� ���� "������� ����� �������� ����� ����� ���������"	(����� �� ����������� �� ������� �������� �� ����������� � ��������� ��� ��������)
put_byte(0x01);
						}
						else
						{
							skip_pack = 0;	//	��������� ���� ����������� � ��������� ���
put_byte(0x00);
						}
					}
					break;
				}

				case CMD_SEARCH_NEW_DEV:			//	���� ������ ����� � ����� �������� � ���� ��
				{
put_byte(0x2E);
					if (skip_pack == 0)				//���� ��������� ����������� - ���������. ����� �� ������� � �����
					{	
						RTOS_SetTask(put_zero_pack,TCNT2,0); put_byte(0x01);	//	���������� � ����� ����� � ������� (�������, ��� �� ����������)
					}
					break;
				}

				case CMD_SENSOR_SET_ADR:	//	���� � ������ ������� ���������� ������
				{
put_byte(0x20);
					if (skip_pack == 0)		//���� ��������� �����������
					{
						set_main_adress(pack[CMD_BYTES_COMMAND+1]);	//	�������� ���� ����� �� ����� ����������
						RTOS_DeleteTask(put_zero_pack);				//	������ ���������� �������� �������� ������
put_byte(pack[CMD_BYTES_COMMAND+1]);
					}
					break;
				}	
			}
		}
	}
				
}
