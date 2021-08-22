//Code written by Ashutosh Jha

void main()
{
	DDRB = 0x08;
	//All pins are set as output
	while(1)
	{
		PORTB = 0x01;
		//Set pb0 as HIGH
		PORTB = 0x00;
		//Set pb0 as LOw
	}
}
