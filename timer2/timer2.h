/*
 * timer2.h
 *
 * Created: 30.10.2019 17:32:38
 *  Author: PC-013
 */ 


#ifndef TIMER2_H_
#define TIMER2_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void Timer2_ini_PWM(void);
void Timer2_stop(void);
int Timer2_get_us(void);

void pulses2_for_test(void);

#endif /* TIMER2_H_ */