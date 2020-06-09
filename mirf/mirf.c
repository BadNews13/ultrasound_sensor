#include "../defines/defines_global.h"
#include <util/delay.h>
#include <avr/io.h>
#include "mirf.h"
#include "nRF24L01.h"
#include "../spi/spi.h"

#include "../uart/uart.h"	//	��� �������
#include <avr/interrupt.h>

void MIRF_SET_TX(void)
{
	cli();
	uint8_t tmp_config = mirf_read_register(CONFIG);	//	������� ������� ��������� �� ����
	sbit(tmp_config,PWR_UP);							//	������� ������� ������
	cbit(tmp_config,PRIM_RX);							//	��������� � ����� �����������
	mirf_write_register(CONFIG, tmp_config);			//	������� ����� ���������
	MIRF_CE_LO();										//	� ���� ( � ���� ������ ������� (10 ���) �� CE ��������� �������� �� ������ FIFO)
	_delay_ms(50);										//	����� ��� ������ ������ � ������� ����� ������
	sei();
}

void MIRF_SET_RX(void)
{
	cli();
	uint8_t tmp_config = mirf_read_register(CONFIG);	//	������� ������� ��������� �� ����
	sbit(tmp_config,PWR_UP);							//	������� ������� ������
	sbit(tmp_config,PRIM_RX);							//	��������� � ����� ���������
	mirf_write_register(CONFIG, tmp_config);			//	������� ����� ���������
	MIRF_CE_HI();										//	� ������ RX �������� �� CE(high) ��������� ������������� �����
	_delay_ms(50);										//	����� ��� ������ ������ � ������� ����� ������
	sei();
}

void MIRF_SET_PD(void)
{
	uint8_t tmp_config = mirf_read_register(CONFIG);	//	������� ������� ��������� �� ����
	cbit(tmp_config,PWR_UP);							//	�������� ������� ������
	sbit(tmp_config,PRIM_RX);							//	��������� � ����� �����������
	mirf_write_register(CONFIG, tmp_config);			//	������� ����� ���������
	MIRF_CE_LO();
}

void setRetries(uint8_t delay, uint8_t count)
{
	mirf_write_register(SETUP_RETR, (delay & 0xf) << ARD | (count & 0xf) << ARC);
}

uint8_t mirf_cmd(uint8_t cmd)
{
	uint8_t status = 0;
	MIRF_CSN_LO();
	status = spi_writeread(cmd);
	MIRF_CSN_HI();
	return status;
}

void mirf_powerUp(void)
{
	//��� ����������� ������� ����� ������ �������� �� SPI � ����� (���������� ����������)
	uint8_t cfg = mirf_read_register(CONFIG);

	// if not powered up then power up and wait for the radio to initialize
	if (!(cfg & (1<<PWR_UP)))	{mirf_write_register(CONFIG, cfg | (1<<PWR_UP));	_delay_us(10);}


	// For nRF24L01+ to go from power down mode to TX or RX mode it must first pass through stand-by mode.
	// There must be a delay of Tpd2stby (see Table 16.) after the nRF24L01+ leaves power down mode before
	// the CEis set high. - Tpd2stby can be up to 5ms per the 1.0 datasheet
}


//initialize mirf
uint8_t mirf_init(void)
{
	//	�����
	//	������� ���������
	//	SPI
		
	uint8_t setup = 0;
	    
	MIRF_CE_LO();	//	CE: Chip Enable. ������� �� ������ ������. ���� ��� ����������� ��� ��������, �� ������� (HIGH) ������� �� CE ��������� ���� ���������� ����� � �������� ������. ������ (LOW) ������� ��������� ��� � Standby-I � ����� ����������� ���������� ��� ����������. ���� ��� �������� �� ��������, CE ������ �������� �� ������ ������. � ���� ������ ��� �������� ������ ����� �������� �� � ������� FIFO � ������� CE ������� �� 10��� (LOW->HIGH, 10���, HIGH->LOW).
	MIRF_CSN_HI();	//	�������� ������� - ������ (����� ���� ��� ���������� - ��������� � �������) (��� �������������� ����������� � +)

	mirf_write_register(CONFIG, (1 << EN_CRC) | (1 << CRCO) | (1 << PWR_UP) | (1 << PRIM_RX));	    // Reset NRF_CONFIG and enable 16-bit CRC. (�������� 16-� ������ ����������� �����)
    
//	setRetries(150, 2);	//	�������� ����� � ���������� ��������� �������
		
	mirf_write_register(RF_SETUP, (1<<RF_DR_HIGH)|(1<<1)|(1<<2));			//	��������� �������� �������� ������ (max)
	mirf_write_register(FEATURE, 0x04);										//	���������: ���������� ������ �� ��������� ACK; ���������� ������ ������ � ACK �������; ���� ������ ������ �����
	mirf_write_register(DYNPD, (1 << DPL_P0) | (1 << DPL_P1));				//	�������� ����� ������� ������������ ����� �� ���� �������

	mirf_write_register(RF_CH, mirf_CH);	//	����� ����������� �� 0 �� 125
	
	//	8-������ ��������, �������� ������ ������, ����������� �� ������� (������), �������������� 0-5
	mirf_write_register(RX_PW_P0, mirf_PAYLOAD);		//length of incoming payload
	mirf_write_register(RX_PW_P1, mirf_PAYLOAD);
	mirf_write_register(RX_PW_P2, mirf_PAYLOAD);
	mirf_write_register(RX_PW_P3, mirf_PAYLOAD);
	mirf_write_register(RX_PW_P4, mirf_PAYLOAD);
	mirf_write_register(RX_PW_P5, mirf_PAYLOAD);
	
	mirf_write_register(EN_AA, 0x00);	// ��������� �� ����� ������ ���������� �����������������
	
	//�������� �������� ������ (�����) ��������. (���� �������� ���������������� �� ����� 0 ������ ���� �������� (� ���� �������� ACK)
	mirf_write_register(EN_RXADDR, 0x00);	//	��� ������ ��� ��������
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


	mirf_write_register(SETUP_AW, adr_3_bytes);	//	����������� ����� ������ (������ 5 ����)

	// Flush buffers
	mirf_cmd(FLUSH_RX);
	mirf_cmd(FLUSH_TX);
	
	mirf_write_register(STATUS, (1<<RX_DR)|(1<<TX_DS)|(1<<MAX_RT));		// ������� ����� ����������
	
	MIRF_CSN_HI();	//	����� ������� � �����
	
	// if setup is 0 or ff then there was no response from module
	return (setup != 0 && setup != 0xff);
}


