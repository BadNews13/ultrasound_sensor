/*
 * spi_master.h
 *
 * Created: 21.01.2020 13:05:15
 *  Author: bad_n
 */ 


#ifndef SPI_MASTER_H_
#define SPI_MASTER_H_


#define sbit(var,bit)			do{var |= (1<<bit);}while(0)			// установить бит
#define cbit(var,bit)			do{var &= ~(1<<bit);}while(0)			// очистить бит
#define tbit(var,bit)			do{var ^= (1<<bit);}while(0)			// переключить бит
#define bit_is_set(var,bit)		(var & (1<<bit))						// проверить бит



#define F_CPU 16000000UL

#define SPI_PORTX PORTB
#define SPI_DDRX DDRB

#define SPI_BUFFER_SIZE 128					//	размер буфера SPI

#define LAST_BYTE 0							//	флаг отсутсвия байта

#define PORT_SPI  PORTB
#define DDR_SPI   DDRB
#define SPI_MISO  DDB4
#define SPI_MOSI  DDB3
#define SPI_SCK   DDB5

#define ENABLE_SPI_SPIE_INT		do{sbit(SPCR,SPIE);}while(0)
#define DISABLE_SPI_SPIE_INT	do{cbit(SPCR,SPIE);}while(0)

#define SPI_SPIE_INT_ON			bit_is_set(SPCR,SPIE)

void spi_ini(void);							//	инициализация SPI hardware

#endif /* SPI_MASTER_H_ */