#include "../defines/defines_global.h"
#include <avr/io.h>
#include "uart.h"
#include <avr/interrupt.h>
#include "../RTOS_assembly/rtos/rtos.h"
#include "../shell/cmd_interp.h"
#include "../crc/crc8.h"

volatile uint8_t tx_counter = 0;							// ���������� byte � ������ ��������
volatile uint8_t tx_write_index = 0;						// ������ ������������� ����� � ����� ��������
volatile uint8_t tx_read_index = 0;							// ������ ������������ ����� �� ������ ��������

volatile uint8_t rx_counter = 0;							// ���������� byte � ������ ������
volatile uint8_t rx_write_index = 0;						// ������ ������������� ����� �� ����� ������
volatile uint8_t rx_read_index = 0;							// ������ ������������ ����� �� ������ ������

uint8_t rx_buffer[RX_BUFFER_SIZE];							// �������� ������
uint8_t tx_buffer[TX_BUFFER_SIZE];							// ������ ��� ��������

uint8_t rx_buffer_overflow = 0;								// ����� ������ ����������

uint16_t main_rs485_address;									// ����� �����

	uint16_t UART_BAUDRATE;
	uint16_t BAUD_PRESCALE;
	

uint8_t pack_state = 0;										// ��� �������
uint8_t	first_byte_from_pack = 0;							// ������ ���� ���������� � uart

uint8_t pack_from_uart[RX_BUFFER_SIZE];						// ����� �� uart
uint8_t first_byte_taken = 0;								// ���� "������ ���� �������"
uint8_t byte_index = 0;										// ������ ������ � ������������� ������


	

