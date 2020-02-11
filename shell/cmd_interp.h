/*
 * cmd_interp.h
 *
 * Created: 27.12.2019 14:49:09
 *  Author: bad_n
 */ 


#ifndef CMD_INTERP_H_
#define CMD_INTERP_H_

#define VALUE_CONST_DISTANCE	3		//	���������� � ������� ������������ ��������� �������
#define PACKET_START_BYTE		0xAA	//	��������� ����

#define START_BYTE		0					//	��������� ���� 0xAA
#define ID_RECIPIENT	1					//	����� ����������
#define ID_SENDER		2					//	����� �����������
#define LEN				3					//	����� ������
#define CMD				4					//	�������
#define RES				4					//	��������� ���������� ������� (��� �������� ����� ����, ��� ���� �������)
#define DATA_OFFSET		5					//	���� � �������� ���������� ������
#define CRC				pack[LEN]+1			//	�� � �������, � � ����������� �����

//****������� � ������***
#define CMD_SENSOR_STATUS			0x11	//	������ � ����������� � ������� � ��������� (��������/������) (��� ������ � ���� ����������� ������)
#define CMD_SENSOR_DISTANCE			0x12	//	������ � ����������� � ������� � ���������� ���������� (������� �� �� ����� ����������)
#define CMD_SENSOR_SET_DISTANCE		0x13	//	���������� ���������� ������������ (������� �� �� ����� ����������)
#define CMD_SENSOR_RESET			0x14	//	���������� ��������� ��������� �� ������� (������� �� �� ����� ����������)


void send_crc8(void); // ����� ������ CRC ��� ������-������� ���������


#endif /* CMD_INTERP_H_ */