/*
 * interrupt.h
 *
 * Created: 25.02.2020 16:06:26
 *  Author: bad_n
 */ 


#ifndef INTERRUPT_H_
#define INTERRUPT_H_

#define INT_1_DISABLE	do{EIMSK=(0<<INT1);}while(0)		// запретить прерывание на INT1	(PD3)
#define INT_1_ENABLE	do{EIMSK=(1<<INT1);}while(0)		// разрешить прерывание на INT1	(PD3)

void int_ini(void);



#endif /* INTERRUPT_H_ */