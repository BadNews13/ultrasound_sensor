
#include "../defines/defines_global.h"
#include "cmd_interp.h"
#include "../uart/uart.h"

 uint8_t flags_package_state = 0;

uint8_t pack[RX_BUFFER_SIZE];	

#define  PACKET_START_BYTE_FLAG 0
#define  MAIN_ADDRESS_FLAG 1
#define  PACKET_LENGTH_FLAG 2
#define  PACKET_READY_FLAG 3

uint8_t sequence = 0;		// количество вытаскиваемых из буфера байт

/*///////////////////////////////////////////////////////////////
структура пакета:
0 byte - Стартовый байт (всегда 0xAA)
1 byte - Адрес получателя
2 byte - Адрес отправителя
3 byte - Длина пакета (без стартового бита и без контрольной суммы)
4 byte - Команда
5 byte - Данные
.
.
.
last byte - Контрольная сумма (crc8)
*///////////////////////////////////////////////////////////////
//(пакет идентифицируется по первому и второму байту)

void search_for_my_package(void)
{
	uint16_t tmp_rx_byte = get_byte();
	
	if (tmp_rx_byte == 0xFFFF)	{return;}
	
//	0-й байт (START_BYTE)
	if (sequence == 0)															//	если это 0-й байт, который вытягиваем
	{
		if (!(bit_is_set(flags_package_state, PACKET_START_BYTE_FLAG)))			//	если не установлен флаг - стартовый байт получен
		{
			if (tmp_rx_byte == PACKET_START_BYTE)								//	если этот байт стартовый (равен 0xAA)
			{
				put_byte(0x00);//------------------
				sbit(flags_package_state, PACKET_START_BYTE_FLAG);				//	поднимаем флаг - стартовый байт получен
				pack[START_BYTE] = tmp_rx_byte;									//	(Он менятся не будет, но для избежания путаниц запишем его)
				sequence++;														//	увеличим количество пришедших байт
				return;
			}
		}
	}

//	1-й байт (ADDRESS)
	if (sequence == 1)																//	если это 1-й байт, который вытягиваем
	{
		if (bit_is_set(flags_package_state, PACKET_START_BYTE_FLAG))				//	если  установлен флаг - стартовый байт получен
		{
			if (tmp_rx_byte == return_main_rs485_address())							//  если в этом байте адрес этого устройства
			{
				put_byte(0x01);//--------------
				sbit(flags_package_state, MAIN_ADDRESS_FLAG);						//	поднимаем флаг "в пакете адрес этого устройства"
				pack[ID_RECIPIENT] = tmp_rx_byte;									//	записываем этот байт в принимаемый пакет на соответствующее место
				sequence++;															//	увеличим количество пришедших байт
				return;
			}
			else
			{
				cbit(flags_package_state, PACKET_START_BYTE_FLAG);					//	опускаем флаг "стартовый байт получен"
				sequence = 0;														//	обнуляем счетчик вытянутых байт
			}
		}
	}

//	все кроме 0-го и 1-го
	if (sequence > 1)																//	если это 2-й или больше байт
	{
		if (bit_is_set(flags_package_state, PACKET_START_BYTE_FLAG))				//	если  установлен флаг "стартовый байт получен"
		{
			if (bit_is_set(flags_package_state, MAIN_ADDRESS_FLAG))					//	если установлен флаг "в пакете адрес этого устройства"
			{
				put_byte(0x02);	//--------------
				if (sequence == 3) {sbit(flags_package_state, PACKET_LENGTH_FLAG);}	//	если это 3-й байт, то поднимаем флаг "длина пакета получена"}
				pack[sequence] = tmp_rx_byte;
				sequence++;
			}
		}
	}

//	проверяем получили ли мы весь пакет
	if (	bit_is_set(flags_package_state, PACKET_LENGTH_FLAG)		)				//	если установлен флаг "получена длина пакета"
	{
		if (sequence == pack[LEN] + 1)
		{
			cbit(flags_package_state, PACKET_START_BYTE_FLAG);						//	опускаем флаг "стартовый байт получен"
			cbit(flags_package_state, MAIN_ADDRESS_FLAG);							//	опускаем флаг "в пакете адрес этого устройства"
			sequence = 0;															//	обнуляем счетчик вытянутых байт
			
			if (	pack[sequence] == crc8(&pack[1], pack[LEN])		)				//	если контрольная сумма совпадает в с полученной в пакете
			{
				sbit(flags_package_state, PACKET_READY_FLAG);						//	поднимаем флаг - пакета готов к обработке
					uint8_t i = 0;
					while(i<pack[LEN] + 1) {put_byte(pack[i++]);}					//	для теста
			}
		}
	}
}




