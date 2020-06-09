
#include "../../defines/defines_global.h"
#include "../timer_2/timer_2.h"
#include "rtos.h"
#include <util/delay.h>


//*********************************** Переменные модуля ***********************************
volatile static task TaskArray[MAX_TASKS];      // очередь задач
volatile static u08 arrayTail;                  // "хвост" очереди

extern volatile uint32_t uptime_cnt;
extern volatile uint32_t cfgtime_cnt;
//*****************************************************************************************


//************************* Инициализация РТОС, время тика - 1 мс *************************
inline void RTOS_Init()
{
	Timer_2_ini();
	TIMER_2_START;
	arrayTail = 0;	// "хвост" в 0
}
//*****************************************************************************************


//****************************** Добавление задачи в список *******************************
void RTOS_SetTask (void (*taskFunc)(void), u16 taskDelay, u16 taskPeriod)
{
   u08 i;
   
   if(!taskFunc) return;
   for(i = 0; i < arrayTail; i++)                     // поиск задачи в текущем списке
   {
      if(TaskArray[i].pFunc == taskFunc)              // если нашли, то обновляем переменные
      {
         DISABLE_INTERRUPT;

         TaskArray[i].delay  = taskDelay;
         TaskArray[i].period = taskPeriod;
         TaskArray[i].run    = 0;   

         RESTORE_INTERRUPT;
         return;                                      // обновив, выходим
      }
   }

   if (arrayTail < MAX_TASKS)                         // если такой задачи в списке нет 
   {                                                  // и есть место,то добавляем
      DISABLE_INTERRUPT;
      
      TaskArray[arrayTail].pFunc  = taskFunc;
      TaskArray[arrayTail].delay  = taskDelay;
      TaskArray[arrayTail].period = taskPeriod;
      TaskArray[arrayTail].run    = 0;   

      arrayTail++;                                    // увеличиваем "хвост"
      RESTORE_INTERRUPT;
   }
}
//*****************************************************************************************


//******************************* Удаление задачи из списка *******************************
void RTOS_DeleteTask (void (*taskFunc)(void))
{
   u08 i;
   
   for (i=0; i<arrayTail; i++)                        // проходим по списку задач
   {
      if(TaskArray[i].pFunc == taskFunc)              // если задача в списке найдена
      {
         
         DISABLE_INTERRUPT;
         if(i != (arrayTail - 1))                     // переносим последнюю задачу
         {                                            // на место удаляемой
            TaskArray[i] = TaskArray[arrayTail - 1];
         }
         arrayTail--;                                 // уменьшаем указатель "хвоста"
         RESTORE_INTERRUPT;
         return;
      }
   }
}
//*****************************************************************************************


//*************************** Диспетчер РТОС, вызывается в main ***************************
void RTOS_DispatchTask()
{
   u08 i;
   void (*function) (void);

   //dac_set(DISPATCH);
   for (i=0; i<arrayTail; i++)                        // проходим по списку задач
   {
      if (TaskArray[i].run == 1)                      // если флаг на выполнение взведен,
      {                                               // запоминаем задачу, т.к. во
         function = TaskArray[i].pFunc;               // время выполнения может 
                                                      // измениться индекс
         if(TaskArray[i].period == 0)                 
         {                                            // если период равен 0
            RTOS_DeleteTask(TaskArray[i].pFunc);      // удаляем задачу из списка,
         }
         else
         {
            TaskArray[i].run = 0;                     // иначе снимаем флаг запуска
            if(!TaskArray[i].delay)                   // если задача не изменила задержку
            {                                         // задаем ее
               TaskArray[i].period = TaskArray[i].period-1; 
            }                                         // задача для себя может сделать паузу  
         }
         (*function)();                               // выполняем задачу
      }
   }
}
//*****************************************************************************************


//**************** Таймерная служба RTOS (прерывание аппаратного таймера) *****************
void RTOS_ISR_INT(void)
{
	static uint16_t subseccnt=1;
	subseccnt++;
	if(subseccnt==100)
	{
		subseccnt = 0;
		for (u08 i=0; i<arrayTail; i++)			// проходим по списку задач
		{
			if  (TaskArray[i].delay == 0)	{TaskArray[i].run = 1;}		// если время до выполнения истекло	// взводим флаг запуска,
			else							{TaskArray[i].delay--;}		// иначе уменьшаем время
		}
		RTOS_DispatchTask();
	}
}




/*
void RTOS_WDT_ISR_CALL()
{
	static uint16_t subseccnt=0;

	u08 i;
	
	for (i=0; i<arrayTail; i++)			// проходим по списку задач
	{
		if  (TaskArray[i].delay == 0)	// если время до выполнения истекло
		TaskArray[i].run = 1;			// взводим флаг запуска,
		else TaskArray[i].delay--;		// иначе уменьшаем время
	}
	if(subseccnt++==1000)
	{
		uptime_cnt++;
		cfgtime_cnt++;
		subseccnt = 0;
	}
	timeout_handler_task();
}
*/

/*
uint16_t RTOS_calc_timeout_by_prescaler(uint16_t time_delay)
{
	uint16_t val=0;
	val=((time_delay*2)+(time_delay*2)/10);
	val=val+val/2;
//	val=time_delay*3;
//	val=val/17;
//	val=val*10;
//	val=((time_delay*40)/5)*100;
	if (val > 0)	{val=val-1;}
	if (val <= 0)	{val=1;}
		
	return val;
}
*/

