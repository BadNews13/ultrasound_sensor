/*
 * cmd_interp.h
 *
 * Created: 27.12.2019 14:49:09
 *  Author: bad_n
 */ 


#ifndef CMD_INTERP_H_
#define CMD_INTERP_H_

//информация о пакете
#define CMD_BYTES_DATALEN		0	//	длина пакета
#define CMD_BYTES_ADDR_1		1	//	адрес устройства
#define CMD_BYTES_ADDR_2		2	//	адрес устройства
#define CMD_BYTES_ADDR_3		3	//	адрес устройства
#define CMD_BYTES_ADDR_4		4	//	адрес устройства
#define CMD_BYTES_ADDR_5		5	//	адрес устройства
#define CMD_BYTES_COUNT_PACK	6	//	счетчик сгенерированных пакетов (не увеличивается при пересылке)
#define CMD_BYTES_FLAGS			7	//	флаги пакета
#define CMD_BYTES_DATA_OFFSET	8	//	байт с которого начинаются данные является командой (минимум 2 байта, потому что всегда есть параметр, даже если пустой)
//дополнительно
#define CMD_BYTES_ADDR_SOURCE	CMD_BYTES_ADDR_5	// адрес устройства отправителя (при генерции пакета адрес отправителя всегда тут)
#define CMD_BYTES_COMMAND		CMD_BYTES_DATA_OFFSET			//	если команда есть то она в первом байте данных
#define CMD_BYTES_CRC			new_pack[CMD_BYTES_DATALEN]		//	контрольная сумма пакета
#define CMD_BYTES_CRC_IN_ACK	CMD_BYTES_DATA_OFFSET+1
//

//флаги в пакете
#define CMD_FLAGS_PACK			0	//	флаг "это пакет"
#define CMD_FLAGS_ACK_FLAG		1	//	флаг "это ответ"
#define CMD_FLAGS_ERROR			7	//	флаг "ошибка"
//

//Спецсимфолы для пакета
#define CMD_IS_DONE				0xFF	//	команда выполнена
#define ROUTE_END				0x00	//	символ конца маршрута
#define NO_PARAMETERS			0x00	//	Парамметры у команды отсутствуют
//

//полный список команд========================================
#define CMD_SENSOR_STATUS				0x21
#define CMD_SENSOR_GET_DISTANCE			0x22
#define CMD_SENSOR_SET_DISTANCE			0x23

#define CMD_MK_SENSORS_STATUS			0x24
#define CMD_MK_ADD_DEVICE				0x25
#define CMD_MK_DEL_DEVICE				0x26
#define CMD_MK_ADD_DEVICE_TO_GROUP		0x27
#define CMD_MK_DEL_DEVICE_FROM_GROUP	0x28
#define CMD_MK_CLEAR_GROUP				0x29

#define CMD_WRITE_TEXT					0x2A
#define CMD_READ_DI						0x2B
#define CMD_WRITE_DO					0x2C

#define CMD_RESET						0xFF
//============================================================

//для пришедших пакетов (чтобы понять что с ним делать)
#define	our_ACK			0x00	//	ответный пакет для нас
#define	our_pack		0xFF	//	пакет с данными для нас
#define	hop_back		0xF0	//	пакет для пересылки назад
#define hop_forward		0x0F	//	пакет для пересылки вперед
#define reset_pack		0xAA	//	нет флага "пакет" (не будем обрабатывать)
//

//размеры пакета
#define MIN_PACK_LENGTH 11
#define MAX_PACK_LENGTH 128
//


#define	BYTES_FOR_SAFE_RESULT	2		//	количество байт для хранения результата измерения
#define max_end_device			255		//	максимальное количество устройств

void return_ACK_error(uint8_t *package_pointer);
uint8_t calculate_datalen(uint8_t *package_pointer);
void reset_device(void);
uint8_t porcessing_pack(uint8_t *package_pointer);			//	бработка полученного пакета
void send_crc8(void); // чтобы узнать CRC для пакета-запроса измерения


#endif /* CMD_INTERP_H_ */