void uart_ini(uint8_t _dipBaudRate, uint8_t _dipAdress)		// ��������� UART
{
	main_rs485_address = _dipAdress;
	UART_BAUDRATE = _dipBaudRate*100;
	BAUD_PRESCALE = (((F_CPU / (UART_BAUDRATE * 16UL))) - 1);

	UBRR0L =	BAUD_PRESCALE;			//������� 8 ��� UBRRL_value
	UBRR0H =	(BAUD_PRESCALE >> 8);	//������� 8 ��� UBRRL_value

	UCSR0A =	(0<<RXC0)|			// Bit - 7	RXC � ���� ���������� ������, ��������������� � 1 ��� ������� ������������� ������ � ������ �������� � UDR;
				(0<<TXC0)|			// Bit - 6	TXC � ���� ���������� ��������, ��������������� � 1 ��� �������� ���� �������� �� ����������� � UDR;
				(0<<UDRE0)|			// Bit - 5	UDRE � ���� ����������� �������� �����������, ��������������� � 1 ��� ������ ������ ����������� � UDR ����� ��������;
				(0<<FE0)|			// Bit - 4	FE � ���� ������ ������������, ��������������� � 1 ��� ����������� ������������� �����, ����� ���� ��� ����� 0-��
				(0<<DOR0)|			// Bit - 3	DOR �  ���� ������������ �������� ���������, ��������������� � 1, ����� ���� ������ ������, � ���������� ��� �� �������� �� UDR;
				(0<<UPE0)|			// Bit - 2	PE �  ���� ������ �������� ��������, ��������������� � 1 ��� ����������� ������ �������� �������� (���� �������� ��������);
				(0<<U2X0)|			// Bit - 1	U2X � ��� ��������� ��������� �������� ������, ���� ����������� 1, �� �������� �������� ����������� (������� ������� �� 8, � �� �� 16), ������ ��� ������������ ������ ��� ����������� ������ ������;
				(0<<MPCM0);			// Bit - 0	MPCM � ��� ������������������� ������, ���� ����������� 1, �� ���������� ��������� �� ��������� ����������, � ������ ����� � ��������, ����� ��������������� ��� ���������� ������ (��� ����������) � ��������� ������������ �����, � �� ��� �����. ������� ���������� �� ������ ������������  � ������� 9-��� ���� � ������ 9-� �������� ������.
	
	UCSR0B =	(1<<RXCIE0)|		// Bit - 7	RXCIE � ��� ���������� ���������� �� ���������� ������, ���� ����������� 1, �� ��� ��������� ����� RXC �������� UCSRA ���������� ���������� ������ ��������;
				(1<<TXCIE0)|		// Bit - 6	TXCIE � ��� ���������� ���������� �� ���������� ��������, ���� ����������� 1, �� ��� ��������� ����� TXC �������� UCSRA ���������� ���������� ��������� ���������;
				(0<<UDRIE0)|		// Bit - 5	UDRIE � ��� ���������� ���������� �� ����������� �������� �����������, ���� ����������� 1, �� ��� ��������� ����� UDRE �������� UCSRA ���������� ���������� �������� ������ ����;
				(1<<RXEN0)|			// Bit - 4	RXEN � ��� ���������� ������, ��� ��������� 1 ����������� ������ ��������� USART � ���������������� ���������������� ������ RXD;
				(1<<TXEN0)|			// Bit - 3	TXEN �  ��� ���������� ��������, ��� ��������� 1 ����������� ������ ����������� USART � ���������������� ���������������� ������ TXD;
				(0<<UCSZ02)|		// Bit - 2	UCSZ2 � ��� ������ �������, ������ ��� ��������� � ������ UCSZ1 � UCSZ0 �������� UCSRC ���������� ���������� ��� ������ � ������
				(0<<RXB80)|			// Bit - 1	RXB8 � 9-�� ������ ����������� ������ ��� ������������� 9-� ������ ������, ��������� �� ������� ���� ����� �� ���������� �������� UDR;
				(0<<TXB80);			// Bit - 0	TXB8 � 9-�� ������ ������������ ������ ��� ������������� 9-� ������� ������, ���������� � ������ ��� ����� �� ������ � ������� UDR.
	
	UCSR0C =	(0<<UMSEL01)|		// Bit - 7	UMSEL1 � ��� ������ ������ Master SPI. ���� 1 � UMSEL0 = 1 �� ����� Master SPI (MSPIM)
				(0<<UMSEL00)|		// Bit - 6	UMSEL0 � ��� ������ ������ ����������� ��� ����������, ���� ���������� 1 � ����� ���������� (�.�. � �������������� ����� ������������� XCK), ���� 0 � ����� �����������;
				(0<<UPM01)|			// Bit - 5	UPM1 � ��� ������ ������ �������� �� ��������/����������;
				(0<<UPM00)|			// Bit - 4	UPM0 � ��� ������ ������ �������� �� ��������/����������;
				(0<<USBS0)|			// Bit - 3	USBS � ��� ���������� �� ���������� ����-�����, ���� ����������� 1 � ��� ����-����, ���� 0 � ���� ����-���;
				(1<<UCSZ01)|		// Bit - 2	UCSZ1 � ��������� � ����� UCSZ2 �������� UCSRB ���������� ���������� ��� ������ � ������ (��. ������� ����);
				(1<<UCSZ00)|		// Bit - 1	UCSZ0 � ��������� � ����� UCSZ2 �������� UCSRB ���������� ���������� ��� ������ � ������ (��. ������� ����);
				(0<<UCPOL0);		// Bit - 0	UCPOL � ��� ���������� ��������� �������, ��� ���������� ������ ���������� �� ������ ������ ���������/���������� ������ � �� ���������� ��� �� ������������.

	RS485_RX_MODE;
}


void put_byte(uint8_t c)
{
	while (tx_counter == TX_BUFFER_SIZE);	// ���� ���������� ���� � �������� ����� ������� ������, �� ���� ��������
	
	cli();
	
	tx_buffer[tx_write_index++] = c;								// ������� ���� � �����
	
	if (tx_write_index == TX_BUFFER_SIZE)	{tx_write_index = 0;}	// ���� ����� �� ����� ������, �� ���������� ������ � ������	
	
	tx_counter++;													// �������� ������� ���������� ������ � ��������

	ENABLE_UART_UDRIE_INT;											// ��������� ���������� �� ����������� �������� UDR0
	
	sei();
}


