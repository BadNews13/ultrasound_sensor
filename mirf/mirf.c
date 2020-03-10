
#include <util/delay.h>
#include <avr/io.h>
#include "../defines/defines_global.h"
#include "mirf.h"
#include "nRF24L01.h"
#include "../spi/spi.h"



//initialize mirf
void mirf_init(void) 
{
	MIRF_CE_LO();
	MIRF_CSN_HI();
}

//config mirf
void mirf_config(void) 
{
    mirf_write_register(RF_CH, mirf_CH); //set RF channel
    mirf_write_register(RX_PW_P0, mirf_PAYLOAD); //length of incoming payload
    mirf_write_register(RX_PW_P1, mirf_PAYLOAD);
    mirf_write_register(RX_PW_P2, mirf_PAYLOAD);
    mirf_write_register(RX_PW_P3, mirf_PAYLOAD);
    mirf_write_register(RX_PW_P4, mirf_PAYLOAD);
    mirf_write_register(RX_PW_P5, mirf_PAYLOAD);
    mirf_write_register(SETUP_RETR, mirf_RETR); // set retries
	#if mirf_ACK == 1  //auto acknowledgement
    	mirf_write_register(EN_AA, 0);
		#if mirf_ENABLED_P0 == 1
    		mirf_write_register(EN_AA, mirf_read_register(EN_AA) | (1<<ENAA_P0));
		#endif
		#if mirf_ENABLED_P1 == 1
    		mirf_write_register(EN_AA, mirf_read_register(EN_AA) | (1<<ENAA_P1));
		#endif
		#if mirf_ENABLED_P2 == 1
			mirf_write_register(EN_AA, mirf_read_register(EN_AA) | (1<<ENAA_P2));
		#endif
		#if mirf_ENABLED_P3 == 1
			mirf_write_register(EN_AA, mirf_read_register(EN_AA) | (1<<ENAA_P3));
		#endif
		#if mirf_ENABLED_P4 == 1
			mirf_write_register(EN_AA, mirf_read_register(EN_AA) | (1<<ENAA_P4));
		#endif
		#if mirf_ENABLED_P5 == 1
			mirf_write_register(EN_AA, mirf_read_register(EN_AA) | (1<<ENAA_P5));
		#endif
	#else
		mirf_write_register(EN_AA, 0); //auto acknowledgement
	#endif

	mirf_write_register(EN_RXADDR, 0);
	#if mirf_ENABLED_P0 == 1
		mirf_write_register(EN_RXADDR, mirf_read_register(EN_RXADDR) | (1<<ERX_P0));
	#endif
	#if mirf_ENABLED_P1 == 1
		mirf_write_register(EN_RXADDR, mirf_read_register(EN_RXADDR) | (1<<ERX_P1));
	#endif
	#if mirf_ENABLED_P2 == 1
		mirf_write_register(EN_RXADDR, mirf_read_register(EN_RXADDR) | (1<<ERX_P2));
	#endif
	#if mirf_ENABLED_P3 == 1
		mirf_write_register(EN_RXADDR, mirf_read_register(EN_RXADDR) | (1<<ERX_P3));
	#endif
	#if mirf_ENABLED_P4 == 1
		mirf_write_register(EN_RXADDR, mirf_read_register(EN_RXADDR) | (1<<ERX_P4));
	#endif
	#if mirf_ENABLED_P5 == 1
		mirf_write_register(EN_RXADDR, mirf_read_register(EN_RXADDR) | (1<<ERX_P5));
	#endif

    MIRF_SET_PD();
    MIRF_CE_LO();
}

//set rx address
void mirf_set_rxaddr(uint8_t channel, uint8_t *addr)
{
    MIRF_CE_LO();
    switch(channel)
    {
    	case 0:
    	    mirf_write_registers(RX_ADDR_P0, addr, 5);
    		break;
    	case 1:
    		mirf_write_registers(RX_ADDR_P1, addr, 5);
    	    break;
    	case 2:
    		mirf_write_registers(RX_ADDR_P2, addr, 5);
    	    break;
    	case 3:
    		mirf_write_registers(RX_ADDR_P3, addr, 5);
    	    break;
    	case 4:
    		mirf_write_registers(RX_ADDR_P4, addr, 5);
    	   break;
    	case 5:
    	   mirf_write_registers(RX_ADDR_P5, addr, 5);
    	   break;

    }
    MIRF_CE_HI();
}

