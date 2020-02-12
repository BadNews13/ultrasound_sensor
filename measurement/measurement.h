/*
 * measurement.h
 *
 * Created: 13.11.2019 8:05:44
 *  Author: PC-013
 */ 

#include "../defines/defines_global.h"

#ifndef MEASUREMENT_H_
#define MEASUREMENT_H_

void measurement(void);						//	запустить измерение
uint16_t get_distance(void);				//	возвращает измеренную дистанцию (в см) (см - требование по ТЗ)
uint8_t set_distance(uint8_t _value);		//	записать эталонное расстояние в энергонезависимую память
uint8_t distance_from_eeprom(void);		//	эталонная дистанция из энергонезависимой памяти
uint8_t return_sensor_status(void);			//	возвращает состояние парковочного места (занято/свободно)

uint8_t return_step_measurement(void);
void set_step_2(void);
void set_step_3(void);

void change_data_type_for_uart(uint16_t data);		//	разбить длинное число на несколько двухзначных (чтобы в порте визульно видно было)

#endif /* MEASUREMENT_H_ */