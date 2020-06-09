/*
 * get_name.c
 *
 * Created: 06.05.2020 15:30:15
 *  Author: bad_n
 */ 
#include "get_name.h"
#include "../timer_1/timer_1.h"
#include "../../shell/cmd_interp.h"
#include "../../crc/crc8.h"
#include <util/delay.h>
#include "../../RTOS_assembly/rtos/rtos.h"

uint8_t zero_pack[3];				//	пакет на 3 байта
uint8_t count_zero_pack = 0x00;		//	счетчик пакетов
uint8_t skip_pack = 1;				//	флаг - пропустить следующий цикл опроса (по умолчанию - 1 - пропустить)
uint8_t flag_delay_done = 0;		//	флаг - пауза выждана

uint8_t wait_send_zero_pack = 0;	//	нулевой пакет ждет паузу перед отправкой

void put_zero_pack(void)
{
	zero_pack[0] = 0x03;								//	Длина
	zero_pack[1] = ++count_zero_pack;					//	номер пакета
	zero_pack[2] = crc8(&zero_pack[0],zero_pack[0]-1);	//	crc8
put_byte(0xC0);
	put_pack_back(zero_pack);	//	к сожалению по радио всеравно отправится 12 байт из-за фиксированной длины. но по uart только 3
	wait_send_zero_pack = 0;
//for(uint8_t i = 0; i<zero_pack[0]; i++)	{put_byte(zero_pack[i]);}		//	передаем пакет по uart
}


void processing_get_name(uint8_t *pack)	//	в зависимости от полученного пакета решаем что делать
{
//for(uint8_t i = 0; i<pack[0]; i++)	{put_byte(pack[i]);}
put_byte(0xFF);
	
	if (pack[0] == 0x03)	//	если прошло всего три байта, то это пакет от другого нового датчика
	{
put_byte(0xD0);
		if (pack[1] > count_zero_pack)		//	если от старшего датчика
		{
			RTOS_DeleteTask(put_zero_pack);	//	удаляем задачу отправки нулевого пакета
			wait_send_zero_pack = 0;		//	опускаем флаг "нулевой пакет выжидает паузу перед отправкой"
			skip_pack = 1;					//	подниман флаг "запретить реагирование"
put_byte(0xDD);
		}	
	}
	
	if(pack[0]>0x03)
	{
		if (pack[CMD_BYTES_ADDR_5] == MASTER)			//	если пакет оправил мастер
		{
			switch (pack[CMD_BYTES_COMMAND])		//	смотрим команду
			{
				case CMD_SENSOR_STATUS:				//	если мастер запрашивает status известного ему датчикa
				{
put_byte(0x21);		
					if(wait_send_zero_pack == 0)	//	если нулевой пакет не выжидает паузу перед отправкой
					{
						//	если разрешено реагировать то отправляем в линию пакет с номером (говорим, что мы существуем)
						if(skip_pack == 0)
						{
							RTOS_SetTask(put_zero_pack,TCNT1,0);
							wait_send_zero_pack = 1;	//	поднимаем флаг "нулевой пакет выжидает паузу перед отправкой"	(чтобы не реагировать на запросы статусов от контроллера к известным ему датчикам)
put_byte(0x01);
						}
						else
						{
							skip_pack = 0;	//	разрешаем себе реагировать в следующий раз
put_byte(0x00);
						}
					}
					break;
				}

				case CMD_SEARCH_NEW_DEV:			//	если мастер знает о новых датчиках и ищет их
				{
put_byte(0x2E);
					if (skip_pack == 0)				//если разрешено реагировать - реагируем. Иначе не влезаем в линию
					{	
						RTOS_SetTask(put_zero_pack,TCNT2,0); put_byte(0x01);	//	отправляем в линию пакет с номером (говорим, что мы существуем)
					}
					break;
				}

				case CMD_SENSOR_SET_ADR:	//	если в пакете команда присвоения адреса
				{
put_byte(0x20);
					if (skip_pack == 0)		//если разрешено реагировать
					{
						set_main_adress(pack[CMD_BYTES_COMMAND+1]);	//	присвоим себе адрес из байта параметров
						RTOS_DeleteTask(put_zero_pack);				//	удалим отложенную отправку нулевого пакета
put_byte(pack[CMD_BYTES_COMMAND+1]);
					}
					break;
				}	
			}
		}
	}
				
}
