#include <avr/io.h>
//#define F_CPU 2000000
//#include <util/delay.h>

#define N_1    (_BV(CS00))

static void
pwm_init(void)
{
    DDRB |= _BV(PB0); // set PWM pin as OUTPUT
    TCCR0A |= _BV(WGM01)|_BV(WGM00); // set timer mode to FAST PWM
    TCCR0A |= _BV(COM0A1) |_BV(COM0A2); // connect PWM signal to pin in invering mode
}

/* When timer is set to Fast PWM Mode, the freqency can be
calculated using equation: F = F_CPU / (N * 256) 
In this sketch, N = 1 therfore, F = 1MHz/256 ~= 3.9kHz */
static void
pwm_set_frequency(uint32_t N)
{

    TCCR0B = (TCCR0B & ~((1<<CS02)|(1<<CS01)|(1<<CS00))) | N; // set prescaler
}

static void
pwm_set_duty(uint8_t duty)
{

    OCR0A = duty; // set the OCRnx
}

int
main(void)
{
    pwm_init();
    pwm_set_frequency(N_1);


    while (1) 
    {
    	unsigned char a = PINB;
    	if(a == 0x0)			//80% duty
			pwm_set_duty(51);
		else if(a == 0x02)		//60% duty
			pwm_set_duty(102);
		else if(a == 0x04)		//40% duty
			pwm_set_duty(153);
		else if(a == 0x06)		//20% duty
			pwm_set_duty(204);
    }
}