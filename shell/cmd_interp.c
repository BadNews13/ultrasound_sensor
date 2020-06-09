
#include "../defines/defines_global.h"
#include "cmd_interp.h"
#include "../uart/uart.h"
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include "../mirf/mirf.h"
#include "../crc/crc8.h"
#include "../sensor/measurement/measurement.h"


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

uint8_t main_adress = 0;

uint8_t results_for_pc[max_end_device][BYTES_FOR_SAFE_RESULT];		//	массив для хранения отправленных пакетов (пока не получат ACK)

uint8_t number_of_the_generated_pack = 0;							//	номер сгенерированного пакета

static uint8_t	new_pack[MAX_PACK_LENGTH];									//	пакет для отправки собственных команд


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
		//	написать функцию отправки пакетов в сеть низшего уровня (в радио сеть по nRF24L01)
		#ifdef MIRF_ENABLED
			MIRF_SET_TX();
			mirf_write(package_pointer);	//	передаем массив
			MIRF_SET_RX();
			for(uint8_t i = 0; i<package_pointer[0]; i++)	{put_byte(package_pointer[i]);}		//	передаем пакет по uart
		#else
//			for(uint8_t i = 0; i<package_pointer[0]; i++)	{put_byte(package_pointer[i]);}		//	передаем пакет по uart
		#endif
}


uint8_t processing_pack(uint8_t *package_pointer)			//	бработка полученного пакета
{
//for (uint8_t i=0; i<package_pointer[0]; i++)	{put_byte(package_pointer[i]);}		//	выведем в uart что получили (удалить после отладки)
	if (main_adress == 0x00)	{processing_get_name(package_pointer);}		//	если мы не имеем адрес, то запустим функцию получения адреса
	else
	{
		RTOS_DeleteTask(blink);									//	отключим мигание
		if(package_pointer[CMD_BYTES_ADDR_1] == main_adress)	//	если адрес в пакете наш
		{
put_byte(0xA2);
			prepare_answear_pack(package_pointer);				//	подготовим ответный пакет (на сколько можем)
			switch (package_pointer[CMD_BYTES_COMMAND])			//	начинается разбор команд
			{
				case CMD_SENSOR_STATUS:							//	запрос состояния парковочного места (занято/свободно)
				{
put_byte(0x21);
put_byte(package_pointer[CMD_BYTES_COMMAND+1]);
					switch (PARAMETER)	//PARAMETER package_pointer[CMD_BYTES_COMMAND+1]  
					{
						case SENSOR_STATUS:	// STATUS
						{
put_byte(0xA3);
							start_measurement();						//	запускаем измерение
							RTOS_SetTask(send_status,3,0);				//	позже отправим результат (при задержки 1 - проскакивает FF)
							break;
						}
						case SENSOR_BLINK:
						{
put_byte(0xA4);
							blink();
							break;
						}
					}
					break;
				}
				//	запрос точной дистанции до обьекта
				case CMD_SENSOR_GET_DISTANCE:	{break;}	// измерить и отправить результат	(расстояние в см)
				//	установить эталонное значение расстояния срабатывания датчика
				case CMD_SENSOR_SET_DISTANCE:	{break;}	//	записать в eeprom полученное значение
					
				case CMD_RESET:							//	перезагрузить устройство
				{
					/*
					adr_shift_back(package_pointer);
					package_pointer[CMD_BYTES_DATA_OFFSET+1] = CMD_IS_DONE;
					package_pointer[CMD_BYTES_DATALEN]++;
					package_pointer[CMD_BYTES_CRC] = crc8(package_pointer, package_pointer[CMD_BYTES_DATALEN]-1);
					put_pack_back(package_pointer);				//отправить пакет в предыдущую сеть
					reset_device();		//	выполняем после, потому что уже ничего не отправим если перезагрузимся
					*/
					break;
				}
			}
		}
	}
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

void prepare_answear_pack(uint8_t *package_pointer)
{
	//	строим маршрут до мастера, заполняем флаги
	new_pack[CMD_BYTES_DATALEN] = 0x0B;										//	0 byte: длина пакета (переделать на авто расчет)
	new_pack[CMD_BYTES_ADDR_1] = package_pointer[CMD_BYTES_ADDR_5];			//	1 byte: указываем адрес того от кого пришел пакет в этой сети
	new_pack[CMD_BYTES_ADDR_2] = package_pointer[CMD_BYTES_ADDR_4];			//	2 byte: маршрут дальше первой передачи не пойдет
	new_pack[CMD_BYTES_ADDR_3] = ROUTE_END;									//	3 byte: этот байт можно не заполнять. его не прочтут
	new_pack[CMD_BYTES_ADDR_4] = ROUTE_END;									//	4 byte: этот байт нужно выставить в 0
	new_pack[CMD_BYTES_ADDR_5] = package_pointer[CMD_BYTES_ADDR_1];			//	5 byte: собственный адрес
	new_pack[CMD_BYTES_COUNT_PACK] = package_pointer[CMD_BYTES_COUNT_PACK];	//	6 byte: номер пакета (по версии PC)
	sbit(new_pack[CMD_BYTES_FLAGS],CMD_FLAGS_PACK);							//	7 byte: ставим флаг "пакет"
	sbit(new_pack[CMD_BYTES_FLAGS],CMD_FLAGS_ACK_FLAG);						//	7 byte: ставим флаг "ACK"
	new_pack[CMD_BYTES_COMMAND] = CMD_SENSOR_STATUS;						//	8 byte: записываем команду на которую отвечаем	
	new_pack[CMD_BYTES_COMMAND+1] = package_pointer[CMD_BYTES_COMMAND+1];	//	9 byte: состояние места (0xFF - неизвестно)
	new_pack[package_pointer[CMD_BYTES_DATALEN]-1] = 0x00;					//	10 byte: CRC
}

uint8_t get_main_adress(void)		{return main_adress;}
void set_main_adress(uint8_t adr)	{main_adress = adr;}

void blink (void)
{
	static uint8_t light = RED;
	
	switch (light)
	{
		case RED:
		{
			PORTC &= ~(1<<5);	//	выключить красный
			PORTC |= (1<<4);	//	включить зеленый
			light = GREEN;
			break;
		}
		case GREEN:
		{
			PORTC &= ~(1<<4);	//	выключить зеленый
			PORTC |= (1<<5);	//	включить красный
			light = RED;
			break;
		}
	}
	
	RTOS_SetTask(blink,20,0);	
}


void send_status(void)
{
	//	каркас пакета подготовили ранее
	if(the_result_is_ready())	{new_pack[CMD_BYTES_COMMAND+1] = get_result();}	//	положим результат в отправляемый пакет
	new_pack[new_pack[CMD_BYTES_DATALEN]-1] = crc8(&new_pack[0],new_pack[CMD_BYTES_DATALEN]-1);	//	посчитаем crc
	put_pack_back(new_pack);							//	отправим пакет
put_byte(0xCD);
//	for(uint8_t i = 0; i<new_pack[0]; i++)	{put_byte(new_pack[i]);}
}

