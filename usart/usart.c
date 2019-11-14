#include "usart.h"

 uint8_t rx_data = 0;		//	�������� ����
int last_rx_data = 0;
int tx_data = 0;		//	���� �� ��������
int last_tx_data = 0;

void USART_ini()			// ��������� UART
{
	cli();

	UBRR0L = UBRRL_value;       //������� 8 ��� UBRRL_value
	UBRR0H = UBRRL_value >> 8;  //������� 8 ��� UBRRL_value

	UCSR0A =	(0<<RXC0)|			// Bit - 7	RXC � ���� ���������� ������, ��������������� � 1 ��� ������� ������������� ������ � ������ �������� � UDR;
				(0<<TXC0)|			// Bit - 6	TXC � ���� ���������� ��������, ��������������� � 1 ��� �������� ���� �������� �� ����������� � UDR;
				(0<<UDRE0)|			// Bit - 5	UDRE � ���� ����������� �������� �����������, ��������������� � 1 ��� ������ ������ ����������� � UDR ����� ��������;
				(0<<FE0)|			// Bit - 4	FE � ���� ������ ������������, ��������������� � 1 ��� ����������� ������������� �����, ����� ���� ��� ����� 0-��
				(0<<DOR0)|			// Bit - 3	DOR �  ���� ������������ �������� ���������, ��������������� � 1, ����� ���� ������ ������, � ���������� ��� �� �������� �� UDR;
				(0<<UPE0)|			// Bit - 2	PE �  ���� ������ �������� ��������, ��������������� � 1 ��� ����������� ������ �������� �������� (���� �������� ��������);
				(0<<U2X0)|			// Bit - 1	U2X � ��� ��������� ��������� �������� ������, ���� ����������� 1, �� �������� �������� ����������� (������� ������� �� 8, � �� �� 16), ������ ��� ������������ ������ ��� ����������� ������ ������;
				(0<<MPCM0);			// Bit - 0	MPCM � ��� ������������������� ������, ���� ����������� 1, �� ���������� ��������� �� ��������� ����������, � ������ ����� � ��������, ����� ��������������� ��� ���������� ������ (��� ����������) � ��������� ������������ �����, � �� ��� �����. ������� ���������� �� ������ ������������  � ������� 9-��� ���� � ������ 9-� �������� ������.
	
	UCSR0B =	(1<<RXCIE0)|		// Bit - 7	RXCIE � ��� ���������� ���������� �� ���������� ������, ���� ����������� 1, �� ��� ��������� ����� RXC �������� UCSRA ���������� ���������� ������ ���������;
				(0<<TXCIE0)|		// Bit - 6	TXCIE � ��� ���������� ���������� �� ���������� ��������, ���� ����������� 1, �� ��� ��������� ����� TXC �������� UCSRA ���������� ���������� ��������� ���������;
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

	sei();
}

ISR(USART_RX_vect)			// ��������� ���� ������ ���� � ����� UART
{
	rx_data = UDR0;
//	PORTB = rx_data;
}

uint8_t getch_usart()			// �� ��������
{
	return rx_data;
}
void putch_usart(int tx_data)
{
		while (!(UCSR0A & (1 << UDRE0)));	// ������� ����������� ������ ��������
		{
			UDR0 = tx_data;
		
/*			while (((tx_data>>8)>0)  & (1 << UDRE0))
			{
				tx_data=tx_data>>8;
				while (!(UCSR0A & (1 << UDRE0)));	// ������� ����������� ������ ��������
				UDR0 = tx_data;
			}*/
		}
}

void write_float(float f)
{
	unsigned char *ptr;
	char i;
	ptr = (unsigned char *)&f;
	for (i=0;i<4;i++)
	putch_usart(*(ptr++));
}