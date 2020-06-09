#ifndef RTOS_H
#define RTOS_H

/******************************************************************************************
 * За основу взят планировщик задач с сайта ChipEnable.ru                                 *
 * http://chipenable.ru/index.php/programming-avr/item/110-planirovschik.html             *
 *                                                                                        *
 * Доработал Шибанов Владимир aka KontAr                                                  *
 * Дата: 26.03.2014                                                                       *
 *                                                                                        *
 * Изменения:                                                                             *
 * - добавлен однократный вызов задачи                                                    *
 * - добавлено удаление задачи по имени                                                   *
 * - при повторном добавлении задачи обновляются ее переменные                            *
 * - добавлен указатель на "хвост" списка                                                 *
 * - функции РТОС скорректированы с учетом "хвоста"                                       *
 ******************************************************************************************/

#include "commons.h"
#include "../timer_2/timer_2.h"

#define MAX_TASKS	16               // Количество задач 

//#define RTOS_ISR	TIMER1_COMPA_vect // Вектор прерывания
/******************************************************************************************
 * Структура задачи
 */

typedef struct task
{   
   void (*pFunc) (void);               // указатель на функцию
   u16 delay;                          // задержка перед первым запуском задачи
   u16 period;                         // сколько раз запустить задачу (перезапускаться будет бет задержки)
   u08 run;                            // флаг готовности задачи к запуску
}task;

/******************************************************************************************
 * Прототипы фукнций
 */
void RTOS_Init (void);
void RTOS_ISR_INT(void);
void RTOS_SetTask (void (*taskFunc)(void), u16 taskDelay, u16 taskPeriod);

void RTOS_DeleteTask (void (*taskFunc)(void));
void RTOS_DispatchTask (void);

void RTOS_WDT_ISR_CALL(void);

uint16_t RTOS_calc_timeout_by_prescaler(uint16_t time_delay);

#endif