//set tx address
void mirf_set_txaddr(uint8_t *addr)
{
    mirf_write_registers(TX_ADDR, addr, 5);
}

//get status
uint8_t mirf_get_status(void)
{
	uint8_t status = 0;
	
	MIRF_CSN_LO();
	status = spi_writeread(NOP);
	MIRF_CSN_HI();
	return status;
}

//read one register
uint8_t mirf_read_register(uint8_t reg)
{
	uint8_t result;

	MIRF_CSN_LO();
	spi_writeread(R_REGISTER | (REGISTER_MASK & reg));
	result = spi_writeread(NOP);
	
    MIRF_CSN_HI();
    return result;
}

//read many registers
void mirf_read_registers(uint8_t reg, uint8_t *value, uint8_t len)
{
	uint8_t i;
	
    MIRF_CSN_LO();
	spi_writeread(R_REGISTER | (REGISTER_MASK & reg));
	for (i = 0; i < len; i++)
    {
		value[i] = spi_writeread(NOP);
    }
    MIRF_CSN_HI();
}

//write one register
void mirf_write_register(uint8_t reg, uint8_t value)
{
	MIRF_CSN_LO();
	spi_writeread(W_REGISTER | (REGISTER_MASK & reg));
	spi_writeread(value);
	MIRF_CSN_HI();
}

//write many registers
void mirf_write_registers(uint8_t reg, uint8_t *value, uint8_t len)
{
	uint8_t i;
	
    MIRF_CSN_LO();
	spi_writeread(W_REGISTER | (REGISTER_MASK & reg));
	for (i = 0; i < len; i++)
    {
		spi_writeread(value[i]);
    }
    MIRF_CSN_HI();
}

//check if there is rx data
uint8_t mirf_read_ready(void)
{
    uint8_t status = mirf_get_status();
    return (status & (1<<RX_DR));
}

uint8_t mirf_write_ready(void)
{
    uint8_t status = mirf_get_status();
    return (status & (1<<TX_DS));
}

//read data
void mirf_read(uint8_t *data)
{
	volatile uint8_t i;

    MIRF_CSN_LO();
	spi_writeread(R_RX_PAYLOAD);
    for(i = 0; i < mirf_PAYLOAD; i++)
    {   
		data[i] = spi_writeread(NOP);
    }
    MIRF_CSN_HI();
	
	//в рабочем MIRF такого нет, поэтому выведу это в условие SOFTSPI
mirf_write_register(STATUS, (1<<RX_DR));

}


void mirf_write(uint8_t *data)
{
    volatile uint8_t i;

	MIRF_CE_LO();
	_delay_ms(50);
	MIRF_SET_TX();
	_delay_us(130);
	
	MIRF_CE_HI();		//	без этого не отправляет
	_delay_ms(50);

	//write data
	MIRF_CSN_LO();
	_delay_ms(50);
	spi_writeread( W_TX_PAYLOAD );
	for (i = 0; i < mirf_PAYLOAD; i++)
    {
		spi_writeread(data[i]);
    }
	MIRF_CSN_HI();

	//start transmission
	MIRF_CE_HI();
    _delay_us(10);
    MIRF_CE_LO();
	
/*	
	// проверим успешна ли отпрака (пришло ли подтверждение на канал 0)						
	while( !(mirf_get_status() & (1<<TX_DS)) )
	{
		//sei();
		_delay_us(10);
	}
*/
	mirf_write_register(STATUS, (1<<TX_DS));	// сбросим флаг успешной передачи
}

void mirf_clear(void)
{
	uint8_t tmp = 0;
	tmp = mirf_get_status();
	//tmp = tmp & ((1<<MAX_RT)|(1<<RX_DR)|(1<<TX_DS));
	mirf_write_register(STATUS, tmp);
}

void mirf_rx_clear(void)
{
	uint8_t ttt;
	MIRF_CSN_LO();
	spi_writeread(FLUSH_RX);
	MIRF_CSN_HI();
	ttt = mirf_get_status();
	mirf_write_register(STATUS,ttt);
}

/*
RF_INTERRUPT(void)
{
	_delay_ms(50);

	_delay_ms(50);
}
*/


