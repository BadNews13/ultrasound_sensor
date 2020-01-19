/*
 * defines_global.h
 *
 * Created: 26.12.2019 10:12:10
 *  Author: bad_n
 */ 


#ifndef DEFINES_GLOBAL_H_
#define DEFINES_GLOBAL_H_


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

#define sbit(var,bit)	do{var |= (1<<bit);}while(0)			// установить бит
#define cbit(var,bit)	do{var &= ~(1<<bit);}while(0)			// очистить бит
#define tbit(var,bit)	do{var ^= (1<<bit);}while(0)			// переключить бит
#ifndef bit_is_set
#define bit_is_set(var,bit)	(var & (1<<bit))
#endif

#ifndef bit_is_clr
#define bit_is_clr(var,bit) (!(bit_is_set(var,bit)))
#endif

//********”правление таймером таймаута UART************
#define UART_TIMEOUT_START()	do{TCNT0 = 0;TCCR0B=(1<<CS02)|(1<<CS00);}while(0)
#define UART_TIMEOUT_STOP()		do{TCCR0B = 0;}while(0)
#define UART_TIMEOUT_CLR()		do{TCNT0 = 0;}while(0)
//********”правление таймером таймаута UART************

/*
//глобальные флаги UART
#define GFU_UART_BYTE_RCV_FLG 0
#define GFU_ACTIVATOR_IS_ACTIVE_FLG 1
#define GFU_RS485_CMD_RCV_FLG 2
#define GFU_UART_SEND_WORKING 3			
#define UART_START_BYTE_ARRIVED 4				// получен стартовый байт
#define UART_OUR_PACKAGE 5						// прием данных разрешен

#define UART_TX_DDR	DDRD
#define UART_TX_PORT PORTD
#define UART_TX_PIN PIND
#define UART_TX_PIN_NUM	PORTD1
#define UART_RX_DDR DDRD
#define UART_RX_PORT PORTD
#define UART_RX_PIN PIND
#define UART_RX_PIN_NUM	PORTD0


/*
//информаци€ о пакете
#define CMD_BYTES_DATALEN 0				// длина пакета
#define CMD_BYTES_RECEIVER 1			// получатель
#define CMD_BYTES_SENDER 2				// отправитель
#define CMD_BYTES_FLAGS 3				// флаги
#define CMD_BYTES_DATA_OFFSET 4			// 
*/


#define CONST_DISTANCE  3				// рассто€ние с которым сравниваетс€ показание датчика

#define PACKET_START_BYTE 0xAA

#define START_BYTE 0
#define ID_RECIPIENT 1
#define ID_SENDER 2
#define LEN 3
#define CMD 4
#define RES 4
#define DATA_OFFSET 5
//#define CRC LEN

//****команды в датчик***
#define CHECK_STATE_PLACE 0x04


#endif /* DEFINES_GLOBAL_H_ */