uint8_t cmd_exec(uint8_t *pack)
{
	if(
	(pack[0] == 0x13)|
	(pack[1] == 0x00)|
	(pack[2] == 0x04)|
	(pack[3] == 0x04)|
	(pack[4] == 0xB0));
	{
//		send_Uart(0x07);
	}
/*	
	uint8_t funcres;
	funcres=0;
			
	uint8_t cmd_id = pack[CMD];								//	извлекаем команду
	uint8_t tmp_my_adr = pack[ID_RECIPIENT];				//	берем свой же адрес из пакета (т.к. из пакета его сейчас легче достать)
	pack[ID_RECIPIENT] = pack[ID_SENDER];					//	вставляем адрес получателя
	pack[ID_SENDER] = tmp_my_adr;							//	вставляем адрес отправителя (наш)
	pack[LEN] = 5;

		switch (cmd_id)
		{
			case CHECK_STATE_PLACE:								//	проверить состояние места
				while(!(return_step_measurement() == 0));		//	ждем пока предыдущее измерение закончится (вдруг дважды зупастили (например если из-за ошибки приняли пакет не с нашим адресом))
				measurement();									//	измеряем
				while((return_distance() == 0))					//	ждем, пока в переменную distance запишется результат (1 или верная дистанция)
				pack[DATA_OFFSET] = return_distance();			//	вставляем результат в пакет
				funcres = 1;
				break;
		}
	
	pack[RES] = funcres;
	pack[CRC] = crc8(&pack[0], pack[LEN]);
	
	putch(0xAA, stdout);
	for (int i = 0; i<LEN+1; i++)
	{
		putch(pack[i], stdout);
	}
*/
}

void send_crc8(void)
{
//сборка пакета для тестов
	
	int pack[10];
/*	
	pack[ID_RECIPIENT] = 0x0D;		//	адрес устройства получателя 13
	pack[ID_SENDER]	= 0x00;			//	адрес устройства отправителя 00
	pack[LEN] = 0x04;				//	длина пакета 04 байта
	pack[CMD] = 0x04;				//	команда 04 (измерить расстояние)
	pack[DATA_OFFSET] = 0x00;		//	данные передаваемые в измерительное устройство
	pack[DATA_OFFSET] = crc8(&pack[1], pack[LEN]);
*/	
	
	
/*	
	pack[0]= 0x10;		//	длина пакета
	pack[1]= 0x10;		//	адрес получателя 16
	pack[2]= 0x00;		//	адрес отправителя 0
	pack[3]= 0x00;		//	флаги
	pack[4]= 0x2C;		//	команда вывести строку
	pack[5]= 0x03;		//	Зеленого цвета	
	
	pack[6]='D';
	pack[7]='O';
	pack[8]='N';
	pack[9]='T';
	pack[10]= 0x20;
	pack[11]='W';
	pack[12]='O';
	pack[13]='R';
	pack[14]='R';
	pack[15]='Y';
	pack[16]= 0x20;
	pack[17]='B';
	pack[18]='E';
	pack[19]= 0x20;;
	pack[20]='H';
	pack[21]='A';
	pack[22]='P';
	pack[23]='P';
	pack[24]='Y';
	
	pack[25]= 0x00;		//crc8
*/	
	
// покет для табло	
	
	pack[0] = 0x07;		//	длина пакета
	pack[1] = 0x10;		//	адрес получателя 16
	pack[2] = 0x00;		//	адрес отправителя 0
	pack[3] = 0x00;		//	флаги
	pack[4] = 0x2C;		//	команда вывести строку
	pack[5] = 0x03;		//	Зеленого цвета
	
	pack[6] ='D';		//	текст
	
	pack[7]= crc8(&pack[0], 7);		//crc8
	
	for(int i=0; i<=7; i++)
	{
		put_byte(pack[i]);
	}

	

//	put_byte(0xAA);
/*	
	for(int i=1; i<=pack[LEN]+1; i++)
	{
		put_byte(pack[i]);
	}*/
}