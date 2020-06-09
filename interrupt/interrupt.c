
#include <avr/io.h>
#include <avr/interrupt.h>
#include "interrupt.h"
#include "../mirf/mirf.h"
#include "../defines/defines_global.h"
#include "../uart/uart.h"

#include "../sensor/measurement/measurement.h"
#include "../sensor/timer0/timer0.h"

//#include "../soft_uart/soft_uart_1/soft_uart_1.h"
//#include "../soft_uart/timer_1/timer_1.h"

volatile uint8_t inter_count = 0;

uint8_t in_data_to_mirf_for_test[mirf_PAYLOAD];


void int_ini(void)
{
	//���������� INT0 �� ����������� ������		(�������������: ������ ��������� �� ���������� ����� ���� �� ����� (��������� �� �� �����))
	EICRA =	(1<<ISC11)|		//	3 bit: ������� ��� INT1			//	����������� �� ��������� �����
			(0<<ISC10)|		//	2 bit: ������� ��� INT1
			(1<<ISC01)|		//	1 bit: ������� ��� INT0			//	����������� �� ������� ������� (1)
			(1<<ISC00);		//	0 bit: ������� ��� INT0			//				-//-

	EIMSK =	(1<<INT1)|		// ��������� ���������� �� INT1	(PD3)
			(1<<INT0);		// ��������� ���������� �� INT0	(PD2)		//	�� �������
}

//	���������� ��������
ISR(INT0_vect)
{
	save_tik(get_tik());		//	�������� ���������� ����� �� ������� � ������� ���������
}


//	���������� ������������
ISR(INT1_vect)
{
	put_byte(0xB4);

	uint8_t status_mirf;
	status_mirf = mirf_get_status();					//	����������� ������ nRF24L01

	if(bit_is_set(status_mirf,RX_DR))					//	���� ������ ���� "�������� ������"
	{
		mirf_read(&in_data_to_mirf_for_test[0]);		//	���������
		sbit(status_mirf,RX_DR);						//	������� ���� ��������� ���������
		mirf_write_register(STATUS, status_mirf);		//	������� ���� ��������� ���������
put_byte(in_data_to_mirf_for_test[1]);
		processing_pack(in_data_to_mirf_for_test);		//	�������� ����� �� ���������
		//for (uint8_t i=0; i<in_data_to_mirf_for_test[0]; i++)	{put_byte(in_data_to_mirf_for_test[i]);}		//	������� � uart ��� �������� (������� ����� �������)
	}
	
	if(bit_is_set(status_mirf,TX_DS))					//	���� ������ ���� "������ ������� ����������"
	{
		sbit(status_mirf,TX_DS);						//	������� ���� "������ ������� ����������"
		mirf_write_register(STATUS, status_mirf);		//	������� ���� "������ ������� ����������"
	}
	
	if(bit_is_set(status_mirf,MAX_RT))					//	���� ������ ���� "���������� ������� ��������"
	{
		sbit(status_mirf,MAX_RT);						//	������� ���� "���������� ������� ��������"
		mirf_write_register(STATUS, status_mirf);		//	������� ���� "���������� ������� ��������"
		mirf_cmd(FLUSH_TX);								//	������� ����� ��������
	}
	mirf_clear();
	//	RTOS_SetTask(mirf_clear, 2, 0);
}