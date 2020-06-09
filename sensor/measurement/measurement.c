
#include "measurement.h"
#include <avr/io.h>
#include <avr/eeprom.h>
#include "../timer0/timer0.h"
#include "../../interrupt/interrupt.h"
#include "../../defines/defines_global.h"

uint8_t ready = 0;			//	флаг - результат готов
uint8_t place = 0;			//	состояние места


//8-ми разрядная переменная в EEPROM
uint8_t reference_distance EEMEM = 100;		//	эталонная дистанция


long distance = 0;					//	дистанция до обьекта
int interrupt_count = 0;			//	количество внешний прерываний от датчика
int times_interrupt[3];				//	массив для хранения тиков таймера на которых произошло внешнее прерывание от датчика

	char uart_byte_1 = 0;											
	char uart_byte_2 = 0;					
	char uart_byte_3 = 0;
	char uart_byte_4 = 0;


uint16_t get_distance(void)
{
	uint16_t dist = (uint16_t)distance;
	return dist;
}

uint8_t set_distance(uint8_t _value)
{
	eeprom_write_byte(&reference_distance,_value);
}

uint8_t distance_from_eeprom(void)
{
	return eeprom_read_byte(&reference_distance);
}


void change_data_type_for_uart(uint16_t data)
{
	// пример 12 345 678;

	while(data>99999999)		//  если число слишком большое, то хотябы увидим старушую часть
		{
			data = data/10;
		}

	uart_byte_1 = data / 1000000;	//	12															//	123456/10000		=12
	uart_byte_2 = (data - uart_byte_1 * 1000000) / 10000;	//	34									//	(123456-(123456/10000)*10000)/100 = (123456-120000)/100 = 3456/100 = 34				
	uart_byte_3 = (data - uart_byte_1 * 1000000 - uart_byte_2 * 10000) / 100;	//	56
	uart_byte_4 = (data - uart_byte_1 * 1000000 - uart_byte_2 * 10000 - uart_byte_3 * 100);	//	78

//			12345678    -     12      * 1000000 -      34     * 10000 -     56      * 100 )
}


void start_measurement(void)
{
	PORTC &= ~(1<<5);		//	выключить красный
	PORTC &= ~(1<<4);		//	выключить зеленый
	
	ready = 0;
	Timer0_stop();			//	если просто макросом отключить, то таймер работает неправильно. Надо отключить прерывания
	
	EIMSK =	(1<<INT1)|		// разрешить прерывание на INT1	(PD3)
			(0<<INT0);		// запретить прерывание на INT0	(PD2)		//	от датчика

	for(uint8_t i = 0; i < 3; i++)	{times_interrupt[i] = 0;}	//	чистим массив времени прерываний
	distance = 0;												//	обнуляем предыдущее значение
	Timer0_ini_PWM();											//	инициализируем таймер, в режим переключения пина OC0A
}

void calculate_distance (void)
{
	TIMER_0_STOP;	//	остановим таймер
/*		
		put_byte(change_number_for_uart(times_interrupt[0]));	//	прерывание при включении разрешении прерывания
		put_byte(change_number_for_uart(times_interrupt[1]));	//	собственная незатухшая волна
		put_byte(change_number_for_uart(times_interrupt[2]));	//	отраженная волна (если есть)
*/
	EIMSK =	(1<<INT1)|		// разрешить прерывание на INT1	(PD3)
			(0<<INT0);		// запретить прерывание на INT0	(PD2)		//	от датчика

	// в будущем использовать эталон для сравнения
	if(times_interrupt[2]<20)		//если первая отраженная волная пришли меньше чем за 20 тиков, то занято
	{
		PORTC &= ~(1<<4);	//	выключить зеленый
		PORTC |= (1<<5);	//	включить красный
		
		PORTC &= ~(1<<1);	//	выключить зеленый на столбе
		PORTC |= (1<<0);	//	включить красный на столбе
		
		place = TAKEN;
		
			//	расчет расстояния до объекта
			distance = 0;
			distance = 33*times_interrupt[2];
			distance = distance*1024;
			distance = distance/16000;
			distance = distance/2;
	}
	else
	{
		PORTC &= ~(1<<5);	//	выключить красный
		PORTC |= (1<<4);	//	включить зеленый
		
		
		PORTC &= ~(1<<0);	//	выключить красный на столбе
		PORTC |= (1<<1);	//	включить зеленый на столбе	
				
		place = FREE;
	}
	interrupt_count = 0;
	ready = 1;		// поднимем флаг - результат готов
	
//	uint8_t temp_byte = (char)distance;		
//	put_byte(change_number_for_uart(temp_byte));
}

void save_tik(int current_tik)		{times_interrupt[interrupt_count++] = current_tik;}

uint8_t get_result(void)			{return place;}		//	передает состояние парковочного места
	
uint8_t the_result_is_ready(void)	{return ready;}		//	показывает состояние процесса измерения