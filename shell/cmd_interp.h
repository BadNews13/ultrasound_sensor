/*
 * cmd_interp.h
 *
 * Created: 27.12.2019 14:49:09
 *  Author: bad_n
 */ 


#ifndef CMD_INTERP_H_
#define CMD_INTERP_H_

//���������� � ������
#define CMD_BYTES_DATALEN		0	//	����� ������
#define CMD_BYTES_ADDR_1		1	//	����� ����������
#define CMD_BYTES_ADDR_2		2	//	����� ����������
#define CMD_BYTES_ADDR_3		3	//	����� ����������
#define CMD_BYTES_ADDR_4		4	//	����� ����������
#define CMD_BYTES_ADDR_5		5	//	����� ����������
#define CMD_BYTES_COUNT_PACK	6	//	������� ��������������� ������� (�� ������������� ��� ���������)
#define CMD_BYTES_FLAGS			7	//	����� ������
#define CMD_BYTES_DATA_OFFSET	8	//	���� � �������� ���������� ������ �������� �������� (������� 2 �����, ������ ��� ������ ���� ��������, ���� ���� ������)
//�������������
#define CMD_BYTES_ADDR_SOURCE	CMD_BYTES_ADDR_5	// ����� ���������� ����������� (��� �������� ������ ����� ����������� ������ ���)
#define CMD_BYTES_COMMAND		CMD_BYTES_DATA_OFFSET			//	���� ������� ���� �� ��� � ������ ����� ������
#define CMD_BYTES_CRC			new_pack[CMD_BYTES_DATALEN]		//	����������� ����� ������
#define CMD_BYTES_CRC_IN_ACK	CMD_BYTES_DATA_OFFSET+1
//

//����� � ������
#define CMD_FLAGS_PACK			0	//	���� "��� �����"
#define CMD_FLAGS_ACK_FLAG		1	//	���� "��� �����"
#define CMD_FLAGS_ERROR			7	//	���� "������"
//

//����������� ��� ������
#define CMD_IS_DONE				0xFF	//	������� ���������
#define ROUTE_END				0x00	//	������ ����� ��������
#define NO_PARAMETERS			0x00	//	���������� � ������� �����������
//

//������ ������ ������========================================
#define CMD_SENSOR_STATUS				0x21
#define CMD_SENSOR_GET_DISTANCE			0x22
#define CMD_SENSOR_SET_DISTANCE			0x23

#define CMD_MK_SENSORS_STATUS			0x24
#define CMD_MK_ADD_DEVICE				0x25
#define CMD_MK_DEL_DEVICE				0x26
#define CMD_MK_ADD_DEVICE_TO_GROUP		0x27
#define CMD_MK_DEL_DEVICE_FROM_GROUP	0x28
#define CMD_MK_CLEAR_GROUP				0x29

#define CMD_WRITE_TEXT					0x2A
#define CMD_READ_DI						0x2B
#define CMD_WRITE_DO					0x2C

#define CMD_RESET						0xFF
//============================================================

//��� ��������� ������� (����� ������ ��� � ��� ������)
#define	our_ACK			0x00	//	�������� ����� ��� ���
#define	our_pack		0xFF	//	����� � ������� ��� ���
#define	hop_back		0xF0	//	����� ��� ��������� �����
#define hop_forward		0x0F	//	����� ��� ��������� ������
#define reset_pack		0xAA	//	��� ����� "�����" (�� ����� ������������)
//

//������� ������
#define MIN_PACK_LENGTH 11
#define MAX_PACK_LENGTH 128
//


#define	BYTES_FOR_SAFE_RESULT	2		//	���������� ���� ��� �������� ���������� ���������
#define max_end_device			255		//	������������ ���������� ���������

void return_ACK_error(uint8_t *package_pointer);
uint8_t calculate_datalen(uint8_t *package_pointer);
void reset_device(void);
uint8_t porcessing_pack(uint8_t *package_pointer);			//	�������� ����������� ������
void send_crc8(void); // ����� ������ CRC ��� ������-������� ���������


#endif /* CMD_INTERP_H_ */