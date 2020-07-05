//Code by Ashutosh Jha


#include <avr/io.h>
#define F_CPU 2.0E6
#include<util/delay.h>

int main (void)
{
  // set PB0 to be output
	DDRB = 0x01;
  while (1) {
    // set PB0 high
    PORTB = 0x01;
    // delay loop
    _delay_ms(2);
    // set PB30low
    PORTB = 0x00;
    // delay loop
    _delay_ms(3);
  }
}
