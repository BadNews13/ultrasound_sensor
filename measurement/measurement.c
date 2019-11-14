
#include "measurement.h"
#include <avr/io.h>
//#include "interrupt0/interrupt0.h"


#define max_delay_interrupt 6000			//	максимальное время ожидания прерывания (60000 - это 10 метров)

int step = 0;								//	шаг функции измерения
int distance = 0;							//	расстояние
int out_pulse_count = 0;					//	отправленные импульсы
int interrupt_count = 0;					//	количество внешних прерываний

int time_count = 0;						//	время из таймера
int time_interrupt = 0;					//	время срабатывания прерывания

//float time_count = 0;						//	время из таймера
//float time_interrupt = 0;					//	время срабатывания прерывания

	char uart_byte_1 = 0;											
	char uart_byte_2 = 0;					
	char uart_byte_3 = 0;
	char uart_byte_4 = 0;


void measurement(void)
{
	switch (step)
	{
		
		case 0:								//	0-й шаг: НАЧАЛО
		{
			distance = 0;					//	обнуляем предыдущее значение
			out_pulse_count = 0;			//	обнуляем предыдущее значение
			interrupt_count = 0;			//	обнуляем предыдущее значение
			time_count = 0;					//	обнуляем предыдущее значение		
			Timer0_ini_PWM();				//	инициализируем таймер, в режим переключения пина OC0A
			step = 1;						//	переход к следующему шагу при повторном вызове
			
			return;
		}
		
		case 1:								//	1-й шаг: ОТПРАВКА ИМПУЛЬСОВ И ОТСЧЕТ ВРЕМЕНИ
		{
			out_pulse_count++;				//	счетчик будет увеличиваться на 1 каждое прерывание таймер (4,9 микросекунд)
			
			if (out_pulse_count==39)		//	ждем 10 импульсов
			{
				Timer0_stop();				//	останавливаем таймер, который переключает пин OC0A
				PORTD = (0<<PIND6);			//	пин OC0B вниз (что бы пин не остался в 1 при четном/нечетном количестве переключений)
				Timer0_ini_CTC();			//	инициализируем таймер в режим отсчета времени
				Int0_ini();					//	инициализируем внешнее прерывание
				step = 2;					//	переход к следующему шагу при повторном вызове
			}
			return;
		}
		
		case 2:
		{
			time_count++;					//	считаем время
			
			// если разница в прерываниях больше допустимой, то считаем его последним и выводим дистанцию
			if (time_count - time_interrupt > max_delay_interrupt)			
			{
				distance = ( ( 340 * time_interrupt ) / 2 ) / 10000;		//	[ дистанция = (скорость ультразвука * время) / 2 ]. Дистанция получится в сантиметрах если /10000
//				write_float(time_interrupt);								//	заменить на вывод дистанции

				change_data_type_for_uart(time_interrupt);				// разбить число на 4 байта
				putch_usart(uart_byte_1);
				putch_usart(uart_byte_2);
				putch_usart(uart_byte_3);
				putch_usart(uart_byte_4);
								
				change_data_type_for_uart(distance);				// разбить число на 4 байта
				putch_usart(uart_byte_1);
				putch_usart(uart_byte_2);
				putch_usart(uart_byte_3);
				putch_usart(uart_byte_4);
				step = 4;
				
							Timer0_stop();					//	останавливаем таймер, который переключает пин OC0A
							Int0_disabled();				//	отключаем внешнее прерывание
							step = 0;						//	ставим флаг - 3-й шаг выполнен, переход к 0-ому шагу в следующем вызове функции

			}
			return;
		}
		
		case 3:								//	2-й шаг: ОЖИДАНИЕ ПРЕРЫВАНИЯ
		{
			time_interrupt = time_count;	//	запомнить время прерывания	
				
			//	если прерывания не прекрщаются
			interrupt_count++;
			if (interrupt_count>10)
			{
				step = 4;
			}
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

void change_data_type_for_uart(int data)
{
	// пример 12 345 678;
	
	uart_byte_1 = data / 1000000;	//	12												//	123456/10000		=12
	uart_byte_2 = (data - uart_byte_1 * 1000000) / 10000;	//	34							//	(123456-(123456/10000)*10000)/100 = (123456-120000)/100 = 3456/100 = 34				
	uart_byte_3 = (data - uart_byte_1 * 1000000 - uart_byte_2 * 10000) / 100;			//	56
	uart_byte_4 = (data - uart_byte_1 * 1000000 - uart_byte_2 * 10000 - uart_byte_3 * 100);		//78

//			12345678    -     12      * 1000000 -      34     * 10000 -     56      * 100 )
}