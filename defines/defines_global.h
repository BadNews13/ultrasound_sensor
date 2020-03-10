/*
 * defines_global.h
 *
 * Created: 26.12.2019 10:12:10
 *  Author: bad_n
 */ 


#ifndef DEFINES_GLOBAL_H_
#define DEFINES_GLOBAL_H_

#define  F_CPU 16000000UL


//********** chois communication (can't work together) **********
//#define MIRF_ENABLED
#ifndef MIRF_ENABLED
//	#define MIRF_Master			//	set MASTER or SLAVE
#endif
//***************************************************************

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


#define HALL_PWRON()	do{sbit(PORTC,4);}while(0)
#define HALL_PWROFF()	do{cbit(PORTC,4);}while(0)
#define HALL_ADC_INPUT	0
#define ADC_VREF_TYPE	((0<<REFS1) | (1<<REFS0) | (0<<ADLAR))
#define ADC_ON()		do{sbit(ADCSRA,ADEN);}while(0)
#define ADC_OFF()		do{cbit(ADCSRA,ADEN);}while(0)



#endif /* DEFINES_GLOBAL_H_ */