#include "defines/defines_global.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "uart/uart.h"
#include "measurement/measurement.h"
#include "interrupt/interrupt.h"
#include "uart_dec_numbers/uart_dec_numbers.h"
#include "shell/cmd_interp.h"
#include "RTOS_assembly/rtos/rtos.h"

void read_dip(void);
uint8_t dipBaudRate;							// скорость полученная с dip переключателей
uint8_t dipAdress;								// адрес платы полученный с dip переключателей

uint8_t start_measurement = 0;
uint8_t button_state = 0;


#ifdef MIRF_ENABLED

#include "mirf/mirf.h"
#include "spi/spi.h"

#define RF_POWER_ON()	do{sbit(PORTC,1);}while(0)
#define RF_POWER_OFF()	do{cbit(PORTC,1);}while(0)

#ifdef MIRF_Master
//	Для ардуины отправителя
uint8_t rxaddr_tag[5] = {0xF0, 0xF0, 0xF0, 0xF0, 0xF0};		//	адрес этого устройства
uint8_t txaddr_tag[5] = {0x0F, 0x0F, 0x0F, 0x0F, 0x0F};		//	адрес удаленного устройства
#else
//	Для ардуины получателя
uint8_t rxaddr_tag[5] = {0x0F, 0x0F, 0x0F, 0x0F, 0x0F};		//	адрес этого устройства
uint8_t txaddr_tag[5] = {0xF0, 0xF0, 0xF0, 0xF0, 0xF0};		//	адрес удаленного устройства
#endif

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
	//	CE								PB1
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
	
	//настройка пинов
	DDRB	=	(0<<PINB7) |		//	кварц
				(1<<PINB6) |		//	кварц
				(1<<PINB5) |		//	SCK
				(0<<PINB4) |		//	MISO
				(1<<PINB3) |		//	MOSI
				(1<<PINB2) |		//	SS (не используем, но на всякий случай притянем к +)
				(1<<PINB1) |		//	CE
				(1<<PINB0);			//	CSN
	
	PORTB	=	(0<<PINB7) |		//	кварц
				(0<<PINB6) |		//	кварц
				(0<<PINB5) |		//	SCK
				(0<<PINB4) |		//	MISO
				(0<<PINB3) |		//	MOSI
				(1<<PINB2) |		//	SS (не используем, но на всякий случай притянем к +)
				(0<<PINB1) |		//	CE	(NRF24L01 активна по высокому уровню)	для перехода в режим прослушивания - притянуть к 1
				(1<<PINB0);			//	CSN	(NRF24L01 активна по низкому уровню)	для обмена данным по SPI - притянуть к 0



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
				(0<<PIND6) |		//	SENSOR_PULSES
				(1<<PIND5) |		//	PACK_IN_LED
				(1<<PIND4) |		//	COTROL_DIR (RS485)
				(0<<PIND3) |		//	IRQ				(INT1)
				(0<<PIND2) |		//	SENSOR_RETURN	(INT0)
				(1<<PIND1) |		//	Hard_uart_TX
				(0<<PIND0);			//	Hard_uart_RX
	
	PORTD	=	(0<<PIND7) |		//	RES_EEPROM
				(0<<PIND6) |		//	SENSOR_RETURN
				(0<<PIND5) |		//	PACK_IN_LED
				(0<<PIND4) |		//	COTROL_DIR (RS485)
				(1<<PIND3) |		//	IRQ				(INT1)	(NRF24L01 активна по низкому уровню)	если придет пакет по радио, то NRF24L01 прижмет эту ногу к 0
				(1<<PIND2) |		//	SENSOR_RETURN	(INT0)
				(1<<PIND1) |		//	Hard_uart_TX	(притянуть к плюсу)
				(1<<PIND0);			//	Hard_uart_RX	(притянуть к плюсу)
}

int main(void)
{
	port_ini();				//	порты
	uart_ini(48,15);		//	инициализация USART BaudRate_4800 Adress_15
	read_dip();				//	считать скорость baud_rate с дип-переключателей
	RTOS_Init();
	
	
	#ifdef MIRF_ENABLED
		spi_ini();				//	for mirf

		RF_POWER_ON();//питание передатчика
		_delay_ms(50);
		mirf_init();
		_delay_ms(50);
		sei();

		mirf_config();
		_delay_ms(50);

		//ретранслятор слушает два канала
		mirf_set_rxaddr(0,txaddr_tag);				//	в 0 канал приемника записывается адрес удаленной стороны (для подтверждений)
		mirf_set_rxaddr(1,rxaddr_tag);				//	в 1 канал приемника записывается адрес этого устройства

		//передача возможна только по одному каналу по умолчанию это канал метки
		mirf_set_txaddr(txaddr_tag);

		mirf_write_register(RF_SETUP,6);//1мегабит/с, максимальная мощность

		//для наших целей приемник всегда должен быть включен
		MIRF_SET_RX();
		MIRF_CE_HI();
	
		mirf_write_register(RF_CH, mirf_CH); //set RF channel.
	
		mirf_full_reset_to_rx();
		_delay_ms(50);		// обязательно поставить задержку

		//	проверка, что nRF24L01 правильно работает (после отладки удалить)
		uint8_t check_mirf = mirf_get_status();
		put_byte(check_mirf);

		//creat_and_send_pack_for_test();
		//send_test_pack_to_mirf();
		//send_cicle_byte_to_soft_uart();
		
	#endif
	

	sei();

	RTOS_SetTask(parsing_uart_RX, 2, 0);
	while(1)
	{
	}
}

void read_dip(void)
{	
	//	bit PC3: BaudRate
	//	bit PC2: BaudRate

	uint8_t tmp_Baud = PORTC;
	tmp_Baud = (tmp_Baud >> 2) & 0b00000011;
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