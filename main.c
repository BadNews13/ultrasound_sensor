#include "defines/defines_global.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "uart/uart.h"
#include "sensor/measurement/measurement.h"
#include "interrupt/interrupt.h"
#include "uart_dec_numbers/uart_dec_numbers.h"
#include "shell/cmd_interp.h"
#include "RTOS_assembly/rtos/rtos.h"
#include "my_name/get_name/get_name.h"

uint8_t adress = 0;				//	�������� ����� ����� ���������� (��� ��������� (��� ������ ��������) ����� 0)

#ifdef MIRF_ENABLED

	#include "mirf/mirf.h"
	#include "spi/spi.h"

	uint8_t message[5] = {0x01, 0x02, 0x03, 0x04, 0x05};		//	�������� ���������
	void mirf_full_reset_to_rx(void)
	{
		mirf_clear();
		mirf_rx_clear();
		MIRF_SET_PD();
		MIRF_SET_RX();
		MIRF_CE_HI();
	}

#endif


void port_ini(void)
{
	// ************** NRF24L01 **************
	//	CE								PB1		->	PD7
	//	CSN								PB0
	//	SCK								PB5
	//	MOSI							PB3
	//	MISO							PB4
	//	IRQ								PD3			(INT1)
	
	// **************** SENSOR *************** 
	//	SENSOR_RETURN					PD2			(INT0)
	//	SENSOR_PULSES					PD6			(OC0A)
	
	// **************** RS485 ****************
	//	COTROL_DIR						PD4
	//	Hard_uart_TX					PD1
	//	Hard_uart_RX					PD0
	
	// ************ BAUD_RATE_DIP ************
	//	dipBaudRate						PC3
	//	dipBaudRate						PC2
	
	// *********** LED (RED/GREEN) ***********
	//	LED_GREEN						PC5
	//	LED_RED							PC4
	
	// ************** LED_FILM *************** 
	//	LED_FILM_GREEN					PC1
	//	LED_FILM_RED					PC0
	
	// ************ PACK_IN_LED **************
	//	PACK_IN_LED						PD5
	
	// ************* RES_EEPROM ************** 
	//	RES_EEPROM						PD7
	
	// ***************** ADC *****************
	//	ADC								PC6
	
	//��������� �����
	DDRB	=	(0<<PINB7) |		//	�����
				(1<<PINB6) |		//	�����
				(1<<PINB5) |		//	SCK
				(0<<PINB4) |		//	MISO
				(1<<PINB3) |		//	MOSI
				(1<<PINB2) |		//	SS (�� ����������, �� �� ������ ������ �������� � +)
				(1<<PINB1) |		//	CE (�� ������������)									(nRF24L01)	!
				(1<<PINB0);			//	CSN (nRF24L01)
	
	PORTB	=	(0<<PINB7) |		//	�����
				(0<<PINB6) |		//	�����
				(0<<PINB5) |		//	SCK
				(0<<PINB4) |		//	MISO
				(0<<PINB3) |		//	MOSI
				(1<<PINB2) |		//	SS (�� ����������, �� �� ������ ������ �������� � +)
				(0<<PINB1) |		//	CE	(NRF24L01 ������� �� �������� ������)	��� �������� � ����� ������������� - ��������� � 1
				(1<<PINB0);			//	CSN	(NRF24L01 ������� �� ������� ������)	��� ������ ������ �� SPI - ��������� � 0


	DDRC	=	(0<<PINC6) |		//	ADC
				(1<<PINC5) |		//	LED_GREEN
				(1<<PINC4) |		//	LED_RED		
				(0<<PINC3) |		//	dipBaudRate bit_1
				(0<<PINC2) |		//	dipBaudRate bit_0
				(1<<PINC1) |		//	LED_FILM_GREEN
				(1<<PINC0);			//	LED_FILM_RED
	
	PORTC	=	(0<<PINC6) |		//	ADC
				(0<<PINC5) |		//	LED_GREEN
				(0<<PINC4) |		//	LED_RED
				(0<<PINC3) |		//	dipBaudRate bit_1
				(0<<PINC2) |		//	dipBaudRate bit_0
				(0<<PINC1) |		//	LED_FILM_GREEN
				(0<<PINC0);			//	LED_FILM_RED


	DDRD	=	(1<<PIND7) |		//	RES_EEPROM
				(1<<PIND6) |		//	SENSOR_PULSES
				(1<<PIND5) |		//	PACK_IN_LED
				(1<<PIND4) |		//	COTROL_DIR (RS485)
				(0<<PIND3) |		//	IRQ				(INT1)
				(0<<PIND2) |		//	SENSOR_RETURN	(INT0)
				(1<<PIND1) |		//	Hard_uart_TX
				(0<<PIND0);			//	Hard_uart_RX
	
	PORTD	=	(0<<PIND7) |		//	RES_EEPROM
				(0<<PIND6) |		//	SENSOR_PULSES
				(0<<PIND5) |		//	PACK_IN_LED
				(0<<PIND4) |		//	COTROL_DIR (RS485)
				(1<<PIND3) |		//	IRQ				(INT1)	(NRF24L01 ������� �� ������� ������)	���� ������ ����� �� �����, �� NRF24L01 ������� ��� ���� � 0
				(1<<PIND2) |		//	SENSOR_RETURN	(INT0)
				(1<<PIND1) |		//	Hard_uart_TX	(��������� � �����)
				(1<<PIND0);			//	Hard_uart_RX	(��������� � �����)
}

int main(void)
{
	adress = get_main_adress();
	if (adress == 0x00)	{Timer_1_ini();}		// ���� ����� �������, �� �������� ������ ��� ���������� ��������

	port_ini();					//	�����
	uart_ini(48,adress);		//	������������� USART BaudRate_4800
	RTOS_Init();				//	�������� ��
	int_ini();					//	����� ��� ��������� ����� � ��� mirf
	
	#ifdef MIRF_ENABLED
		spi_ini();
		uint8_t self_adr[3] =		{0x00, 0x00, 0x02};		//	����� ����� ����������	(������ �15)
		uint8_t remote_adr[3] =		{0x00, 0x00, 0x02};		//	����� ���������� ���������� (������� ������ 1)
		mirf_init();
		mirf_set_rxaddr(1,self_adr);	//	����� �1 ��������� ����� (��� �����)
		mirf_set_txaddr(remote_adr);	//	������ ����� ����������
		mirf_set_rxaddr(0,remote_adr);	//	���� �������� ACK, �� RX_ADR_P0 ������ ���� ����� �� ��� � ���������� ����������
		MIRF_SET_RX();
	#endif	

	sei();
	put_byte(0x06);
	put_byte(adress);
	
		PORTC &= ~(1<<1);	//	��������� ������� �� ������
		PORTC &= ~(1<<0);	//	��������� ������� �� ������
	
	blink();

	RTOS_SetTask(parsing_uart_RX, 2, 0);	//	���������� ���������� �� hard_uart ������, ���� ��� ���� (���� ������, �� �� �����)
	while(1)
	{
//		Timer0_ini_PWM();
//		start_measurement();	_delay_ms(200);

	}
}
