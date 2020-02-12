/*
 * measurement.h
 *
 * Created: 13.11.2019 8:05:44
 *  Author: PC-013
 */ 

#include "../defines/defines_global.h"

#ifndef MEASUREMENT_H_
#define MEASUREMENT_H_

void measurement(void);						//	��������� ���������
uint16_t get_distance(void);				//	���������� ���������� ��������� (� ��) (�� - ���������� �� ��)
uint8_t set_distance(uint8_t _value);		//	�������� ��������� ���������� � ����������������� ������
uint8_t distance_from_eeprom(void);		//	��������� ��������� �� ����������������� ������
uint8_t return_sensor_status(void);			//	���������� ��������� ������������ ����� (������/��������)

uint8_t return_step_measurement(void);
void set_step_2(void);
void set_step_3(void);

void change_data_type_for_uart(uint16_t data);		//	������� ������� ����� �� ��������� ����������� (����� � ����� �������� ����� ����)

#endif /* MEASUREMENT_H_ */