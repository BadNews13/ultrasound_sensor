/*
 * cmd_interp.h
 *
 * Created: 27.12.2019 14:49:09
 *  Author: bad_n
 */ 


#ifndef CMD_INTERP_H_
#define CMD_INTERP_H_


//информация о пакете
#define CMD_BYTES_DATALEN		0
#define CMD_BYTES_ADDR_NET_1	1
#define CMD_BYTES_ADDR_NET_2	2
#define CMD_BYTES_ADDR_NET_3	3
#define CMD_BYTES_ADDR_NET_4	4
#define CMD_BYTES_ADDR_BACK		5
#define CMD_BYTES_COUNT_PACK	6
#define CMD_BYTES_FLAGS			7
#define CMD_BYTES_DATA_OFFSET	8								
#define CMD_BYTES_COMMAND		8								//	если команда есть то она в первом байте данных
#define CMD_BYTES_CRC			pack[CMD_BYTES_DATALEN]+1		//	НЕ К ИНДЕКСУ, А К СОДЕРЖИМОМУ БАЙТА
//

#define CMD_IS_DONE				0xFF	//	команда выполнена

//****команды в датчик***
#define CMD_SENSOR_STATUS			0x11	//	запрос с КОНТРОЛЛЕРА к ДАТЧИКУ о состоянии (свободно/занято) (для записи в свою оперативную память)
#define CMD_SENSOR_DISTANCE			0x12	//	запрос с КОНТРОЛЛЕРА к ДАТЧИКУ о измеренном расстоянии (команда из ПК через КОНТРОЛЛЕР)
#define CMD_SENSOR_SET_DISTANCE		0x13	//	установить расстояние срабатывания (команда из ПК через КОНТРОЛЛЕР)
#define CMD_SENSOR_RESET			0x14	//	установить заводские настройки на датчике (команда из ПК через КОНТРОЛЛЕР)

//список флагов
#define CMD_FLAGS_BITS_PACK 0
#define CMD_FLAGS_BITS_ACK 1
#define CMD_FLAGS_BITS_NETWORK 2
#define CMD_FLAGS_BITS_ERR 7
//


void send_crc8(void); // чтобы узнать CRC для пакета-запроса измерения


#endif /* CMD_INTERP_H_ */