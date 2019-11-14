
#include "measurement.h"
#include <avr/io.h>
//#include "interrupt0/interrupt0.h"


#define max_delay_interrupt 6000		//	максимальное время ожидания прерывания (60000 - это 10 метров)

int step = 0;							//	шаг функции измерения
int distance = 0;						//	расстояние
int out_pulse_count = 0;				//	отправленные импульсы
int interrupt_count = 0;				//	количество внешних прерываний
float time_count = 0;					//	время из таймера
float time_interrupt = 0;				//	время срабатывания прерывания



void measurement(void)
{
	switch (step)
	{
		
		case 0:							//	0-й шаг: НАЧАЛО
		{
			distance = 0;				//	обнуляем предыдущее значение
			out_pulse_count = 0;		//	обнуляем предыдущее значение
			interrupt_count = 0;		//	обнуляем предыдущее значение
			time_count = 0;				//	обнуляем предыдущее значение		
			Timer0_ini_PWM();			//	инициализируем таймер, в режим переключения пина OC0A
			step = 1;					//	переход к следующему шагу при повторном вызове
			
			return;
		}
		
		case 1:							//	1-й шаг: ОТПРАВКА ИМПУЛЬСОВ И ОТСЧЕТ ВРЕМЕНИ
		{
			out_pulse_count++;			//	счетчик будет увеличиваться на 1 каждое прерывание таймер (4,9 микросекунд)
			
			if (out_pulse_count==39)	//	ждем 10 импульсов
			{
				Timer0_stop();			//	останавливаем таймер, который переключает пин OC0A
				PORTD = (0<<PIND6);		//	пин OC0B вниз (что бы пин не остался в 1 при четном/нечетном количестве переключений)
				Timer0_ini_CTC();		//	инициализируем таймер в режим отсчета времени
				Int0_ini_down();		//	инициализируем внешнее прерывание
				step = 2;				//	переход к следующему шагу при повторном вызове
			}
			return;
		}
		
		case 2:
		{
			time_count++;				//	считаем время
			
			// если разница в прерываниях больше допустимой, то считаем его последним и выводим дистанцию
			if (time_count - time_interrupt > max_delay_interrupt)			
			{
				distance = ( ( 340 * time_interrupt ) / 2 ) / 10000;		//	Дистанция получится в сантиметрах		/10000
				write_float(time_interrupt);								//	заменить на вывод дистанции
				step = 4;
			}
			
			return;
		}
		
		case 3:								//	2-й шаг: ОЖИДАНИЕ ПРЕРЫВАНИЯ
		{
			time_interrupt = time_count;	//	запомнить время прерывания
			change_interrup();				//	переключить прерывание 
			return;
		}
		
		case 4:							//	3-й шаг: ДЕЙСТВИЕ ПРИ ОШИБКИ ИЛИ ЗАВЕРШЕНИЕ
		{
			Timer0_stop();				//	останавливаем таймер, который переключает пин OC0A
			Int0_disabled();			//	отключаем внешнее прерывание
			step = 0;					//	ставим флаг - 3-й шаг выполнен, переход к 0-ому шагу в следующем вызове функции
			return;
		}
	}
}


int return_step_measurement()
{
	return step;
}

void set_step_3(void)
{
	step = 3;
}

void set_step_2(void)
{
	step = 2;
}

