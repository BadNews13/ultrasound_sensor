/*
 * measurement.h
 *
 * Created: 13.11.2019 8:05:44
 *  Author: PC-013
 */ 

#include "../../defines/defines_global.h"

#ifndef MEASUREMENT_H_
#define MEASUREMENT_H_

// distance = (((V*100)/1000) * (tik*prescaler) * ((1/F_CPU)*1000)) / 2		[cm]

#define TAKEN	0x01;
#define FREE	0x00;

void measurement(void);						//	��������� ���������
uint16_t get_distance(void);				//	���������� ���������� ��������� (� ��) (�� - ���������� �� ��)
uint8_t set_distance(uint8_t _value);		//	�������� ��������� ���������� � ����������������� ������
uint8_t distance_from_eeprom(void);		//	��������� ��������� �� ����������������� ������
uint8_t return_sensor_status(void);			//	���������� ��������� ������������ ����� (������/��������)

void start_measurement(void);			//	��������� ���������
void save_tik(int current_tik);			//	������������� ���������� ����� � ������ ����������
uint8_t get_result(void);					//	��������� � ������� ���������
uint8_t the_result_is_ready(void);			//	��������� ������ ��������� �� ���������

void change_data_type_for_uart(uint16_t data);		//	������� ������� ����� �� ��������� ����������� (����� � ����� �������� ����� ����)



#endif /* MEASUREMENT_H_ */