ISR(USART_UDRE_vect)		// ���������� ������, ���� ���� (����������, ����� ������� ����)
{
	if(tx_counter == 0) {RS485_RX_MODE;}
	else				{RS485_TX_MODE;}
		
	// ���� ���� ����� �� ��������
	if (tx_counter)
	{
		--tx_counter;
		
		UDR0 = tx_buffer[tx_read_index++];
		
		if (tx_read_index == TX_BUFFER_SIZE)	{tx_read_index = 0;}	// ���� ����� �� ����� ������, �� ���������� ������ � ������	

		if (tx_counter == 0)	{DISABLE_UART_UDRIE_INT;}				// ���� ��������� ��������� ����, �� �������� ���������� �� ����������� UDR

	}
	else {DISABLE_UART_UDRIE_INT;}										// ���� ������ �� ����, �� �������� ���������� �� ����������� UDR
}


uint16_t get_byte(void)
{
	uint8_t data;
	
	if(rx_counter == 0)		{return NO_CHAR;}

	data = rx_buffer[rx_read_index++];
	
	if (rx_read_index == RX_BUFFER_SIZE)	{rx_read_index = 0;}	// ������ ���� �� �����
	
	--rx_counter;
	
	return data;
}


ISR(USART_RX_vect)
{
	if(tx_counter == 0) {RS485_RX_MODE;}
	else				{RS485_TX_MODE;}	
	
	uint16_t status, data;

	status = UCSR0A;
	data = UDR0;
	if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN)) == 0)
	{
		rx_buffer[rx_write_index++] = data;
		 
		if (rx_write_index == RX_BUFFER_SIZE)	{rx_counter = 0;}

		if (++rx_counter == RX_BUFFER_SIZE)
		{
			rx_counter = 0;
			rx_buffer_overflow = 1;
		}
	}
}

uint16_t return_main_rs485_address(void)
{
	return main_rs485_address;
}

	
void parsing_uart_RX(void)
{
	uint8_t start_RX = 1;
	while(start_RX)
	{
		uint16_t tmp_byte = get_byte();
		if(!(tmp_byte == NO_CHAR))			//	���� �������� ����
		{		
			RTOS_SetTask(reset_uart_rx, 1, 0);
			if(!first_byte_taken)			//	���� ������� ����� ��� �� ����
			{
				byte_index = 0;				//	�������� ������ ��������� ����
				first_byte_taken = 1;		//	��������� ���� "������ ���� �����"
			}
				
			pack_from_uart[byte_index++] = tmp_byte;		//	������� ���� � ������ � �������� ������
				
			if(first_byte_taken == 1)						//	���� 1-� ���� �����
			{
				if(byte_index == pack_from_uart[0])			//	���� ���-�� ��������� ���� ����� ����������� 1-�� �����
				{
					first_byte_taken = 0;					//	������� ���� "������ ��� �����"

					uint8_t tmp_crc8 = crc8(&pack_from_uart[0],pack_from_uart[0]-1);		//	��������� crc
						
					if(tmp_crc8 == pack_from_uart[pack_from_uart[0]-1])			//	�������� crc
					{
						pack_state = 1;							//	��������� ���� "����� ����� � ���������"
						processing_pack(&pack_from_uart[0]);
					}
					else
					{
						first_byte_taken = 0;					//	������� ���� "������ ��� �����"
						pack_state = 0;							//	�������� ���� "����� ����� � ���������"
						byte_index = 0;							//	�������� ������ ��������� ����
					}
				}
			}
		}else{start_RX = 0;}
	}
	RTOS_SetTask(parsing_uart_RX, 2, 0);
}


void reset_uart_rx(void)
{
	rx_counter = 0;
	rx_write_index = 0;
	rx_read_index = 0;
			
	pack_state = 0;
	first_byte_taken = 0;
	byte_index = 0;
	rx_buffer_overflow = 0;
}



ISR(USART_TX_vect)
{
	if(tx_counter == 0) {RS485_RX_MODE;}
	else				{RS485_TX_MODE;}
}
