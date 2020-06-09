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

void measurement(void);						//	запустить измерение
uint16_t get_distance(void);				//	возвращает измеренную дистанцию (в см) (см - требование по ТЗ)
uint8_t set_distance(uint8_t _value);		//	записать эталонное расстояние в энергонезависимую память
uint8_t distance_from_eeprom(void);		//	эталонная дистанция из энергонезависимой памяти
uint8_t return_sensor_status(void);			//	возвращает состояние парковочного места (занято/свободно)

void start_measurement(void);			//	запустить измерение
void save_tik(int current_tik);			//	зафиксировать количество тиков в момент прерывания
uint8_t get_result(void);					//	посчитать и вернуть результат
uint8_t the_result_is_ready(void);			//	позволяет узнать завершено ли измерение

void change_data_type_for_uart(uint16_t data);		//	разбить длинное число на несколько двухзначных (чтобы в порте визульно видно было)



#endif /* MEASUREMENT_H_ */