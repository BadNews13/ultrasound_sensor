/*
 * timer_1.h
 *
 * Created: 12.05.2020 20:57:14
 *  Author: bad_n
 */ 


#ifndef TIMER_1_H_
#define TIMER_1_H_

void Timer_1_ini(void);

//********Управление таймером************
#define TIMER_2_START		do{TCNT1 = 0; TCCR2B = (0<<CS12)|(0<<CS11)|(1<<CS10);}while(0)		//	start with no prescaler
#define TIMER_2_STOP		do{TCCR1B = (0<<CS12)|(0<<CS11)|(0<<CS10); }while(0)
#define TIMER_2_CLR			do{TCNT1L = 0; TCNT1H = 0;}while(0)
//********Управление таймером************


#endif /* TIMER_1_H_ */