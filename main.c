#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "uart/uart.h"
#include "timer2/timer2.h"
#include "measurement/measurement.h"
#include "interrupt/interrupt.h"
#include "uart_dec_numbers/uart_dec_numbers.h"
#include "defines/defines_global.h"
#include "shell/cmd_interp.h"

void read_dip(void);
uint8_t dipBaudRate;							// скорость полученная с dip переключателей
uint8_t dipAdress;								// адрес платы полученный с dip переключателей

int start_measurement = 0;
int button_state = 0;
char temp_data = 0;

void PORT_ini(void)
{
	DDRD |= (1<<PIND4);		//	sensor_pulses (отправка импульсов)
	DDRD |= (0<<PIND2);		//	sensor_return (приме ответного сигнала)
	DDRB |= (1<<PINB1)|(1<<PINB2);		//	светодиод над парковочным местом

//	DDRD |= (1<<PIND5);		// pin OC0B для посыла 10 импульсов (в излучатель)
	DDRD |= (1<<PIND5);		
	DDRD |= (1<<PIND6);		// pin OC0A для посыла 10 импульсов (в излучатель)

	DDRB = 0xff;
}

int main(void)
{
	PORT_ini();				//	порты
	uart_ini(48, 13);		//	инициализация USART BaudRate_4800 Adress_3
	read_dip();				//	считать адрес платы и скорость с дип-переключателей

	sei();
	
//put_byte(0xFF);

	send_crc8();

while(1)	
{
//	search_for_my_package();
}


/*	while(1)
	{	
		
//****Светодиод над парковочным местом	
		if (return_distance()>1)		//		если есть адекватное расстояние
		{
			if (return_distance()>=CONST_DISTANCE)		//	если показание датчика больше установленного значения, то место свободно
			{
				PORTB = (1<<PINB1);			//	led_green
				PORTB = (0<<PINB2);			//	led_red
			}		
			else										//	если показание датчика меньше установленного значения, то место занято
			{
				PORTB = (0<<PINB1);			//	led_green
				PORTB = (1<<PINB2);			//	led_red
			}
		}
		
//if   (PIND & (1<<PIND4)){}
		
	}*/
}


void read_dip(void)
{	
	//	bit PC0: tmp_Adress
	//	bit PC1: tmp_Adress
	//	bit PC2: tmp_Adress
	//	bit PC3: tmp_Adress
	//	bit PC4: tmp_Adress
	//	bit PC5: tmp_Adress
	//	bit PD6: BaudRate
	//	bit PD7: BaudRate

	dipAdress = PORTC & 0b00111111;

	uint8_t tmp_Baud = PORTD;
	tmp_Baud = (tmp_Baud >> 6) & 0b00000011;
	switch(tmp_Baud)
	{
		case 0:
		dipBaudRate = 96;
		break;
		case 1:
		dipBaudRate = 12;
		break;
		case 2:
		dipBaudRate = 24;
		break;
		case 3:
		dipBaudRate = 48;
		break;
	}
}