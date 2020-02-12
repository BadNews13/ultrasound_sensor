
#include "../defines/defines_global.h"
#include "cmd_interp.h"
#include "../uart/uart.h"
#include <avr/eeprom.h>
#include <avr/wdt.h>

uint8_t pack[RX_BUFFER_SIZE];		//	массив дл€ хранени€ пакета
	
//	используетс€ дл€ выт€гивани€ нашего пакета из общего потока данных
uint8_t flags_package_state = 0;				//	хранит флаги
uint8_t sequence = 0;							//	количество вытаскиваемых из буфера байт
#define  PACKET_START_BYTE_FLAG		0			//	флаг "пришел стартовый байт"
#define  MAIN_ADDRESS_FLAG			1			//	флаг "в пакете наш адрес"
#define  PACKET_LENGTH_FLAG			2			//	флаг "длина пакета получена"
#define  PACKET_READY_FLAG			3			//	флаг "пакет готов к обработке"
//	==================================================================





/*///////////////////////////////////////////////////////////////
структура пакета:
0 byte - ƒлина пакета
1 byte - јдрес получател€
2 byte - јдрес отправител€
3 byte - флаги
4 byte -  оманда
5 byte - ƒанные
.
N byte -  онтрольна€ сумма (crc8)	(N = ƒлина пакета + 1)
*///////////////////////////////////////////////////////////////
//(пакет идентифицируетс€ по первому и второму байту, затем на основе длины высчитываетс€ контрольна€ сумма)




uint8_t cmd_exec(uint8_t *pack)
{
	uint8_t funcres;
	funcres=0;
			
	uint8_t cmd_id = pack[CMD_BYTES_DATA_OFFSET];				//	извлекаем команду
	uint8_t tmp_my_adr = pack[CMD_BYTES_RECEIVER];				//	берем свой же адрес из пакета (т.к. из пакета его сейчас легче достать)
	pack[CMD_BYTES_RECEIVER] = pack[CMD_BYTES_SENDER];			//	вставл€ем адрес получател€
	pack[CMD_BYTES_SENDER] = tmp_my_adr;						//	вставл€ем адрес отправител€ (наш)
	cbit(pack[CMD_BYTES_FLAGS],CMD_FLAGS_BITS_NETWORK);			//	поднимаем флаг ACK

		switch (cmd_id)
		{
			case CMD_SENSOR_STATUS:										//	проверить состо€ние места
				while(!(return_step_measurement() == 0));				//	ждем пока предыдущее измерение закончитс€ (вдруг дважды зупастили (например если из-за ошибки прин€ли пакет не с нашим адресом))
				measurement();											//	измер€ем
				while((return_distance() == 0))							//	ждем, пока в переменную distance запишетс€ результат (1 или верна€ дистанци€)
				pack[CMD_BYTES_DATA_OFFSET+1] = return_sensor_status();	//	вставл€ем результат в пакет (команду не трогаем. кладем ответ в следующий байт)

				break;
				
			case CMD_SENSOR_SET_DISTANCE:
				set_distance(pack[CMD_BYTES_DATA_OFFSET+1]);				//	«апишем эталонное значение дистанции в eeprom
				if(distance_from_eeprom()==pack[CMD_BYTES_DATA_OFFSET+1])	//	—читаем эталонное значение дистанции из eeprom
				{
					pack[CMD_BYTES_DATA_OFFSET+1] = CMD_IS_DONE;			//	подвтерждение выполнен€ команды кладем в ответный пакет (команду не трогаем. кладем ответ в следующий байт)
				}
			
			case CMD_SENSOR_RESET:
				{
					wdt_disable();
					wdt_enable(WDTO_15MS);
					while (1) {}
				}
		}
		
		
		
		
}
