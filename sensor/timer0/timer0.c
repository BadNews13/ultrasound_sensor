#include "../../defines/defines_global.h"

#define  F_CPU 16000000UL

#include <avr/interrupt.h>
#include "timer0.h"
#include "../measurement/measurement.h"

#include "../../shell/cmd_interp.h"
#include "../../my_name/get_name/get_name.h"

uint8_t timer_mode = 0;			//	метка, показывает в каком режиме в запущен таймер
int 	tik = 0;				//	кол-во тиков таймера (не более 380 000/1024 = 371 дл€ измерени€ 4 метров до объекта)


	
void Timer0_ini_PWM(void)
{
//	“аймер дл€ генерации импульсов на PWM ноге								*
//	PWM PhaseCorrect mode													*
//	no prescaler															*
//	TOP = OCRA = 100 (0x64)													*
//	For 16Mhz: toggle OC0A = 160 000kHz (pulses = toggle OC0A / 4 = 40kHz)	*

//ѕеределал в CTC-mode т.к. дл€ OC0B отсутствует режим работы toggle в PWM-mode. если делать под один датчик, то можно вернуть PWM и использовать OC0A с последующей перенастройкой ноги дл€ компаратора

	TCCR0A =	(0<<COM0A1)|	//	7 bit: Compare Output Mode
				(1<<COM0A0)|	//	6 bit: Compare Output Mode		/1 toggle в PWM
				(0<<COM0B1)|	//	5 bit: Compare Output Mode		
				(0<<COM0B0)|	//	4 bit: Compare Output Mode		/1 clear/set
				(0<<WGM01)|		//	1 bit: Waveform Generation Mode
				(1<<WGM00);		//	0 bit: Waveform Generation Mode
	
	TCCR0B =	(0<<FOC0A)|		//	7 bit: Force Output Compare A
				(0<<FOC0B)|		//	6 bit: Force Output Compare B
				(1<<WGM02)|		//	3 bit: Waveform Generation Mode
				(0<<CS02)|		//	2 bit: Clock Select
				(0<<CS01)|		//	1 bit: Clock Select
				(1<<CS00);		//	0 bit: Clock Select
	
	TCNT0 =		0x00;			//	“екущее значение счетчика
	
	OCR0A =		100;			//	Output Compare Register A
	OCR0B = 	0;				//	Output Compare Register B
	
	TIFR0 =		(1<<OCF0B)|		//	2 bit: Output Compare Match Flag B	//	сбрасываем флаги
				(1<<OCF0A)|		//	1 bit: Output Compare Match Flag A
				(1<<TOV0);		//	0 bit: Overflow Flag
	
	TIMSK0 =	(0<<OCIE0B)|	//	2 bit: Output Compare Match B Interrupt Enable						//	прерывание при сравнении с B
				(1<<OCIE0A)|	//	1 bit: Output Compare Match A Interrupt Enable						//	прерывание при сравнении с A
				(0<<TOIE0);		//	0 bit: Overflow Interrupt Enable									//	прерывание по переполнению
	
//	TCCR0B |=	(0<<CS02)|(0<<CS01)|(1<<CS00);		// start

	timer_mode = PWM;
	tik = 0;
}

void Timer0_ini_CTC(void)
{
/*
“аймер дл€ отсчета времени (считает тики кварца)	*
CTC mode											*
prescaler 1/1024									*
верх таймера 0xFF									*
*/
	TCCR0A =	(0<<COM0A1)|	//	7 bit: Compare Output Mode
				(0<<COM0A0)|	//	6 bit: Compare Output Mode		/1
				(0<<COM0B1)|	//	5 bit: Compare Output Mode
				(0<<COM0B0)|	//	4 bit: Compare Output Mode
				(1<<WGM01)|		//	1 bit: Waveform Generation Mode
				(0<<WGM00);		//	0 bit: Waveform Generation Mode
		
	TCCR0B =	(0<<FOC0A)|		//	7 bit: Force Output Compare A
				(0<<FOC0B)|		//	6 bit: Force Output Compare B
				(0<<WGM02)|		//	3 bit: Waveform Generation Mode
				(1<<CS02)|		//	2 bit: Clock Select
				(0<<CS01)|		//	1 bit: Clock Select
				(1<<CS00);		//	0 bit: Clock Select
		
	TCNT0 =		0x00;			//	“екущее значение счетчика
		
	OCR0A =		0x01;			//	Output Compare Register A				//	значение до которого считает таймер
	OCR0B = 	0x00;			//	Output Compare Register B				//	значение до которого считает таймер
	
	TIFR0 =		(1<<OCF0B)|		//	2 bit: Output Compare Match Flag B
				(1<<OCF0A)|		//	1 bit: Output Compare Match Flag A
				(1<<TOV0);		//	0 bit: Overflow Flag
		
	TIMSK0 =	(0<<OCIE0B)|	//	2 bit: Output Compare Match B Interrupt Enable						//	прерывание при сравнении с B
				(1<<OCIE0A)|	//	1 bit: Output Compare Match A Interrupt Enable						//	прерывание при сравнении с A
				(0<<TOIE0);		//	0 bit: Overflow Interrupt Enable									//	прерывание по переполнению
				
//	TCCR0B |=	(1<<CS02)|(0<<CS01)|(1<<CS00);		// start
	
	timer_mode = CTC;
	tik = 0;
}

ISR(TIMER0_COMPA_vect)
{	
	switch (timer_mode)							//	в зависимости от того в каком режиме запущен таймер
	{
		case PWM:								//	режим генерации ультразвуковой волны
		{
			tik++;								//	счетчик будет увеличиватьс€ на 1 в каждом прерывании таймера (4,9 микросекунд)
			if (tik > 20)						//	ждем 10 импульсов
			{
				TIMER_0_STOP;					//	останавливаем таймер, который переключает пин OC0A
//				Timer0_stop();
				PORTD = (0<<PIND6);				//	пин OC0B вниз (чтобы пин не осталс€ в 1 при четном/нечетном количестве переключений)		
				Timer0_ini_CTC();				//	запускаем таймер в режиме CTC дл€ отсчета времени
				
				EIMSK =	(1<<INT1)|		// разрешить прерывание на INT1	(PD3)
						(1<<INT0);		// разрешить прерывание на INT0	(PD2)		//	от датчика
			}
			break;
		}
		case CTC:								//	режим отсчета времени
		{
			tik++;								//	увеличим количество тиков
			if(tik > 350)	{calculate_distance();}		//	ƒальше 4 метров мерить не об€зательно, потому что там точно земл€ (дл€ 4 метров - 387 878 тиков)
			break;
		}
	}
}

//	удалить если будет работать через TIMER_0_STOP
void Timer0_stop(void)
{
	TCCR0B =	(0<<CS02)|		//	2 bit: Clock Select
				(0<<CS01)|		//	1 bit: Clock Select
				(0<<CS00);		//	0 bit: Clock Select
				
	TIMSK0 =	(0<<OCIE0B)|	//	2 bit: Output Compare Match B Interrupt Enable						//	прерывание при сравнении с B
				(0<<OCIE0A)|	//	1 bit: Output Compare Match A Interrupt Enable						//	прерывание при сравнении с A
				(0<<TOIE0);		//	0 bit: Overflow Interrupt Enable									//	прерывание по переполнению

}

int get_tik(void)	{return tik;}	//	дл€ передачи значени€ текущего тика таймера в режиме отсчета времени
	