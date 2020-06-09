
#include <avr/io.h>
#include <avr/interrupt.h>
#include "interrupt.h"
#include "../mirf/mirf.h"
#include "../defines/defines_global.h"
#include "../uart/uart.h"

#include "../sensor/measurement/measurement.h"
#include "../sensor/timer0/timer0.h"

//#include "../soft_uart/soft_uart_1/soft_uart_1.h"
//#include "../soft_uart/timer_1/timer_1.h"

volatile uint8_t inter_count = 0;

uint8_t in_data_to_mirf_for_test[mirf_PAYLOAD];


void int_ini(void)
{
	//прерывания INT0 по восходящему фронту		(перепроверить: сейчас настроено на прерывание когда нога на земле (реагирует не на фронт))
	EICRA =	(1<<ISC11)|		//	3 bit: событие для INT1			//	реагировать на спадающий фронт
			(0<<ISC10)|		//	2 bit: событие для INT1
			(1<<ISC01)|		//	1 bit: событие для INT0			//	реагировать на падение уровеня (1)
			(1<<ISC00);		//	0 bit: событие для INT0			//				-//-

	EIMSK =	(1<<INT1)|		// разрешить прерывание на INT1	(PD3)
			(1<<INT0);		// разрешить прерывание на INT0	(PD2)		//	от датчика
}

//	прерывание датчиком
ISR(INT0_vect)
{
	save_tik(get_tik());		//	передаем количество тиков от таймера в функцию измерения
}


//	прерывание радиомодулем
ISR(INT1_vect)
{
	put_byte(0xB4);

	uint8_t status_mirf;
	status_mirf = mirf_get_status();					//	запрашиваем статус nRF24L01

	if(bit_is_set(status_mirf,RX_DR))					//	если поднят флаг "получены данные"
	{
		mirf_read(&in_data_to_mirf_for_test[0]);		//	считываем
		sbit(status_mirf,RX_DR);						//	сбросим флаг успешного получения
		mirf_write_register(STATUS, status_mirf);		//	сбросим флаг успешного получения
put_byte(in_data_to_mirf_for_test[1]);
		processing_pack(in_data_to_mirf_for_test);		//	отправим пакет на обработку
		//for (uint8_t i=0; i<in_data_to_mirf_for_test[0]; i++)	{put_byte(in_data_to_mirf_for_test[i]);}		//	выведем в uart что получили (удалить после отладки)
	}
	
	if(bit_is_set(status_mirf,TX_DS))					//	если поднят флаг "Данные успешно отправлены"
	{
		sbit(status_mirf,TX_DS);						//	сбросим флаг "Данные успешно отправлены"
		mirf_write_register(STATUS, status_mirf);		//	сбросим флаг "Данные успешно отправлены"
	}
	
	if(bit_is_set(status_mirf,MAX_RT))					//	если поднят флаг "Превышения попыток отправки"
	{
		sbit(status_mirf,MAX_RT);						//	сбросим флаг "Превышения попыток отправки"
		mirf_write_register(STATUS, status_mirf);		//	сбросим флаг "Превышения попыток отправки"
		mirf_cmd(FLUSH_TX);								//	очистим буфер отправки
	}
	mirf_clear();
	//	RTOS_SetTask(mirf_clear, 2, 0);
}