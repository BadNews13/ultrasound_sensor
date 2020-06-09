/*
 * timer0.h
 *
 * Created: 30.10.2019 17:32:38
 *  Author: PC-013
 */ 


#ifndef TIMER0_H_
#define TIMER0_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define PWM 0x01		//	режим таймера для генерации ультразвуковой волны
#define CTC 0x02		//	режим таймера для отсчета времени
#define TIMER_0_STOP		do{TCCR0B = (0<<CS02)|(0<<CS01)|(0<<CS00); }while(0)

void Timer0_ini_PWM(void);
void Timer0_ini_CTC(void);

int get_tik(void);

#endif /* TIMER0_H_ */