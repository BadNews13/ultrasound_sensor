
#include "../defines/defines_global.h"
#include "cmd_interp.h"
#include "../uart/uart.h"
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include "../mirf/mirf.h"
#include "../crc/crc8.h"


/*///////////////////////////////////////////////////////////////
структура пакета:
0 byte - Длина пакета
1 byte - Адрес получателя
2 byte - Адрес
3 byte - Адрес
4 byte - Адрес (спецсимвол конца пути 0)
5 byte - Адрес отправителя
6 byte - Счетчик пакетов
7 byte - флаги
8 byte - Команда
9 byte - Данные/парамметры
.
N byte - Контрольная сумма (crc8)	(N = Длина пакета + 1)
*///////////////////////////////////////////////////////////////

uint8_t main_485_adress = 0x0F;										//	собственный адрес (сейчас 15)

uint8_t sensors_is_live[max_end_device/8];							//	массив работающих датчиков (1 бит - 1 датчик) (номер бита в массиве = адресу датчика)
uint8_t sensors_status[max_end_device/8];							//	массив результатов датчиков (1 бит - 1 датчик) (номер бита в массиве = адресу датчика)
uint8_t adr_sensor = 0;												//	номер адреса датчика для запроса результата с живого сенсора
uint8_t last_adr_dead_sensor = 0;									//	номер адреса датчика для запроса результата с мертвого сенсора
uint8_t	time_last_request = 0;										//	время когда был сделан последний запрос
uint8_t wait_sensor_answer = 0;										//	флаг "ожидаю ответ от датчика"

uint8_t results_for_pc[max_end_device][BYTES_FOR_SAFE_RESULT];		//	массив для хранения отправленных пакетов (пока не получат ACK)
uint8_t count_sensors = 0;											//	кол-во подключенных устройств
uint8_t number_of_the_generated_pack = 0;							//	номер сгенерированного пакета

uint8_t	new_pack[MAX_PACK_LENGTH];									//	пакет для отправки собственных команд




/////////	НАЧАЛО СЛУЖЕБНЫХ ФУНКЦИИ	/////////

void adr_shift_back(uint8_t *package_pointer)
{
	//сдвигаем адреса назад
	uint8_t temp_byte = package_pointer[CMD_BYTES_ADDR_5];
	for (uint8_t i = CMD_BYTES_ADDR_5; i<CMD_BYTES_ADDR_1; i--)		{package_pointer[i]=package_pointer[i-1];}
	package_pointer[CMD_BYTES_ADDR_1] = temp_byte;
}

void put_pack_back(uint8_t *package_pointer)
{
		#ifdef MIRF_ENABLED
			mirf_write(package_pointer);														//	передаем пакет по радио
		#else
			for(uint8_t i = 0; i<package_pointer[0]; i++)	{put_byte(package_pointer[i]);}		//	передаем пакет по uart
		#endif
}


uint8_t porcessing_pack(uint8_t *package_pointer)			//	бработка полученного пакета
{
	if(package_pointer[CMD_BYTES_ADDR_1]==15)			//	если адрес в пакете наш
	{
		switch (package_pointer[CMD_BYTES_COMMAND])		//	начинается разбор команд
		{
			case CMD_SENSOR_STATUS:					//	запрос состояния парковочного места (занято/свободно)
			{
				// измерение
				// отправить результат (занято/свободно)
			}
			
			case CMD_SENSOR_GET_DISTANCE:			//	запрос точной дистанции до обьекта
			{
				// измерение
				// отправить результат	(расстояние в см)
			}

			case CMD_SENSOR_SET_DISTANCE:			//	установить эталонное значение расстояния срабатывания датчика
			{
				//	записать в eeprom полученное значение
			}

			case CMD_RESET:							//	перезагрузить устройство
			{
				adr_shift_back(package_pointer);
				package_pointer[CMD_BYTES_DATA_OFFSET+1] = CMD_IS_DONE;
				package_pointer[CMD_BYTES_DATALEN]++;
				package_pointer[CMD_BYTES_CRC] = crc8(package_pointer, package_pointer[CMD_BYTES_DATALEN]-1);
				put_pack_back(package_pointer);				//отправить пакет в предыдущую сеть
				reset_device();		//	выполняем после, потому что уже ничего не отправим если перезагрузимся
			}
		}
	}
}



uint8_t calculate_datalen(uint8_t *package_pointer)
{
	//	способ примитивный, но пака так
	for(uint8_t i = MAX_PACK_LENGTH; i>MIN_PACK_LENGTH-1; i--)
	{
		if(!(package_pointer[i] == 0x00))	{return i+1;}
	}
	return 0x00;	//	если мы не узнали длину опускаясь от максимумальной длины до минимальной минус 1 байт CRC, то вернем 0
}



/////////	НАЧАЛО ВЫПОЛНЯЕМЫХ КОМАНД ПО ЗАПРОСУ	/////////

void reset_device(void)
{
	wdt_disable();
	wdt_enable(WDTO_15MS);
	while (1) {}
}

/////////	КОНЕЦ ВЫПОЛНЯЕМЫХ КОМАНД ПО ЗАПРОСУ		/////////

void return_ACK_error(uint8_t *package_pointer)
{
	return 0;
}