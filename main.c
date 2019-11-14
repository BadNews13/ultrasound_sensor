#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "usart/usart.h"
#include "timer0/timer0.h"
//#include "comparator/comparator.h"
#include "measurement/measurement.h"
#include "interrupt0/interrupt0.h"

int start_measurement = 0;
int button_state = 0;

void PORT_ini(void)
{
	DDRD |= (0<<PIND4);		// кнопка для запуска измерения
//	DDRD |= (1<<PIND5);		// pin OC0B для посыла 10 импульсов (в излучатель)
	DDRD |= (1<<PIND5);		
	DDRD |= (1<<PIND6);		// pin OC0A для посыла 10 импульсов (в излучатель)

}

int main(void)
{
	PORT_ini();				//	порты
	USART_ini();			//	инициализация USART

sei();

	while(1)
	{	
		
//		putch_usart(0x04);
//		Timer0_ini_PWM();
//		_delay_ms(100);
		
		
															if   (PIND & (1<<PIND4))
															{
																if (return_step_measurement() == 0)
																{
																	measurement();		//	идем в шаг 0
																}

															}
		
		if   (getch_usart()==0x04)
		{
			if (return_step_measurement() == 0)
			{
				measurement();		//	идем в шаг 0
			}

		}
	}
}
