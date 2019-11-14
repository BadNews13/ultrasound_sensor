 
#ifndef USART_H_
#define USART_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define F_CPU 1000000UL		//� �������� �� ��������� �� �������� �� 1 ���
//#define F_CPU 16000000UL		
#define BAUD 4800L // �������� ������ �������
#define UBRRL_value (F_CPU/(BAUD*16))-1

void USART_ini(void);
uint8_t getch_usart();
void putch_usart(int tx_data);
void write_float(float f);

#endif /* USART_H_ */