//set rx address (������������� ����������� ����� ��� ������ �� ������ (�����))
void mirf_set_rxaddr(uint8_t channel, uint8_t *addr)
{
    MIRF_CE_LO();
    switch(channel)
    {
    	case 0:		{mirf_write_registers(RX_ADDR_P0, addr, 3);		break;}
		case 1:		{mirf_write_registers(RX_ADDR_P1, addr, 3);	    break;}
    	case 2:		{mirf_write_registers(RX_ADDR_P2, addr, 3);		break;}
    	case 3:		{mirf_write_registers(RX_ADDR_P3, addr, 3);		break;}
    	case 4:		{mirf_write_registers(RX_ADDR_P4, addr, 3);		break;}
    	case 5:		{mirf_write_registers(RX_ADDR_P5, addr, 3);		break;}
    }
    MIRF_CE_HI();
}

//set tx address	(����� ���������� ����������. �� ��������� E7E7E7E7E7)
void mirf_set_txaddr(uint8_t *addr)
{
    mirf_write_registers(TX_ADDR, addr, 3);
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
	for (i = 0; i < len; i++)    {value[i] = spi_writeread(NOP);}
    MIRF_CSN_HI();
}

//write one register
uint8_t mirf_write_register(uint8_t reg, uint8_t value)
{
	uint8_t status = 0;
	MIRF_CSN_LO();
	spi_writeread(W_REGISTER | (REGISTER_MASK & reg));
	spi_writeread(value);
	MIRF_CSN_HI();
	return status;	
}

//write many registers
void mirf_write_registers(uint8_t reg, uint8_t *value, uint8_t len)
{
	uint8_t i;
	MIRF_CSN_LO();
	spi_writeread(W_REGISTER | (REGISTER_MASK & reg));
	for (i = 0; i < len; i++)		{spi_writeread(value[i]);}
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
    for(i = 0; i < mirf_PAYLOAD; i++)		{data[i] = spi_writeread(NOP);}
    MIRF_CSN_HI();
}


void mirf_write(uint8_t *data)
{
	volatile uint8_t i;

	MIRF_CE_LO();		//	���� �������� ��������. ������� ������� �� ������ ������

	//write data
	MIRF_CSN_LO();		//	������ ������� �� CSN ��������� ������� � ����� �� SPI
	spi_writeread( W_TX_PAYLOAD );
	for (i = 0; i < mirf_PAYLOAD; i++)   {spi_writeread(data[i]);}
	MIRF_CSN_HI();
	
	//start transmission
	MIRF_CE_HI();
	_delay_us(15);
	MIRF_CE_LO();	

/*	// �������� ������� �� ������� (������ �� ������������� �� ����� 0)		 (����� ��� �����? - �� �� �������� ���� ����� �� ������. ����� ��� ���� �� ������� ������� mirf)				
	while( !(mirf_get_status() & (1<<TX_DS)) )
	{
//		sei();
		_delay_us(1);
	}*/
}

void mirf_clear(void)
{
	// Flush buffers
	mirf_cmd(FLUSH_RX);
	mirf_cmd(FLUSH_TX);
		
	mirf_write_register(STATUS, (1<<RX_DR)|(1<<TX_DS)|(1<<MAX_RT));		// ������� ����� ����������
/*	
	uint8_t tmp = 0;
	tmp = mirf_get_status();
	//tmp = tmp & ((1<<MAX_RT)|(1<<RX_DR)|(1<<TX_DS));
	mirf_write_register(STATUS, tmp);
	*/
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



