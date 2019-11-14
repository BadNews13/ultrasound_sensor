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

void Timer0_ini_PWM(void);
void Timer0_ini_CTC(void);
void Timer0_stop(void);
int Timer0_get_us(void);

#endif /* TIMER0_H_ */