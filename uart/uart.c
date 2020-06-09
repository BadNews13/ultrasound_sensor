#include "../defines/defines_global.h"
#include <avr/io.h>
#include "uart.h"
#include <avr/interrupt.h>
#include "../RTOS_assembly/rtos/rtos.h"
#include "../shell/cmd_interp.h"
#include "../crc/crc8.h"

volatile uint8_t tx_counter = 0;							// количество byte в буфере отправки
volatile uint8_t tx_write_index = 0;						// индекс записываемого байта в буфер отправки
volatile uint8_t tx_read_index = 0;							// индекс считываемого байта из буфера отправки

volatile uint8_t rx_counter = 0;							// количество byte в буфере преима
volatile uint8_t rx_write_index = 0;						// индекс записываемого байта из буфер приема
volatile uint8_t rx_read_index = 0;							// индекс считываемого байта из буфера приема

uint8_t rx_buffer[RX_BUFFER_SIZE];							// принятые данные
uint8_t tx_buffer[TX_BUFFER_SIZE];							// данные для отправки

uint8_t rx_buffer_overflow = 0;								// буфер приема переполнен

uint16_t main_rs485_address;									// адрес платы

	uint16_t UART_BAUDRATE;
	uint16_t BAUD_PRESCALE;
	

uint8_t pack_state = 0;										// пак получен
uint8_t	first_byte_from_pack = 0;							// первый байт полученный в uart

uint8_t pack_from_uart[RX_BUFFER_SIZE];						// пакет из uart
uint8_t first_byte_taken = 0;								// флаг "первый байт получен"
uint8_t byte_index = 0;										// индекс байтов в вытаскиваемом пакете


	

void uart_ini(uint8_t _dipBaudRate, uint8_t _dipAdress)		// настройка UART
{
	main_rs485_address = _dipAdress;
	UART_BAUDRATE = _dipBaudRate*100;
	BAUD_PRESCALE = (((F_CPU / (UART_BAUDRATE * 16UL))) - 1);

	UBRR0L =	BAUD_PRESCALE;			//Младшие 8 бит UBRRL_value
	UBRR0H =	(BAUD_PRESCALE >> 8);	//Старшие 8 бит UBRRL_value

	UCSR0A =	(0<<RXC0)|			// Bit - 7	RXC — флаг завершения приема, устанавливается в 1 при наличие непрочитанных данных в буфере приемник — UDR;
				(0<<TXC0)|			// Bit - 6	TXC — флаг завершения передачи, устанавливается в 1 при передачи всех разрядов из передатчика — UDR;
				(0<<UDRE0)|			// Bit - 5	UDRE — флаг опустошения регистра передатчика, устанавливается в 1 при пустом буфере передатчика — UDR после передачи;
				(0<<FE0)|			// Bit - 4	FE — флаг ошибки кадрирования, устанавливается в 1 при обнаружение неправильного кадра, когда стоп бит равен 0-лю
				(0<<DOR0)|			// Bit - 3	DOR —  флаг переполнения регистра приемника, устанавливается в 1, когда байт данных принят, а предыдущий еще не прочитан из UDR;
				(0<<UPE0)|			// Bit - 2	PE —  флаг ошибки контроля четности, устанавливается в 1 при обнаружение ошибки контроля четности (если включена проверка);
				(0<<U2X0)|			// Bit - 1	U2X — бит установки удвоенной скорости обмена, если установлена 1, то скорость передачи удваивается (частота делится на 8, а не на 16), данный бит используется только при асинхронном режиме работы;
				(0<<MPCM0);			// Bit - 0	MPCM — бит мультипроцессорного обмена, если установлена 1, то контроллер аппаратно не принимает информацию, а только кадры с адресами, далее устанавливается бит завершения приема (или прерывание) и программа обрабатывает адрес, её ли это адрес. Отличие информации от адреса определяется  с помощью 9-ого бита в режиме 9-и битового обмена.
	
	UCSR0B =	(1<<RXCIE0)|		// Bit - 7	RXCIE — бит разрешения прерывания по завершению приема, если установлена 1, то при установке флага RXC регистра UCSRA произойдет прерывание «прием завершен«;
				(1<<TXCIE0)|		// Bit - 6	TXCIE — бит разрешения прерывания по завершению передачи, если установлена 1, то при установке флага TXC регистра UCSRA произойдет прерывание «передача завершена«;
				(0<<UDRIE0)|		// Bit - 5	UDRIE — бит разрешения прерывания по опустошению регистра передатчика, если установлена 1, то при установке флага UDRE регистра UCSRA произойдет прерывание «регистр данных пуст«;
				(1<<RXEN0)|			// Bit - 4	RXEN — бит разрешения приема, при установки 1 разрешается работа приемника USART и переопределяется функционирование вывода RXD;
				(1<<TXEN0)|			// Bit - 3	TXEN —  бит разрешения передачи, при установки 1 разрешается работа передатчика USART и переопределяется функционирование вывода TXD;
				(0<<UCSZ02)|		// Bit - 2	UCSZ2 — бит формат посылок, данный бит совместно с битами UCSZ1 и UCSZ0 регистра UCSRC определяют количество бит данных в кадрах
				(0<<RXB80)|			// Bit - 1	RXB8 — 9-ый разряд принимаемых данных при использование 9-и битого режима, считывать из данного бита нужно до считывание регистра UDR;
				(0<<TXB80);			// Bit - 0	TXB8 — 9-ый разряд передаваемых данных при использование 9-и битного режима, записывать в данный бит нужно до записи в регистр UDR.
	
	UCSR0C =	(0<<UMSEL01)|		// Bit - 7	UMSEL1 — бит выбора режима Master SPI. Если 1 и UMSEL0 = 1 то режим Master SPI (MSPIM)
				(0<<UMSEL00)|		// Bit - 6	UMSEL0 — бит выбора режима асинхронный или синхронный, если установлен 1 — режим синхронный (т.е. с использованием линии синхронизации XCK), если 0 — режим асинхронный;
				(0<<UPM01)|			// Bit - 5	UPM1 — бит выбора режима проверки на четность/нечетность;
				(0<<UPM00)|			// Bit - 4	UPM0 — бит выбора режима проверки на четность/нечетность;
				(0<<USBS0)|			// Bit - 3	USBS — бит отвечающий за количество стоп-битов, если установлена 1 — два стоп-бита, если 0 — один стоп-бит;
				(1<<UCSZ01)|		// Bit - 2	UCSZ1 — совместно с битом UCSZ2 регистра UCSRB определяют количество бит данных в кадрах (см. таблицу выше);
				(1<<UCSZ00)|		// Bit - 1	UCSZ0 — совместно с битом UCSZ2 регистра UCSRB определяют количество бит данных в кадрах (см. таблицу выше);
				(0<<UCPOL0);		// Bit - 0	UCPOL — бит полярность тактового сигнала, при синхронном режиме определяет по какому фронту принимать/передавать данные – по спадающему или по нарастающему.

	RS485_RX_MODE;
}


