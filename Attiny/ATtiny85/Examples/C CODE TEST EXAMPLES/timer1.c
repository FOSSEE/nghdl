/*THIS IS THE C CODE TO TEST THE TIMER1 IN ATTTINY85 
AND THE EXAMPLES OF SQUARE WAVE GENERATOR AND
TRIANGULAR WAVE GENERATOR*/
//THIS CODE WAS DEVELOPED BY SUMANTO KAR
#include <avr/io.h>
#include <avr/sleep.h>
#include <math.h>
static void
pwm_set_duty(uint8_t duty)
{

    OCR1A = duty; // set the OCRnx
}
int main(void)
{    
      DDRB|=_BV(PB0);
      TIMSK=0xFF;
	
	unsigned char a=PINB&0x0C;
	unsigned char b=PINB&0x30;
	
	if(b==0x00)
		TCCR1=0x11;
	else if(b==0x10)
		TCCR1=0x12;
	else if(b==0x20)
		TCCR1=0x13;
	else if(b==0x30)
		TCCR1=0x14;
	if(a==0x00)
		OCR1A=1;
	 else if(a==0x04)
		OCR1A=63;
	 else if(a==0x08)
		OCR1A=128;
	 else if(a==0x0C)
		OCR1A=254;
	while(1)
	{};

}
/*//ANOTHER CODE TO TEST TIMER1
#include <avr/io.h>
#include <avr/sleep.h>
#include <math.h>
int main(void)
{    
      DDRB|=_BV(PB0);
      TIMSK=0xFF;
    //CTC MODE PRESCALAR=2    
    TCCR1=0x13;    //COM1A0=1 and CS11=1 REST ALL 0;
    OCR1A=254;    
    while(1)
    {
    }

}*/

