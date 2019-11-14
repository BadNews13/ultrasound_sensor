
#include <avr/io.h>
#include <avr/interrupt.h>
#include "interrupt0.h"
#include "C:\Users\PC-013\Desktop\work\!�������\Ultrasound\MA40S4R\MA40S4R\measurement/measurement.h"


void Int0_ini(void)
{
	//���������� INT0 �� ����������� ������
	putch_usart(0x01);
	
	EICRA =	(0<<ISC11)|		//	3 bit: ������� ��� INT1
			(0<<ISC10)|		//	2 bit: ������� ��� INT1
			(0<<ISC01)|		//	1 bit: ������� ��� INT0			// ����������� �� ������������ �����
			(1<<ISC00);		//	0 bit: ������� ��� INT0			//				-//-
	
	EIMSK =	(0<<INT1)|		// ��������� ���������� �� INT1
			(1<<INT0);		// ��������� ���������� �� INT0
}



void Int0_disabled()
{
		
	EICRA =	(0<<ISC11)|		//	3 bit: ������� ��� INT1
			(0<<ISC10)|		//	2 bit: ������� ��� INT1
			(0<<ISC01)|		//	1 bit: ������� ��� INT0			// ����������� �� ������������ �����
			(0<<ISC00);		//	0 bit: ������� ��� INT0			//				-//-
	
	EIMSK =	(0<<INT1)|		//	��������� ���������� �� INT1
			(0<<INT0);		//	��������� ���������� �� INT0
}

ISR(INT0_vect)
{
	putch_usart(0xff);
	set_step_3();
	measurement();
}