void put_byte(uint8_t c)
{
	while (tx_counter == TX_BUFFER_SIZE);	// если количество байт к отправке равно размеру буфера, то ждем отправку
	
	cli();
	
	tx_buffer[tx_write_index++] = c;								// запишем байт в буфер
	
	if (tx_write_index == TX_BUFFER_SIZE)	{tx_write_index = 0;}	// если дошли до конца буфера, то переставим индекс в начало	
	
	tx_counter++;													// увеличим счетчик количества байтов к отправке

	ENABLE_UART_UDRIE_INT;											// разрешаем прерывание по опустошению регистра UDR0
	
	sei();
}


ISR(USART_UDRE_vect)		// отправлять данные, если есть (вызывается, когда регистр пуст)
{
	if(tx_counter == 0) {RS485_RX_MODE;}
	else				{RS485_TX_MODE;}
		
	// если есть байты на отправку
	if (tx_counter)
	{
		--tx_counter;
		
		UDR0 = tx_buffer[tx_read_index++];
		
		if (tx_read_index == TX_BUFFER_SIZE)	{tx_read_index = 0;}	// если дошли до конца буфера, то переставим индекс в начало	

		if (tx_counter == 0)	{DISABLE_UART_UDRIE_INT;}				// если отправили последний байт, то отключим прерывание по опустошению UDR

	}
	else {DISABLE_UART_UDRIE_INT;}										// если данных не было, то отключим прерывание по опустошению UDR
}


uint16_t get_byte(void)
{
	uint8_t data;
	
	if(rx_counter == 0)		{return NO_CHAR;}

	data = rx_buffer[rx_read_index++];
	
	if (rx_read_index == RX_BUFFER_SIZE)	{rx_read_index = 0;}	// индекс идет по кругу
	
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
		if(!(tmp_byte == NO_CHAR))			//	если вытянули байт
		{		
			RTOS_SetTask(reset_uart_rx, 1, 0);
			if(!first_byte_taken)			//	если первого байта еще не было
			{
				byte_index = 0;				//	обнуляем индекс вытянутых байт
				first_byte_taken = 1;		//	поднимаем флаг "первый байт занят"
			}
				
			pack_from_uart[byte_index++] = tmp_byte;		//	запишем байт в массив и увеличим индекс
				
			if(first_byte_taken == 1)						//	если 1-й байт занят
			{
				if(byte_index == pack_from_uart[0])			//	если кол-во пришедших байт равно содержимому 1-го байта
				{
					first_byte_taken = 0;					//	снимаем флаг "первый бай занят"

					uint8_t tmp_crc8 = crc8(&pack_from_uart[0],pack_from_uart[0]-1);		//	посчитаем crc
						
					if(tmp_crc8 == pack_from_uart[pack_from_uart[0]-1])			//	проверим crc
					{
						pack_state = 1;							//	поднимаем флаг "пакет готов к обработке"
						processing_pack(&pack_from_uart[0]);
					}
					else
					{
						first_byte_taken = 0;					//	снимаем флаг "первый бай занят"
						pack_state = 0;							//	опускаем флаг "пакет готов к обработке"
						byte_index = 0;							//	обнуляем индекс вытянутых байт
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
