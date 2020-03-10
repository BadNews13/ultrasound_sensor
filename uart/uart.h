
#ifndef UART_H_
#define UART_H_

#define UART_TX_DDR			DDRD
#define UART_TX_PORT		PORTD
#define UART_TX_PIN			PIND
#define UART_TX_PIN_NUM		PORTD1
#define UART_RX_DDR			DDRD
#define UART_RX_PORT		PORTD
#define UART_RX_PIN			PIND
#define UART_RX_PIN_NUM		PORTD0

//#define F_CPU 16000000UL

#define sbit(var,bit)			do{var |= (1<<bit);}while(0)			// установить бит
#define cbit(var,bit)			do{var &= ~(1<<bit);}while(0)			// очистить бит
#define tbit(var,bit)			do{var ^= (1<<bit);}while(0)			// переключить бит
#define bit_is_set(var,bit)		(var & (1<<bit))						// проверить бит

#define RX_BUFFER_SIZE			128
#define TX_BUFFER_SIZE			RX_BUFFER_SIZE
#define NO_CHAR					0xFFFF

#define DATA_REGISTER_EMPTY		(1<<UDRE0)
#define RX_COMPLETE				(1<<RXC0)
#define FRAMING_ERROR			(1<<FE0)
#define PARITY_ERROR			(1<<UPE0)
#define DATA_OVERRUN			(1<<DOR0)

#define ENABLE_UART_TX_INT		do{sbit(UCSR0B,TXCIE0);}while(0)
#define DISABLE_UART_TX_INT		do{cbit(UCSR0B,TXCIE0);}while(0)
#define ENABLE_UART_UDRIE_INT	do{sbit(UCSR0B,UDRIE0);}while(0)
#define DISABLE_UART_UDRIE_INT	do{cbit(UCSR0B,UDRIE0);}while(0)

void uart_ini(uint8_t _dipBaudRate, uint8_t _dipAdress);
void put_byte(uint8_t c);
uint16_t get_byte(void);
uint16_t return_main_rs485_address(void);
void reset_uart_rx(void);
void parsing_uart_RX(void);

#endif /* UART_H_ */