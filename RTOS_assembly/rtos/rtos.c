
#include "../../defines/defines_global.h"
#include "../timer_2/timer_2.h"
#include "rtos.h"
#include <util/delay.h>


//*********************************** ���������� ������ ***********************************
volatile static task TaskArray[MAX_TASKS];      // ������� �����
volatile static u08 arrayTail;                  // "�����" �������

extern volatile uint32_t uptime_cnt;
extern volatile uint32_t cfgtime_cnt;
//*****************************************************************************************


//************************* ������������� ����, ����� ���� - 1 �� *************************
inline void RTOS_Init()
{
	Timer_2_ini();
	TIMER_2_START;
	arrayTail = 0;	// "�����" � 0
}
//*****************************************************************************************


//****************************** ���������� ������ � ������ *******************************
void RTOS_SetTask (void (*taskFunc)(void), u16 taskDelay, u16 taskPeriod)
{
   u08 i;
   
   if(!taskFunc) return;
   for(i = 0; i < arrayTail; i++)                     // ����� ������ � ������� ������
   {
      if(TaskArray[i].pFunc == taskFunc)              // ���� �����, �� ��������� ����������
      {
         DISABLE_INTERRUPT;

         TaskArray[i].delay  = taskDelay;
         TaskArray[i].period = taskPeriod;
         TaskArray[i].run    = 0;   

         RESTORE_INTERRUPT;
         return;                                      // �������, �������
      }
   }

   if (arrayTail < MAX_TASKS)                         // ���� ����� ������ � ������ ��� 
   {                                                  // � ���� �����,�� ���������
      DISABLE_INTERRUPT;
      
      TaskArray[arrayTail].pFunc  = taskFunc;
      TaskArray[arrayTail].delay  = taskDelay;
      TaskArray[arrayTail].period = taskPeriod;
      TaskArray[arrayTail].run    = 0;   

      arrayTail++;                                    // ����������� "�����"
      RESTORE_INTERRUPT;
   }
}
//*****************************************************************************************


//******************************* �������� ������ �� ������ *******************************
void RTOS_DeleteTask (void (*taskFunc)(void))
{
   u08 i;
   
   for (i=0; i<arrayTail; i++)                        // �������� �� ������ �����
   {
      if(TaskArray[i].pFunc == taskFunc)              // ���� ������ � ������ �������
      {
         
         DISABLE_INTERRUPT;
         if(i != (arrayTail - 1))                     // ��������� ��������� ������
         {                                            // �� ����� ���������
            TaskArray[i] = TaskArray[arrayTail - 1];
         }
         arrayTail--;                                 // ��������� ��������� "������"
         RESTORE_INTERRUPT;
         return;
      }
   }
}
//*****************************************************************************************


//*************************** ��������� ����, ���������� � main ***************************
void RTOS_DispatchTask()
{
   u08 i;
   void (*function) (void);

   //dac_set(DISPATCH);
   for (i=0; i<arrayTail; i++)                        // �������� �� ������ �����
   {
      if (TaskArray[i].run == 1)                      // ���� ���� �� ���������� �������,
      {                                               // ���������� ������, �.�. ��
         function = TaskArray[i].pFunc;               // ����� ���������� ����� 
                                                      // ���������� ������
         if(TaskArray[i].period == 0)                 
         {                                            // ���� ������ ����� 0
            RTOS_DeleteTask(TaskArray[i].pFunc);      // ������� ������ �� ������,
         }
         else
         {
            TaskArray[i].run = 0;                     // ����� ������� ���� �������
            if(!TaskArray[i].delay)                   // ���� ������ �� �������� ��������
            {                                         // ������ ��
               TaskArray[i].period = TaskArray[i].period-1; 
            }                                         // ������ ��� ���� ����� ������� �����  
         }
         (*function)();                               // ��������� ������
      }
   }
}
//*****************************************************************************************


//**************** ��������� ������ RTOS (���������� ����������� �������) *****************
void RTOS_ISR_INT(void)
{
	static uint16_t subseccnt=1;
	subseccnt++;
	if(subseccnt==10)
	{
		subseccnt = 0;
		for (u08 i=0; i<arrayTail; i++)			// �������� �� ������ �����
		{
			if  (TaskArray[i].delay == 0)	{TaskArray[i].run = 1;}		// ���� ����� �� ���������� �������	// ������� ���� �������,
			else							{TaskArray[i].delay--;}		// ����� ��������� �����
		}
		RTOS_DispatchTask();
	}
}




/*
void RTOS_WDT_ISR_CALL()
{
	static uint16_t subseccnt=0;

	u08 i;
	
	for (i=0; i<arrayTail; i++)			// �������� �� ������ �����
	{
		if  (TaskArray[i].delay == 0)	// ���� ����� �� ���������� �������
		TaskArray[i].run = 1;			// ������� ���� �������,
		else TaskArray[i].delay--;		// ����� ��������� �����
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

