#ifndef _MIRF_H_
#define _MIRF_H_ 1

#include <stdio.h>

#include "nRF24L01.h"

#ifndef uint16_t
#define uint8_t unsigned char
#define uint16_t unsigned int
#endif

#define NRF_CE_DDR		DDRD
#define NRF_CE_PORT		PORTD
#define NRF_CE_PIN_NUM  PORTD7

#define NRF_CSN_DDR		DDRB
#define NRF_CSN_PORT	PORTB
#define NRF_CSN_PIN_NUM	PORTB0

#define mirf_CH      87+(0x0F*2)//124 //transmission channel
#define mirf_PAYLOAD 16 //payload lenght
#define mirf_CONFIG ((1<<EN_CRC) | (1<<CRCO) | (1<<MASK_TX_DS) | (0<<MASK_RX_DR))
#define mirf_ACK 0 //auto ack enabled
#define mirf_RETR 0x00 //auto ack enabled
//#define mirf_RETR 0x00
#define mirf_ENABLED_P0 1
#define mirf_ENABLED_P1 1
#define mirf_ENABLED_P2 0
#define mirf_ENABLED_P3 0
#define mirf_ENABLED_P4 0
#define mirf_ENABLED_P5 0

#define MIRF_CSN_HI() do{sbit(NRF_CSN_PORT,NRF_CSN_PIN_NUM);}while(0)
#define MIRF_CSN_LO() do{cbit(NRF_CSN_PORT,NRF_CSN_PIN_NUM);}while(0)

#define MIRF_CE_HI() do{sbit(NRF_CE_PORT,NRF_CE_PIN_NUM);}while(0)
#define MIRF_CE_LO() do{cbit(NRF_CE_PORT,NRF_CE_PIN_NUM);}while(0)

#define MIRF_SET_TX() do{mirf_write_register(CONFIG, mirf_CONFIG | ( (1<<PWR_UP) | (0<<PRIM_RX) ) );}while(0)
#define MIRF_SET_RX() do{mirf_write_register(CONFIG, mirf_CONFIG | ( (1<<PWR_UP) | (1<<PRIM_RX) ) );}while(0)
#define MIRF_SET_PD() do{mirf_write_register(CONFIG, mirf_CONFIG | ( (0<<PWR_UP) | (1<<PRIM_RX) ) );}while(0)

uint8_t spi_writeread (uint8_t data);

void mirf_init(void);
void mirf_config(void);
uint8_t mirf_get_status(void);
void mirf_set_rxaddr(uint8_t channel, uint8_t *addr);
void mirf_set_txaddr(uint8_t *addr);
uint8_t mirf_read_ready(void);
uint8_t mirf_write_ready(void);
void mirf_read(uint8_t *data);
void mirf_write(uint8_t *data);

void mirf_write_register(uint8_t reg, uint8_t value);
void mirf_write_registers(uint8_t reg, uint8_t *value, uint8_t len);
uint8_t mirf_read_register(uint8_t reg);
void mirf_read_registers(uint8_t reg, uint8_t *value, uint8_t len);

void mirf_clear(void);
void mirf_rx_clear(void);


#endif /* _MIRF_H_ */

