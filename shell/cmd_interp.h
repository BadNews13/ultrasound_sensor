/*
 * cmd_interp.h
 *
 * Created: 27.12.2019 14:49:09
 *  Author: bad_n
 */ 


#ifndef CMD_INTERP_H_
#define CMD_INTERP_H_

#define VALUE_CONST_DISTANCE	3		//	расстояние с которым сравнивается показание датчика
#define PACKET_START_BYTE		0xAA	//	системный байт

#define START_BYTE		0					//	стартовый байт 0xAA
#define ID_RECIPIENT	1					//	адрес получателя
#define ID_SENDER		2					//	адрес отправителя
#define LEN				3					//	длина пакета
#define CMD				4					//	команда
#define RES				4					//	результат выполнения команды (при отправке ложим туда, где была команда)
#define DATA_OFFSET		5					//	байт с которого начинаются данные
#define CRC				pack[LEN]+1			//	НЕ К ИНДЕКСУ, А К СОДЕРЖИМОМУ БАЙТА

//****команды в датчик***
#define CMD_SENSOR_STATUS			0x11	//	запрос с КОНТРОЛЛЕРА к ДАТЧИКУ о состоянии (свободно/занято) (для записи в свою оперативную память)
#define CMD_SENSOR_DISTANCE			0x12	//	запрос с КОНТРОЛЛЕРА к ДАТЧИКУ о измеренном расстоянии (команда из ПК через КОНТРОЛЛЕР)
#define CMD_SENSOR_SET_DISTANCE		0x13	//	установить расстояние срабатывания (команда из ПК через КОНТРОЛЛЕР)
#define CMD_SENSOR_RESET			0x14	//	установить заводские настройки на датчике (команда из ПК через КОНТРОЛЛЕР)


void send_crc8(void); // чтобы узнать CRC для пакета-запроса измерения


#endif /* CMD_INTERP_H_ */