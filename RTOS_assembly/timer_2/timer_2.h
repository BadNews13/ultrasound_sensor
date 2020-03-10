/*
 * timer2.h
 *
 * Created: 30.10.2019 17:32:38
 *  Author: PC-013
 */ 


#ifndef TIMER_2_H_
#define TIMER_2_H_

void Timer_2_ini(void);

//********Управление таймером************
#define TIMER_2_START		do{TCNT2 = 0; TCCR2B = (1<<CS22)|(1<<CS21)|(1<<CS20);}while(0)		//	start with prescaler 1/1024
#define TIMER_2_STOP		do{TCCR2B = (0<<CS22)|(0<<CS21)|(0<<CS20); }while(0)
#define TIMER_2_CLR			do{TCNT2 = 0;}while(0)
//********Управление таймером************

#endif /* TIMER2_H_ */