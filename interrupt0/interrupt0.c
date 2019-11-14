
#include <avr/io.h>
#include <avr/interrupt.h>
#include "interrupt0.h"
#include "C:\Users\PC-013\Desktop\work\!проекты\Ultrasound\MA40S4R\MA40S4R\measurement/measurement.h"

int int_state_up = 0;
int int_state_down = 0;

void Int0_ini_up(void)
{
	//прерывания INT0 по восходящему фронту
	
	int int_state_up = 1;
	
	EICRA =	(0<<ISC11)|		//	3 bit: событие для INT1
			(0<<ISC10)|		//	2 bit: событие для INT1
			(1<<ISC01)|		//	1 bit: событие для INT0			// реагировать на возрастающий фронт
			(1<<ISC00);		//	0 bit: событие для INT0			//				-//-
	
	EIMSK =	(0<<INT1)|		// разрешить прерывание на INT1
			(1<<INT0);		// разрешить прерывание на INT0
}

void Int0_ini_down(void)
{
	//прерывания INT0 по низходящему фронту
	
	int_state_down = 1;		//	флаг - прерывание по низходящему фронту
	
	EICRA =	(0<<ISC11)|		//	3 bit: событие для INT1
			(0<<ISC10)|		//	2 bit: событие для INT1
			(1<<ISC01)|		//	1 bit: событие для INT0			// реагировать на низходящий фронт
			(0<<ISC00);		//	0 bit: событие для INT0			//				-//-
	
	EIMSK =	(0<<INT1)|		//	разрешить прерывание на INT1
			(1<<INT0);		//	разрешить прерывание на INT0
}


void Int0_disabled()
{
	EICRA =	(0<<ISC11)|		//	3 bit: событие для INT1
			(0<<ISC10)|		//	2 bit: событие для INT1
			(0<<ISC01)|		//	1 bit: событие для INT0			// реагировать на возрастающий фронт
			(0<<ISC00);		//	0 bit: событие для INT0			//				-//-
	
	EIMSK =	(0<<INT1)|		//	разрешить прерывание на INT1
			(0<<INT0);		//	разрешить прерывание на INT0
}

ISR(INT0_vect)
{
	set_step_3();
	measurement();
}

void change_interrup(void)
{
	if (int_state_down = 1)
	{
		Int0_ini_up();
		int_state_down = 0;
		return;
	}
	if (int_state_up = 1)
	{
		Int0_ini_down();
		int_state_up = 0;
		return;
	}
}