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



#endif /* DEFINES_GLOBAL_H_ */