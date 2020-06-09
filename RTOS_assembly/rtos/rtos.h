#ifndef RTOS_H
#define RTOS_H

/******************************************************************************************
 * �� ������ ���� ����������� ����� � ����� ChipEnable.ru                                 *
 * http://chipenable.ru/index.php/programming-avr/item/110-planirovschik.html             *
 *                                                                                        *
 * ��������� ������� �������� aka KontAr                                                  *
 * ����: 26.03.2014                                                                       *
 *                                                                                        *
 * ���������:                                                                             *
 * - �������� ����������� ����� ������                                                    *
 * - ��������� �������� ������ �� �����                                                   *
 * - ��� ��������� ���������� ������ ����������� �� ����������                            *
 * - �������� ��������� �� "�����" ������                                                 *
 * - ������� ���� ��������������� � ������ "������"                                       *
 ******************************************************************************************/

#include "commons.h"
#include "../timer_2/timer_2.h"

#define MAX_TASKS	16               // ���������� ����� 

//#define RTOS_ISR	TIMER1_COMPA_vect // ������ ����������
/******************************************************************************************
 * ��������� ������
 */

typedef struct task
{   
   void (*pFunc) (void);               // ��������� �� �������
   u16 delay;                          // �������� ����� ������ �������� ������
   u16 period;                         // ������� ��� ��������� ������ (��������������� ����� ��� ��������)
   u08 run;                            // ���� ���������� ������ � �������
}task;

/******************************************************************************************
 * ��������� �������
 */
void RTOS_Init (void);
void RTOS_ISR_INT(void);
void RTOS_SetTask (void (*taskFunc)(void), u16 taskDelay, u16 taskPeriod);

void RTOS_DeleteTask (void (*taskFunc)(void));
void RTOS_DispatchTask (void);

void RTOS_WDT_ISR_CALL(void);

uint16_t RTOS_calc_timeout_by_prescaler(uint16_t time_delay);

#endif
