/* This C Code for ATTINY series  was developed by ASHUTOSH JHA
	Modified by Sumanto Kar for ATTINY85

				****!!! LATEST EDIT - 12:12 PM, 22/5/2020 by AJ ****!!! 

	1. Combined instructions given by SM,VY,SK
	2. Following instructions were having clashes with other instructions -
		i)		ROL with ADC Rd,Rd
		ii)		LSL with ADD Rd,Rd
		iii)	CLR with EOR Rd,Rd
		iv)		TST with AND Rd,Rd
		v)		BSET with SEC,SEH,SEN,SEV,SES,SET,SEI,SEZ
		vi)		BCLR with CEC,CEH,CEN,CEV,CES,CET,CEI,CEZ
		vii)	BRBC with BRCC,BRGE,BRHC,BRID,BRNE,BRPL,BRTC,BRVC
		viii)	BRBS with BRCS,BREQ,BRHS,BRIE,BRLT,BRMI,BRTS,BRVS	

		For the above, instruction which is more universal is continued while the other is deprecated for use
		(eg- ADC is used and contains ROL, ROL not defined separately)
	3. Some instructions were exactly the same as others (opcode as well as logical function) -
		i) 		BRLO == BRCS
		ii)		BRSH == BRCC
		iii)	CBR  == ANDI

		For the above, BRCS,BRCC, ANDI are used and BRLO,BRSH,CBR are deprecated for use. 

   **NOTE	:-	The functions "MapToRam" and "output" &
   				the variables PB0 ... PB5 
   				linked to the VHDL code of ATTINY85
   				by "ghdl_access.vhdl" file.	*/ 

   // CODE WITH COMMENTS AND TIMER0 and TIMER 1.

#include<stdio.h>
#include<math.h>
#include<string.h>
#define size 4096		//4kb program memory for attiny85

int debugMode=1;
int PB0,PB1,PB2,PB3,PB4,PB5,wait_Clocks=0;
int PC = 0;                      // changed PC to int
int SP = 512;                	//IOREG[29]+IOREG[30]*256;
struct memory					//Structure to store RAM and other registers
{
	unsigned char data;
}prog_mem[size],GPR[32],SREG[8],IOREG[64],SRAM[512];

/* SREG MAP :- 

| C | Z | N | V | S | H | T | I |
  0   1   2   3   4   5   6   7 

*/

struct BinArrays
{
    int arr[8];
}bin[3];

int * get_ptr0() {
  return &PB0;
}

int * get_ptr1() {
  return &PB1;
}

int * get_ptr2() {
  return &PB2;
}

int * get_ptr3() {
  return &PB3;
}

int * get_ptr4() {
  return &PB4;
}

int * get_ptr5() {
  return &PB5;
}

int * wait_cycles() {
  return &wait_Clocks;
}

void ClearBins(int binSel)
{
    int i;
        for(i=0;i<8;i++)
    	{
        	bin[binSel].arr[i]=0;
    	}
}

void SetRam(int lb, int ub,char val)
{
	int i;
	for(i=lb;i<ub;i++)
		prog_mem[i].data = val;
	for(i=0;i<64;i++)
		IOREG[i].data = val;
}

void PrintSREG()
{
	printf("\nStatus Register:- \n");
	printf("I: %d ,T: %d ,H: %d ,S: %d ,V: %d ,N: %d ,Z: %d ,C: %d \n",SREG[7].data,SREG[6].data,SREG[5].data,SREG[4].data,SREG[3].data,SREG[2].data,SREG[1].data,SREG[0].data);

}

void PrintRam(int lb, int ub)
{
	int i=0;
	unsigned char b1,b2;
	printf("\n***********RAM*************\n");
	for(i=lb;i<ub;i+=2)
		{
			b1=prog_mem[i+0x1].data;b2=prog_mem[i].data;
			printf("\nRAM[%X]: %X%X\n",i,b1,b2);
		}
	printf("\n************************\n");
}

void PrintReg(int lb, int ub)
{
	int i;
	printf("\n***********Register File*************\n");
	for(i=lb;i<ub;i++)
		printf("R[%d]: %X\n",i,GPR[i].data);
	printf("\n************************\n");
}

void Hex2Bin(int binSel,int hex)			//Function to convert hex number to binary array
{
        int i=0,a,b,t=hex;
        ClearBins(binSel);
	    while(hex!=0 && i<8)
	    {
	        bin[binSel].arr[i] = hex % 2;
	        i++;
	        hex /= 2;
	    }
	    
}


void TwosComp(int binSel)			//Function to get 2's complement
{

    int i,t,carry=0;
    for(i=0;i<8;i++)
    bin[binSel].arr[i] = !bin[binSel].arr[i];

    for(i=0;i<8;i++)
    {
        if(i==0)
            {
                t = carry + bin[binSel].arr[i] + 1;
                bin[binSel].arr[i] = bin[binSel].arr[i] ^ carry ^ 1;
            }
        else
            {
                t = carry + bin[binSel].arr[i];
                bin[binSel].arr[i] = bin[binSel].arr[i] ^ carry;
            }
        if(t<=1)
            carry = 0;
        else
            carry = 1;
    }
    
}

void Bin_Add(int a, int b, int c,int select,int withCarry)			//Function to perform binary addition
{
    int i,t;
    for(i=0;i<8;i++)
	    {
	    	if(i==0 && withCarry == 0)
	    	{
	    		t = bin[b].arr[i] + bin[a].arr[i];
	        	bin[c].arr[i] = bin[a].arr[i] ^ bin[b].arr[i];	
	    	}
	    	else
	    	{
	    		t = bin[b].arr[i] + bin[a].arr[i] + SREG[0].data;
	        	bin[c].arr[i] = bin[a].arr[i] ^ bin[b].arr[i] ^ SREG[0].data;
	    	}
	        
	        if(t<=1)
	        {
	            SREG[0].data = 0;
	            printf("\nCarry bit is reset\n");
	        }
	        else
	        {
	        	SREG[0].data = 1;
	        	printf("\nCarry bit is set\n");
	        	if(i==3)
	        		{
	        			SREG[5].data = 1;
	        			printf("\nHalf Carry bit is set\n");
	        		}
	        	else if(i==7)
	        		{
	        			SREG[2].data = 0;
	        			//printf("\nNegative bit is reset\n");
	        		}
	        }
	    }
	    
	int t0=bin[c].arr[0],t1=bin[c].arr[1],t2=bin[c].arr[2],t3=bin[c].arr[3];
	for(i=0;i<4;i++)
	{
	    bin[c].arr[i]=bin[c].arr[4+i];
	}

	bin[c].arr[4]=t0;
	bin[c].arr[5]=t1;
	bin[c].arr[6]=t2;
	bin[c].arr[7]=t3;
	    
	    if(select == 1)
	    {
	        SREG[5].data = !SREG[5].data;
	        SREG[2].data = !SREG[0].data;
	        SREG[0].data = !SREG[0].data;
	    }
}



void SetPins(int flag)			//Function to set/reset the I/O pins
{
	if(flag == 1)
	{
		int val = IOREG[0x18].data;
		int bin[6]={0,0,0,0,0,0},i=0; 
	    while(val!=0)
	    {
	        bin[i] = val % 2;
	        i++;
	        val /= 2;
	    }

	    PB0 = bin[0];
	    PB1 = bin[1];
	    PB2 = bin[2];
	    PB3 = bin[3];
	    PB4 = bin[4];
	    PB5 = bin[5];
	}
	

}

void MapToRam(int flag)				//Function to map the external hex file contents into this C code
{
	int i=0,filesize,j,s,line,adr=0,lineCount=0;
	unsigned char c,temp;
	SetRam(0,size,0x0);
	if(flag==1)
	{
		FILE *fptr;
		char * line = NULL;
    	size_t len = 0;
    	ssize_t read;
    	fptr = fopen("hex.txt", "r");	
		while ((read = getline(&line, &len, fptr)) != -1) 
		{
	        printf("\nLine: %s\n", line);
	        for(i=9;i<strlen(line)-4;i++)
	        	{
	        		c = line[i];
			    	if(c>=48 && c<=57)
			        	c-=48;
			    	else if(c>=65 && c<=70)
			       		c-=55;
			    	else if(c>=97 && c<=102)
			        	c-=87;
			        temp = c * 16;
			        i++;
			        c = line[i];
			    	if(c>=48 && c<=57)
			        	c-=48;
			    	else if(c>=65 && c<=70)
			       		c-=55;
			    	else if(c>=97 && c<=102)
			        	c-=87;
			        c += temp;
			        prog_mem[adr].data = c;
			        printf("\nRam[%X]: %X",adr,c);
			        adr++;
	        	}
	        	lineCount++;
    	}
		
	}
	for(i=0;i<8;i++)
		SREG[i].data = 0;

	if(debugMode==1)
		PrintRam(0,lineCount*16-5);
}

void UpdateSreg()			//Function to update Zero and Signed bit of SREG
{
	int i,t=0;
	for(i=0;i<8;i++)
	{
	    if(bin[2].arr[i]==0)
	        	t++;
	}
	if(t == 8)
	{
	    SREG[1].data = 1;
	    //printf("\nZero bit is set\n");	
	}
	else
	{
	    SREG[1].data = 0;
	    //printf("\nZero bit is set\n");	
	}

	SREG[4].data = SREG[2].data ^ SREG[3].data;
	//Signed bit is exor of Negative and Overflow bits
	if(SREG[4].data == 1)
	{
		//printf("\nSigned bit is set\n");
	}
	else
	{
		//printf("\nSigned bit is reset\n");
	}

}

void input(int in_Val)
{
	IOREG[0x16].data = in_Val;
	if(debugMode == 1)
		printf("\nPINB = %X\n",IOREG[0x16].data);
}

void timer(int flag)
{
// TIMER0 by AJ		03/05/20 edited by SK,SM and VY
	unsigned char temp = 0x0;static unsigned char bit=0x00;
    if(IOREG[0x33].data >= 0x01)	
    {
       if(debugMode == 1)
			     printf("\n**!!TIMER 0 operation detected!!**\n");
	     Hex2Bin(3,IOREG[0x2A].data);
         Hex2Bin(2,IOREG[0x33].data);
         //for normal mode by vy
       if((bin[3].arr[0] == 0 && bin[3].arr[1]==0 && bin[2].arr[4]==0) || (bin[3].arr[6] == 0 && bin[3].arr[7]==0))
       {
        printf("\n**normal mode!!**\n");
        temp = IOREG[0x38].data & 0x02; //TIFR overflow
		if(temp == 0x02)
			{
				printf("\n**!!TOV0 overflow!!**\n");
				IOREG[0x32].data = 0; //counter made 0 coz of overflow
				IOREG[0x38].data = IOREG[0x38].data ^ 0x02;
			}
		//incrementing
         if(IOREG[0x32].data < 255)
            {
			IOREG[0x32].data += 0x01;
            IOREG[0x18].data = IOREG[0x18].data ^ 0x01;//for testing
			printf("\nPORTB: %X\n",IOREG[0x18].data);
            }
		 else if(IOREG[0x32].data == 255)
			{
				IOREG[0x38].data = IOREG[0x38].data | 0x02;
				printf("\nTOV0 set, TIFR: %X\n",IOREG[0x38].data);//setting TIFR overflow
			}
         printf("\nTCCNT0 : %X ",IOREG[0x32].data);
		 SetPins(1);
        }
                //normal mode ended
	if(IOREG[0x2A].data >= 0x01) //TCCR0A
	{
	  if((bin[3].arr[0] == 1 && bin[3].arr[1]==1) || (bin[3].arr[0] == 0 && bin[3].arr[1]==1))//following code is for fast pwm or ctc mode
	  {
		//If TOV0 is set, reset counter TCNT0
		temp = IOREG[0x38].data & 0x02; //TIFR overflow
		if(temp == 0x02)
			{
				printf("\n**!!TOV0 overflow!!**\n");
				IOREG[0x32].data = 0; //counter made 0 coz of overflow
				IOREG[0x38].data = IOREG[0x38].data ^ 0x02;
				printf("\nTIFR after toggling TOV0: %X\n",IOREG[0x38].data);
				IOREG[0x38].data = IOREG[0x38].data & 0xEF;
				printf("\nTIFR after resetting OCF0A: %X\n",IOREG[0x38].data);
			}
                //fast pwm mode
        int FastMode=0,OC0A_Con=0;
	   if(bin[3].arr[0] == 1 && bin[3].arr[1]==1) // w0 w1=11,fast pwm mode
	        {
			FastMode = 1;//to check mode
            printf("\n**!!FAST PWM!!**\n");//testing
            
         if(bin[3].arr[7] == 1 && bin[3].arr[6] == 0)
			OC0A_Con = 1;//non inverted
		 else
			OC0A_Con = 0;
		 temp = IOREG[0x38].data & 0xC0;
		// For non-inverting operation
		 if(FastMode==1 && temp == 0xC0)
			IOREG[0x38].data = IOREG[0x38].data | 0x10;

		// Incrementing counter TCNT0
		 if(IOREG[0x32].data < 255)
			IOREG[0x32].data += 0x01;
		 else if(IOREG[0x32].data == 255)
			{
				IOREG[0x38].data = IOREG[0x38].data | 0x02;
				printf("\nTOV0 set, TIFR: %X\n",IOREG[0x38].data);
			}
		// Setting Timer0 overflow flag == 0

		// IF TCNT0 == OCR0A
		 if(IOREG[0x32].data == IOREG[0x29].data)
			{
				IOREG[0x38].data = IOREG[0x38].data | 0x10;
				printf("\n**!!TCNT0 = OCR0A!!**\n");
				printf("\nTIFR: %X\n",IOREG[0x38].data);
			}

		 temp = IOREG[0x38].data & 0x10;
		 if(OC0A_Con == 1 && temp == 0x10)
			{
				IOREG[0x18].data = IOREG[0x18].data | 0x01;
				printf("\nPORTB: %X\n",IOREG[0x18].data);
			}
		 else if(OC0A_Con == 1 && temp != 0x10)
			IOREG[0x18].data = IOREG[0x18].data & 0xFE;
            }
		//else
			//FastMode = 0;
	    //CTC mode by SK
		int CTCMode=0,COM0A_Con=0;
     if(bin[3].arr[1] == 1 && bin[3].arr[0]==0)
     {
            CTCMode = 1;
            printf("\n**!!CTC mode!!**\n");//testing
            temp = IOREG[0x38].data & 0x10;
        if(temp == 0x10)
            {
                IOREG[0x38].data = IOREG[0x38].data & 0xEF;
                printf("\nTIFR after resetting OCF0A: %X\n",IOREG[0x38].data);
            }
        if(bin[3].arr[7] == 0 && bin[3].arr[6] == 0)
            COM0A_Con = 0;
        else if(bin[3].arr[7] == 0 && bin[3].arr[6] == 1)
            COM0A_Con = 1;
        else if(bin[3].arr[7] == 1 && bin[3].arr[6] == 0)
            COM0A_Con = 2;
        else if(bin[3].arr[7] == 1 && bin[3].arr[6] == 1)
            COM0A_Con = 3;


        // Incrementing counter TCNT0
        if(CTCMode==1)
        {
        
        if(IOREG[0x32].data == IOREG[0x29].data)
            {    IOREG[0x32].data = 0x0;
                IOREG[0x38].data = IOREG[0x38].data | 0x10;
                printf("\n**!!TCNT0 = OCR0A!!**\n");
                printf("\nOC0A set, TIFR: %X\n",IOREG[0x38].data);
                bit=~bit;
            }
        else
            IOREG[0x32].data += 0x01;
        // Setting Timer0 overflow flag == 0
        if(IOREG[0x32].data == 256)
            {    
                IOREG[0x38].data = IOREG[0x38].data  | 0x02;
                printf("\nTOV0 set, TIFR: %X\n",IOREG[0x38].data);
            
            }
        if(COM0A_Con == 1)
            {    
                if(bit==0xFF)
                IOREG[0x18].data = IOREG[0x18].data | 0x01;
                else
                IOREG[0x18].data = IOREG[0x18].data & 0xFE;
                printf("\nPORTB: %X\n",IOREG[0x18].data);
            }
        else if(COM0A_Con == 2)
            {    if(bit==0xFF)
                IOREG[0x18].data = IOREG[0x18].data & 0xFE;
                printf("\nPORTB: %X\n",IOREG[0x18].data);
            }
        else if(COM0A_Con == 3)
            {    if(bit==0xFF)
                IOREG[0x18].data = IOREG[0x18].data | 0x01;
                printf("\nPORTB: %X\n",IOREG[0x18].data);
            }
        
        }
            }
        else
            CTCMode = 0;
       } 
       //Phase correct PWM by SM
       char top;
       char k=IOREG[0x2A].data &0x01;
       int PhaseCorrectPWM=0,OC0A_Con = 0;
     if(bin[3].arr[1] == 0 && bin[3].arr[0]==1)
     {
			PhaseCorrectPWM = 1;
			printf("PhaseCorrectPWM mode");
		    if(bin[3].arr[7] == 1 && bin[3].arr[6] == 0)		//non inverting
			OC0A_Con = 1;
		else if (bin[3].arr[7] == 1 && bin[3].arr[6] == 1)	// inverting
			OC0A_Con = 0;
		else 
			printf("Not a Timer mode");

		if (IOREG[0x33].data & 0x08 ==0x08)				//33reg -TCCR0B, to check WGM02 Set
			top= IOREG[0x29].data;
		else
			top=0xFF;
		IOREG[0x18].data = IOREG[0x18].data | 0x01;	

		if (IOREG[0X32].data == 0xFF)					//reset TOV0
			IOREG[0x38].data = IOREG[0x38].data & 0xFD;			

		// Decrmenting counter TCNT0
			k=IOREG[0x38].data & 0x02;
		if(IOREG[0x32].data > 0 && k==0x00)		//TOV is reset
			IOREG[0x32].data -= 0x01;
		else if(IOREG[0x32].data == 0)
			{
				IOREG[0x38].data = IOREG[0x38].data | 0x02; 				//TOV0 set
				printf("\nTOV0 set, TIFR: %X\n",IOREG[0x38].data);
				IOREG[0x38].data = IOREG[0x38].data & 0xEF;						//OCF0 reset
				printf("\nTIFR after resetting OCF0A: %X\n",IOREG[0x38].data);				
			}			

		//If TOV0 is set, increment TCNT0
		temp = IOREG[0x38].data & 0x02;
		if(temp == 0x02 && IOREG[0X32].data < 0xFF)
			{
					IOREG[0x32].data += 0x01;
			}

		// IF TCNT0 == OCR0A
		if(IOREG[0x32].data == IOREG[0x29].data)
			{
				IOREG[0x38].data = IOREG[0x38].data | 0x10;
				printf("\n**!!TCNT0 = OCR0A!!**\n");
				printf("\nTIFR: %X\n",IOREG[0x38].data);
			}
    
    	k= IOREG[0X38].data & 0x02;
		temp = IOREG[0x38].data & 0x10;  		//match condition
		if(temp == 0x10 && k== 0x02)								// TOV is set...incre
			{		
				if (OC0A_Con == 1){
					IOREG[0x18].data = IOREG[0x18].data & 0xFE;				// reset OCF0
					printf("\nPORTB: %X\n",IOREG[0x18].data);
				}
				else{
					IOREG[0x18].data = IOREG[0x18].data | 0x01 ;				// reset OCF0
					printf("\nPORTB: %X\n",IOREG[0x18].data);
				}
			}
		else if(temp != 0x10 && k != 0x02){							// Tov is reset...dec
			if (OC0A_Con == 0){
				IOREG[0x18].data = IOREG[0x18].data | 0x01;					// set OCF0
			}
			else
				IOREG[0x18].data = IOREG[0x18].data & 0xFE;
		}
	}
		else
			PhaseCorrectPWM = 0;
		printf("\nTCCNT0 : %X ,OCR0A: %X\n",IOREG[0x32].data,IOREG[0x29].data);
		SetPins(1);

	}
    }
}



void timer1(int flag)
{   
//    TIMER 1 OPERATION BY SK        02/06/2020
    char temp = 0x0; static unsigned int count=0;                        
    if(IOREG[0x2E].data >= 0x01 )//0x2E=OCR1A
    {    int COM1A_Con=0,mode=0, CS_Con = 0;
        
        if(debugMode == 1)
            printf("\n**!!TIMER 1 operation detected!!**\n");
        
        Hex2Bin(3,IOREG[0x30].data);
        CS_Con=IOREG[0X30].data & 0x0F;                //Masking the CS bits to get the clocking modes
        mode=(int)pow(2,(int)CS_Con-1);                //Refer Timer/Counter1 Prescale Select in datasheet
        if(CS_Con!=0)
        {    printf("\nCLOCKING MODE: %X\n",CS_Con);
            IOREG[0x27].data=IOREG[0x27].data|0x01;            //Enabling the Plock bit in PLLCSR register
            temp = IOREG[0x27].data & 0x07;                //Checking whether Asynchronous mode is enabled or not
            if(temp==0x07)
                printf("Timer1 is in Asynchronous Mode");
            else
                printf("Timer1 is in Synchronous Mode");
            //If TOV1 is set, reset counter TCNT1
            temp = IOREG[0x38].data & 0x04;                //Checking the timer overflow
            if(temp == 0x04)
            {    
                printf("\n**!!TOV1 overflow!!**\n");
                IOREG[0x2F].data = 0;
                IOREG[0x38].data = IOREG[0x38].data ^ 0x04;
                printf("\nTIFR after toggling TOV1: %X\n",IOREG[0x38].data);
                IOREG[0x38].data = IOREG[0x38].data & 0xBF;
                printf("\nTIFR after resetting OCF1A: %X\n",IOREG[0x38].data);
            }

    
            if(bin[3].arr[5] == 0 && bin[3].arr[4] == 0)        //Checking the compare match options
                COM1A_Con = 0;
            else if(bin[3].arr[5] == 0 && bin[3].arr[4] == 1)
                COM1A_Con = 1;
            else if(bin[3].arr[5] == 1 && bin[3].arr[4] == 0)
                COM1A_Con = 2;
            else if(bin[3].arr[5] == 1 && bin[3].arr[4] == 1)
                COM1A_Con = 3;

            temp = IOREG[0x38].data & 0xC0;                // For non-inverting operation        
            if(temp == 0xC0)
                IOREG[0x38].data = IOREG[0x38].data | 0x40;    //Setting OC1A bit
        
            // Incrementing counter TCNT1
            if(count==mode)                        //counter to divide the frequency according to the mode
            {    count=0;
                temp = IOREG[0x39].data & 0x04;
                if(IOREG[0x2F].data < 255)
                IOREG[0x2F].data += 0x01;
                else if(IOREG[0x2F].data == 255 && temp==0x04)    //IF TCNT1 == 255 and TOIE1 is set
                {    
                    IOREG[0x38].data = IOREG[0x38].data | 0x04;
                    printf("\nTOV1 set, TIFR: %X\n",IOREG[0x38].data);
                }
            }
            else
                count++;                    //incrementing the counter

            // Setting Timer1 overflow flag == 0
            temp=IOREG[0x39].data & 0x40;                //Checking OCIE1A in TIMSK register
        
            if(IOREG[0x2F].data == IOREG[0x2E].data && temp==0x40)    // IF TCNT1 == OCR1A and OCIE1A is set
            {    
                IOREG[0x38].data = IOREG[0x38].data | 0x40;
                printf("\n**!!TCNT1 = OCR1A!!**\n");
                printf("\nTIFR: %X\n",IOREG[0x38].data);
            }

            temp = IOREG[0x38].data & 0x40;

            if(COM1A_Con == 1 )                    //Checking the Compare match Modes
            {
                if(temp==0x40)
                    IOREG[0x18].data = IOREG[0x18].data | 0x01;
                else if(temp!=0x40)
                    IOREG[0x18].data = IOREG[0x18].data & 0xFE;
                printf("\nPORTB: %X\n",IOREG[0x18].data);
            }
            else if(COM1A_Con == 2 )
            {
                if(temp == 0x40)
                    IOREG[0x18].data = IOREG[0x18].data & 0xFE;
            }
            else if(COM1A_Con == 3 )
            {
                if(temp == 0x40)
                    IOREG[0x18].data = IOREG[0x18].data | 0x01;
            }

            printf("\nTCCNT1 : %X ,OCR1A: %X\n",IOREG[0x2F].data,IOREG[0x2E].data);
            SetPins(1);
        }
        else
            printf("\nCLOCKING MODE: %X , TIMER STOPPED\n",CS_Con);    //Stopping timer in mode 1
        
    }
}


void Compute()			//Function that performs main computation based on current instruction
{
	int i,j,t;
	unsigned a1=prog_mem[PC+0x1].data,a2=prog_mem[PC].data,b1,b2,b3,b4;
	b1 = a1/16;
	b2 = a1%16;
	b3 = a2/16;
	b4 = a2%16;
	if (debugMode==1)
		printf("instruction:%X%X%X%X\n",b1,b2,b3,b4);

//	ADD by AJ		10/05/20
	if(b1==0x0 && b2>=12 && b2<=15)
	{	
		int dbits[5],rbits[5],Rd=0,Rr=0;
		//For finding Rd and Rr
		Hex2Bin(0,b2);
		Hex2Bin(1,b3);
		Hex2Bin(2,b4);
		rbits[4] = bin[0].arr[1];
		dbits[4] = bin[0].arr[0];
		for(i=0;i<4;i++)
		{
			dbits[i] = bin[1].arr[i];
			rbits[i] = bin[2].arr[i];
		}
		for(i=0;i<5;i++)
			{
				Rd += dbits[i]*pow(2,i);
				Rr += rbits[i]*pow(2,i);
			}
		if(Rd == Rr && debugMode==1)
			printf("\nLSL instruction decoded\n");		//LSL = ADD Rd,Rd
		else if(Rd != Rr && debugMode==1)
			printf("\nADD instruction decoded\n");

		if(debugMode == 1)
			printf("\nBefore execution\nReg[%d] = %X\nReg[%d] = %X\n",Rd,GPR[Rd].data,Rr,GPR[Rr].data);			
		//For finding sum
		Hex2Bin(0,GPR[Rd].data);
		Hex2Bin(1,GPR[Rr].data);
		Hex2Bin(2,GPR[Rd].data+GPR[Rr].data);

		//For setting SREG bits
		//For setting Carry flag bit
		SREG[0].data = (bin[0].arr[7]&bin[1].arr[7]) | (bin[1].arr[7]&!bin[2].arr[7]) |
						(!bin[2].arr[7]&bin[0].arr[7]);
		//For setting Zero flag bit
		SREG[1].data = !bin[2].arr[0] & !bin[2].arr[1] & !bin[2].arr[2] & !bin[2].arr[3] &
						!bin[2].arr[4] & !bin[2].arr[5] & !bin[2].arr[6] & !bin[2].arr[7];
		//For setting Negative flag bit
		SREG[2].data = bin[2].arr[7];
		//For setting Overflow flag bit
		SREG[3].data = (bin[0].arr[7]&bin[1].arr[7]&!bin[2].arr[7]) |
						(!bin[0].arr[7]&!bin[1].arr[7]&bin[2].arr[7]);
		//For setting Signed bit
		SREG[4].data = SREG[2].data ^ SREG[3].data;
		//For setting Half Carry flag bit
		SREG[5].data = (bin[0].arr[3]&bin[1].arr[3]) | (bin[1].arr[3]&!bin[2].arr[3]) |
						(!bin[2].arr[3]&bin[0].arr[3]);

		//Rd = Rd + Rr
		GPR[Rd].data += GPR[Rr].data;

		if(debugMode==1)
			printf("\nAfter execution\nReg[%d] = %X\nReg[%d] = %X\n",Rd,GPR[Rd].data,Rr,GPR[Rr].data);

		ClearBins(0); ClearBins(1); ClearBins(2);			

		PC += 0x2;
	}

/************************************************************************************************/	
//	ADC by AJ		10/05/20
	else if(b1==0x1 && b2>=12 && b2<=15)
	{
		
		int dbits[5],rbits[5],Rd=0,Rr=0;
		//For finding Rd and Rr
		Hex2Bin(0,b2);
		Hex2Bin(1,b3);
		Hex2Bin(2,b4);
		rbits[4] = bin[0].arr[1];
		dbits[4] = bin[0].arr[0];
		for(i=0;i<4;i++)
		{
			dbits[i] = bin[1].arr[i];
			rbits[i] = bin[2].arr[i];
		}
		for(i=0;i<5;i++)
			{
				Rd += dbits[i]*pow(2,i);
				Rr += rbits[i]*pow(2,i);
			}
		if(Rd == Rr && debugMode==1)
			printf("\nROL instruction decoded\n");		//ROL = ADC Rd,Rd
		else if(Rd != Rr && debugMode==1)
			printf("\nADC instruction decoded\n");

		if(debugMode == 1)
			printf("\nBefore execution\nReg[%d] = %X\nReg[%d] = %X\n",Rd,GPR[Rd].data,Rr,GPR[Rr].data);			
		//For finding sum
		Hex2Bin(0,GPR[Rd].data);
		Hex2Bin(1,GPR[Rr].data);
		Hex2Bin(2,GPR[Rd].data + GPR[Rr].data + SREG[0].data);

		//Rd = Rd + Rr + Carry flag bit (SREG[0])
		GPR[Rd].data += GPR[Rr].data + SREG[0].data;

		//For setting SREG bits
		//For setting Carry flag bit
		SREG[0].data = (bin[0].arr[7]&bin[1].arr[7]) | (bin[1].arr[7]&!bin[2].arr[7]) |
						(!bin[2].arr[7]&bin[0].arr[7]);
		//For setting Zero flag bit
		SREG[1].data = !bin[2].arr[0] & !bin[2].arr[1] & !bin[2].arr[2] & !bin[2].arr[3] &
						!bin[2].arr[4] & !bin[2].arr[5] & !bin[2].arr[6] & !bin[2].arr[7];
		//For setting Negative flag bit
		SREG[2].data = bin[2].arr[7];
		//For setting Overflow flag bit
		SREG[3].data = (bin[0].arr[7]&bin[1].arr[7]&!bin[2].arr[7]) |
						(!bin[0].arr[7]&!bin[1].arr[7]&bin[2].arr[7]);
		//For setting Signed bit
		SREG[4].data = SREG[2].data ^ SREG[3].data;
		//For setting Half Carry flag bit
		SREG[5].data = (bin[0].arr[3]&bin[1].arr[3]) | (bin[1].arr[3]&!bin[2].arr[3]) |
						(!bin[2].arr[3]&bin[0].arr[3]);

		if(debugMode==1)
			printf("\nAfter execution\nReg[%d] = %X\nReg[%d] = %X\n",Rd,GPR[Rd].data,Rr,GPR[Rr].data);

		ClearBins(0); ClearBins(1); ClearBins(2);			

		PC += 0x2;
	}

/************************************************************************************************/
//	SUB by AJ		10/05/20
	else if(b1==0x1 && b2 >= 0x08 && b2 <= 0x0B)
	{
		if(debugMode==1)
			printf("\nSUB instruction decoded\n");
		int dbits[5],rbits[5],Rd=0,Rr=0;
		//For finding Rd and Rr
		Hex2Bin(0,b2);
		Hex2Bin(1,b3);
		Hex2Bin(2,b4);
		rbits[4] = bin[0].arr[1];
		dbits[4] = bin[0].arr[0];
		for(i=0;i<4;i++)
		{
			dbits[i] = bin[1].arr[i];
			rbits[i] = bin[2].arr[i];
		}
		for(i=0;i<5;i++)
			{
				Rd += dbits[i]*pow(2,i);
				Rr += rbits[i]*pow(2,i);
			}
		ClearBins(0); ClearBins(1); ClearBins(2);
		if(debugMode == 1)
			printf("\nBefore execution\nReg[%d] = %X\nReg[%d] = %X\n",Rd,GPR[Rd].data,Rr,GPR[Rr].data);			
		//For finding difference
		Hex2Bin(0,GPR[Rd].data);
		Hex2Bin(1,GPR[Rr].data);
		//Rd = Rd - Rr
		GPR[Rd].data -= GPR[Rr].data;
		Hex2Bin(2,GPR[Rd].data);

		//For setting SREG
		//For setting Carry flag bit
		SREG[0].data = (!bin[0].arr[7]&bin[1].arr[7]) | (bin[1].arr[7]&bin[2].arr[7]) |
						(bin[2].arr[7]&!bin[0].arr[7]);
		//For setting Zero flag bit
		SREG[1].data = !bin[2].arr[0] & !bin[2].arr[1] & !bin[2].arr[2] & !bin[2].arr[3] &
						!bin[2].arr[4] & !bin[2].arr[5] & !bin[2].arr[6] & !bin[2].arr[7];
		//For setting Negative flag bit
		SREG[2].data = bin[2].arr[7];
		//For setting Overflow flag bit
		SREG[3].data = (bin[0].arr[7]&!bin[1].arr[7]&!bin[2].arr[7]) |
						(!bin[0].arr[7]&bin[1].arr[7]&bin[2].arr[7]);
		//For setting Signed bit
		SREG[4].data = SREG[2].data ^ SREG[3].data;
		//For setting Half Carry flag bit
		SREG[5].data = (!bin[0].arr[3]&bin[1].arr[3]) | (bin[1].arr[3]&bin[2].arr[3]) |
						(bin[2].arr[3]&!bin[0].arr[3]);

		if(debugMode==1)
			printf("\nAfter execution\nReg[%d] = %X\nReg[%d] = %X\n",Rd,GPR[Rd].data,Rr,GPR[Rr].data);

		ClearBins(0); ClearBins(1); ClearBins(2);			

		PC += 0x2;
	}

/************************************************************************************************/
//	SUBI by AJ		10/05/20
	else if(b1==0x5)
	{
		if(debugMode==1)
			printf("\nSUBI instruction decoded\n");
		int dbits[4],Kbits[8],Rd=0;
		unsigned char K=0x0;
		//For finding Rd and K
		Hex2Bin(0,b2);
		printf("\n");	
		Hex2Bin(1,b3);
		Hex2Bin(2,b4);
		for(i=0;i<4;i++)
			dbits[i] = bin[1].arr[i];
		for(i=0;i<4;i++)
			{
				Kbits[i] = bin[2].arr[i];
				Kbits[i+4] = bin[0].arr[i];
			}
		for(i=0;i<4;i++)
				Rd += dbits[i]*pow(2,i);
		for(i=0;i<8;i++)
				K += Kbits[i]*pow(2,i);
		//Since this SUBI can address R16 to R32
			Rd += 16;
		ClearBins(0); ClearBins(1); ClearBins(2);

		if(debugMode == 1)
			printf("\nBefore execution\nReg[%d] = %X\nK = %X\n",Rd,GPR[Rd].data,K);			
		//For finding difference
		Hex2Bin(0,GPR[Rd].data);
		Hex2Bin(1,K);
		//Rd = Rd - K
		GPR[Rd].data -= K;
		Hex2Bin(2,GPR[Rd].data);

		//For setting SREG
		//For setting Carry flag bit
		SREG[0].data = (!bin[0].arr[7]&bin[1].arr[7]) | (bin[1].arr[7]&bin[2].arr[7]) |
						(bin[2].arr[7]&!bin[0].arr[7]);
		//For setting Zero flag bit
		SREG[1].data = !bin[2].arr[0] & !bin[2].arr[1] & !bin[2].arr[2] & !bin[2].arr[3] &
						!bin[2].arr[4] & !bin[2].arr[5] & !bin[2].arr[6] & !bin[2].arr[7];
		//For setting Negative flag bit
		SREG[2].data = bin[2].arr[7];
		//For setting Overflow flag bit
		SREG[3].data = (bin[0].arr[7]&!bin[1].arr[7]&!bin[2].arr[7]) |
						(!bin[0].arr[7]&bin[1].arr[7]&bin[2].arr[7]);
		//For setting Signed bit
		SREG[4].data = SREG[2].data ^ SREG[3].data;
		//For setting Half Carry flag bit
		SREG[5].data = (!bin[0].arr[3]&bin[1].arr[3]) | (bin[1].arr[3]&bin[2].arr[3]) |
						(bin[2].arr[3]&!bin[0].arr[3]);

		if(debugMode==1)
			printf("\nAfter execution\nReg[%d] = %X\n",Rd,GPR[Rd].data);

		ClearBins(0); ClearBins(1); ClearBins(2);			

		PC += 0x2;	
	}

/************************************************************************************************/
//	SBCI by AJ		10/05/20
	else if(b1==0x4)
	{
		if(debugMode==1)
			printf("\nSUBCI instruction decoded\n");
		int dbits[4],Kbits[8],Rd=0;
		unsigned char K=0x0;
		//For finding Rd and K
		Hex2Bin(0,b2);
		printf("\n");	
		Hex2Bin(1,b3);
		Hex2Bin(2,b4);
		for(i=0;i<4;i++)
			dbits[i] = bin[1].arr[i];
		for(i=0;i<4;i++)
			{
				Kbits[i] = bin[2].arr[i];
				Kbits[i+4] = bin[0].arr[i];
			}
		for(i=0;i<4;i++)
				Rd += dbits[i]*pow(2,i);
		for(i=0;i<8;i++)
				K += Kbits[i]*pow(2,i);
		//Since this SUBI can address R16 to R32
			Rd += 16;
		ClearBins(0); ClearBins(1); ClearBins(2);

		if(debugMode == 1)
			printf("\nBefore execution\nReg[%d] = %X\nK = %X\n",Rd,GPR[Rd].data,K);			
		//For finding difference
		Hex2Bin(0,GPR[Rd].data);
		Hex2Bin(1,K);
		//Rd = Rd - K - Carry flag bit (SREG[0])
		GPR[Rd].data -= K + SREG[0].data;
		Hex2Bin(2,GPR[Rd].data);

		//For setting SREG
		//For setting Carry flag bit
		SREG[0].data = (!bin[0].arr[7]&bin[1].arr[7]) | (bin[1].arr[7]&bin[2].arr[7]) |
						(bin[2].arr[7]&!bin[0].arr[7]);
		//For setting Zero flag bit
		SREG[1].data = !bin[2].arr[0] & !bin[2].arr[1] & !bin[2].arr[2] & !bin[2].arr[3] &
						!bin[2].arr[4] & !bin[2].arr[5] & !bin[2].arr[6] & !bin[2].arr[7];
		//For setting Negative flag bit
		SREG[2].data = bin[2].arr[7];
		//For setting Overflow flag bit
		SREG[3].data = (bin[0].arr[7]&!bin[1].arr[7]&!bin[2].arr[7]) |
						(!bin[0].arr[7]&bin[1].arr[7]&bin[2].arr[7]);
		//For setting Signed bit
		SREG[4].data = SREG[2].data ^ SREG[3].data;
		//For setting Half Carry flag bit
		SREG[5].data = (!bin[0].arr[3]&bin[1].arr[3]) | (bin[1].arr[3]&bin[2].arr[3]) |
						(bin[2].arr[3]&!bin[0].arr[3]);

		if(debugMode==1)
			printf("\nAfter execution\nReg[%d] = %X\n",Rd,GPR[Rd].data);

		ClearBins(0); ClearBins(1); ClearBins(2);			

		PC += 0x2;
	}

/************************************************************************************************/
//	SBIW by AJ		date
	else if(b1==0x9 && b2==7)
	{
		if(debugMode==1)
		{
			printf("SBIW instruction decoded\n");
			PrintReg(15,32);
		}
	    char b=0;
	    int rd1,rd2;
	    int arr2[16];
	    for(i=0;i<16;i++)
	        arr2[i]=0;
	    
	    // for selecting rd
	    ClearBins(0);
	    Hex2Bin(0,b3);
	    i = bin[0].arr[1];
	    j = bin[0].arr[0];
	    
	    t = i*2 + j;
	    
	    if(t==0)
	    {
	        rd1 = 24;
	        rd2 = rd1 + 1;
	    }
	    else if(t==1)
	    {
	        rd1 = 26;
	        rd2 = rd1 + 1;
	    }
	    else if(t==2)
	    {
	        rd1 = 28;
	        rd2 = rd1 + 1;
	    }
	    else if(t==3)
	    {
	        rd1 = 30;
	        rd2 = rd1 + 1;
	    }
	    
	    //storing k values into arr2
	    ClearBins(1);
	    Hex2Bin(1,b4);
	    for(i=0;i<4;i++)
	        arr2[i] = bin[1].arr[i];
	    i = bin[0].arr[3];
	    j = bin[0].arr[2];
	    arr2[4] = j;
	    arr2[5] = i;
	    
	    //subtracting k value from register pair data

	    if(GPR[rd1].data== 0 && GPR[rd2].data==0)
	    	{
	    		SREG[1].data = 1;
	    		SREG[1].data = 0;
	    	}
	    else
	    	{	
	    		b=0;
	    		for(i=0;i<16;i++)
		        	b += arr2[i]*pow(2,i);

			    if(b <= GPR[rd1].data)
			    {
			    	GPR[rd1].data -= b;
			    }
			    else if(b>GPR[rd1].data && GPR[rd2].data>0)
			    {
			    	GPR[rd2].data -= 0x1;
			    	t = GPR[rd1].data - b;
			    	GPR[rd1].data = 0xFF + t;
			    }

			    if(GPR[rd1].data==0 && GPR[rd2].data==0)
			    	SREG[1].data = 1;
			    else
			    	SREG[1].data = 0;
			}

	    if(debugMode==1)
		{
			printf("\nAfter Operation - \n");
			PrintReg(15,32);
		}
		PC += 0x2;
	}

/************************************************************************************************/
//	ORI by AJ		02/05/20
	else if(b1==0x06)
	{
		unsigned char k = b2*16 + b4;
		if(debugMode == 1)
		{
			printf("\nORI instruction decoded\n");
			printf("\nReg[%d](%X) or %X = %X\n",b3+16,GPR[b3+16].data,k,GPR[b3+16].data|k);
		}
		GPR[b3+16].data = GPR[b3+16].data | k;
		PC += 0x02;
	}

/************************************************************************************************/
//	ANDI by AJ		02/05/20
//	Modified by SK		13/06/2020
	else if(b1==0x07)
	{
		unsigned char k = b2*16 + b4;
		if(debugMode == 1)
		{
			printf("\nANDI instruction decoded\n");
			printf("\nReg[%d](%X) or %X = %X\n",b3+16,GPR[b3+16].data,k,GPR[b3+16].data&k);
		}
		GPR[b3+16].data = GPR[b3+16].data & k;
		if(GPR[b3+16].data==0)
			SREG[1].data=1;
		else
			SREG[1].data=0;
		PC += 0x02;
	}

/************************************************************************************************/
//	LDI by AJ		date
	else if(b1==0xE)								
	{
		if(debugMode==1)
			printf("LDI instruction decoded\n");
		GPR[b3+16].data = b2*16 + b4;
		PC += 0x2;
	}

/************************************************************************************************/	
/*//	CLC by AJ & SB	14/03/20			!!!COVERED IN BCLR!!!
	else if(b1==0x9 && b2==4 && b3==8 && b4==8)
	{
		if(debugMode==1)
	    printf("CLC instruction decoded\n");
	    SREG[0].data = 0;
	    PC += 0x2;
	}*/
/************************************************************************************************/	
//	CP by SB		27/03/20
//	Modified by AJ	5/4/20
//	Modified by SM 	11/5/20
	else if(b1==0x1 && b2>=4 && b2<=7)
	{
	            // GPR[17].data=0x99;
	            // GPR[0].data=0xf0;

		if(debugMode==1)
    	printf("\nCP instruction decoded\n");
	    //Comparing Rd and Rr (Reg data doesn't have to be modified)
            
	    unsigned char r=((b2>>1)&1)*16+b4, d=(b2&1)*16+b3;			//setting r and d as per opcode

        if(debugMode == 1)
			printf("\n%X - %X = %d\n",GPR[d].data,GPR[r].data,GPR[d].data -GPR[r].data);

	    //UPDATE FLAGS
	    if(GPR[d].data < (GPR[r].data))
	    	SREG[0].data = 1;				// carry flag
            else
	    	SREG[0].data = 0;

	    if(GPR[d].data - GPR[r].data == 0)
	    	SREG[1].data = 1;		      		//zero flag
	    else
		SREG[1].data=0;

	    unsigned char dl=GPR[d].data & 0x80;
	    unsigned char rl=GPR[r].data & 0x80;
        unsigned char fl=(GPR[d].data - GPR[r].data) & 0x80;
	    if (fl==0x80)					// negative flag	
		SREG[2].data=1;
	    else
		SREG[2].data=0;

	    unsigned char f=(GPR[d].data & 0xf); 		//half carry flag
            unsigned char g=(GPR[r].data & 0xf);
	    printf("%X,%X",f,g);
	    if (f<g)
		SREG[5].data=1;
	    else
		SREG[5].data=0;
								//overflow flag
	    if(dl==0 && rl==0x80 && fl==0x80)	
		SREG[3].data=1;
	    else if(dl==0x80 && rl==0 && fl==0)
		SREG[3].data=1;
	    else
		SREG[3].data=0;
	    
	    SREG[4].data=SREG[3].data ^ SREG[2].data;		//signed flag

	    PC += 0x2;
	}

/************************************************************************************************/	
/*//	CLH by SB		30/03/20			!!!COVERED IN BCLR!!!
//	Modified by AJ	5/4/20
	else if(b1==0x9 && b2==4 && b3==0xD && b4==8)
	{
		if(debugMode==1)
	    printf("CLH instruction decoded\n");
	    SREG[5].data = 0;
	    PC += 0x2;
	}*/

/************************************************************************************************/	
/*//	CLI by SB		30/03/20			!!!COVERED IN BCLR!!!
//	Modified by AJ	5/4/20
	else if(b1==0x9 && b2==4 && b3==0xF && b4==8)
	{
	if(debugMode==1)
		printf("CLI instruction decoded\n");
	SREG[7].data = 0;
	PC += 0x2;
	}*/
/************************************************************************************************/	
/*//	CLN by SB		30/03/20			!!!COVERED IN BCLR!!!
//	Modified by AJ	5/4/20
	else if(b1==0x9 && b2==4 && b3==0xA && b4==8)
	{
		if(debugMode==1)
	    	printf("CLN instruction decoded\n");
	    SREG[5].data = 0;
	    PC += 0x2;
	}*/

/************************************************************************************************/	
/*//	CLZ by SB		30/03/20			!!!COVERED IN BCLR!!!
//	Modified by AJ	5/4/20
	else if(b1==0x9 && b2==4 && b3==0x9 && b4==8)
	{
		if(debugMode==1)
	    printf("CLZ instruction decoded\n");
	    SREG[1].data = 0;
	    PC += 0x2;
	}*/

/************************************************************************************************/	
/*//	CLS by SB 		30/03/20			!!!COVERED IN BCLR!!!
//	Modified by AJ	5/4/20
	else if(b1==0x9 && b2==4 && b3==0xC && b4==8)
	{
		if(debugMode==1)
	    	printf("CLS instruction decoded\n");
	    SREG[4].data = 0;
	    PC += 0x2;
	}*/

/************************************************************************************************/	
/*//	CLT by SB		30/03/20			!!!COVERED IN BCLR!!!
//	Modified by AJ	5/4/20
	else if(b1==0x9 && b2==4 && b3==0xE && b4==8)
	{
		if(debugMode==1)
	    	printf("CLT instruction decoded\n");
	    SREG[6].data = 0;
	    PC += 0x2;
	}*/

/************************************************************************************************/	
/*//	CLV by SB		30/03/20			!!!COVERED IN BCLR!!!
//	Modified by AJ	5/4/20
	else if(b1==0x9 && b2==4 && b3==0xB && b4==8)
	{
		if(debugMode==1)
	    	printf("CLV instruction decoded\n");
	    SREG[3].data = 0;
	    PC += 0x2;
	}*/


/************************************************************************************************/
/*//	SEC by SB & AJ	14/03/2020		!!!COVERED IN BSET!!!
	else if(b1==0x9 && b2==4 && b3==0 && b4==8)
	{
	   if(debugMode==1)
	   		printf("\nSEC instruction decoded\n");
	   SREG[0].data = 1;
	   PC += 0x2;
	}*/

/************************************************************************************************/
//	SBI by AJ	2/05/2020
	else if(b1==0x09 && b2==0x0A)
	{
		if(debugMode == 1)
	    	printf("\nSBI instruction decoded\n");
	    int Abits[5],Bbits[3];
	    Hex2Bin(0,b3);
	    Hex2Bin(1,b4);
	    for(i=0;i<4;i++)
	    	Abits[i+1] = bin[0].arr[i];
	    Abits[0] = bin[1].arr[3];
	    for(i=0;i<3;i++)
	    	Bbits[i] = bin[1].arr[i];

	    j=0;
	    for(i=0;i<3;i++)
	    	j += Bbits[i]*pow(2,i);
	    t=0;
	    for(i=0;i<5;i++)
	    	t += Abits[i]*pow(2,i);

	    if(debugMode == 1)
	    	printf("\nBefore execution: IOREG[%X]: %X",t,IOREG[t].data);

	    Hex2Bin(2,IOREG[t].data);
	    bin[2].arr[j] = 1;
	    j=0;
	    for(i=0;i<8;i++)
	    	j += bin[2].arr[i]*pow(2,i);
	    IOREG[t].data = j;

	    if(debugMode == 1)
	    	printf("\nAfter execution: IOREG[%X]: %X",t,IOREG[t].data);

	    PC += 0x02;
	}

/************************************************************************************************/
//	CPI by AJ	27/04/2020
//  Modified by SM   11/05/2020
	else if(b1==0x03)
	{
	   	if(debugMode==1)
	    	printf("\nCPI instruction decoded\n");
	   	unsigned char k = b2*16 + b4;				////setting k reg as per opcode
	   	if(debugMode == 1)
	   	printf("\n%X - %X = %d\n",GPR[b3+16].data,k,GPR[b3+16].data - k);

		//UPDATE FLAGS

	   	if(GPR[b3+16].data == k)					// zero flag
			SREG[1].data = 1;
       	else if(GPR[b3+16].data != k)
			SREG[1].data = 0;
	    
	    if(GPR[b3+16].data < k)					// carry flag
	    	SREG[0].data = 1;				
        else
	    	SREG[0].data = 0;

	    unsigned char rl=GPR[b3+16].data & 0x80;
	    unsigned char kl=k & 0x80;
	    unsigned char fl=GPR[b3+16].data-k & 0x80;
	    if (fl==0x80)			       			// negative flag	
			SREG[2].data=1;
	    else
			SREG[2].data=0;

	    unsigned char f=GPR[b3+16].data & 0xf; 			//half carry flag
        k= k & 0xf;
	    if (f<k)
			SREG[5].data=1;
	    else
			SREG[5].data=0;
									//overflow flag
	    if(rl==0 && kl==0x80 && fl==0x80)	
			SREG[3].data=1;
	    else if(rl==0x80 && kl==0 && fl==0)
			SREG[3].data=1;
	    else
			SREG[3].data=0;
	    
	    SREG[4].data=SREG[3].data ^ SREG[2].data;			//signed flag

	    PC += 0x2;
	}
/***********************************************************************************************/	
//	COM by AJ		05/05/20
	else if(b1==0x09 && (b2==0x05 || b2==0x04) && b4==0x0)
	{
		if(debugMode == 1)
			{
				printf("\nCOM instruction decoded\n");
				printf("\nBEfore execution Reg[%d] = %X",b3+16,GPR[b3+16].data);
			}

		// Converting hex data contained in reg to binary for complementing
		Hex2Bin(0,GPR[b3+16].data);
		unsigned char temp = 0x0;

		// 1's complement
		for(i=0;i<7;i++)
			bin[0].arr[i] = !bin[0].arr[i];
		// Set N flag in SREG if MSB is set, reset otherwise
		if(bin[0].arr[7] ==1)
			SREG[2].data = 1;
		else
			SREG[2].data = 0;
		// Set Z flag if result is zero
		j=0;
		for(i=0;i<8;i++)
			j |= bin[0].arr[i];
		if(j==0)
			SREG[1].data = 1;
		else
			SREG[1].data = 0;
		// C flag is always set
		SREG[0].data = 1;

		// Generating hex value from binary
		for(i=0;i<8;i++)
			temp += bin[0].arr[i]*pow(2,i);

		GPR[b3+16].data = temp;
		if(debugMode == 1)
			printf("\nAfter execution Reg[%d] = %X\n",b3+16,GPR[b3+16].data);

		PC += 0x02;

	}

/***********************************************************************************************/	
//	OUT by AJ		03/05/20
	else if(b1==0x0B && (b2>=0x08 && b2<= 0x0F))
	{
		if(debugMode==1)
			printf("\nOUT instruction decoded\n");
		int Abits[6];
		Hex2Bin(0,b2);
		Hex2Bin(1,b4);
		Abits[5] = bin[0].arr[2];
		Abits[4] = bin[0].arr[1];
		for(i=0;i<4;i++)
			Abits[i] = bin[1].arr[i];

		t=0;
		for(i=0;i<6;i++)
			t += Abits[i]*pow(2,i);

		IOREG[t].data = GPR[b3+16].data;
		if(debugMode == 1)
			printf("\nIOREG[%X] = %X",t,IOREG[t].data);
		ClearBins(0);
		ClearBins(1);			
		PC += 0x2;
	}

/***********************************************************************************************/	
//	IN by AJ		27/04/20
	else if(b1==0x0B && b2 <= 0x07)
	{
			if(debugMode==1)
				printf("\nIN instruction decoded\n");
			int Abits[6];
			Hex2Bin(0,b2);
			Hex2Bin(1,b4);
			Abits[5] = bin[0].arr[2];
			Abits[4] = bin[0].arr[1];
			for(i=0;i<4;i++)
				Abits[i] = bin[1].arr[i];

			t=0;
			for(i=0;i<6;i++)
				t += Abits[i]*pow(2,i);

			if(debugMode == 1)
				printf("\n Reg[%d] = %X",b3+16,IOREG[t].data);

			GPR[b3+16].data = IOREG[t].data;
			ClearBins(0);
			ClearBins(1);
			PC += 0x2;
	}


/************************************************************************************************/	
//	RJMP by AJ		30/4/20
	else if(b1==0x0C)
	{
    	if(debugMode==1)
			printf("\nRJMP instruction decoded\n");
		int i,j=0,kbits[12];
		char jump=0x0;
		Hex2Bin(0,b2);
		Hex2Bin(1,b3);
		Hex2Bin(2,b4);
		for(i=0;i<4;i++)
			{
				kbits[i] = bin[2].arr[i];
				kbits[i+4] = bin[1].arr[i];
				kbits[i+8] = bin[0].arr[i];
			}

		//If signed bit is set, negative jump
		if(kbits[11] == 1)	
		{
			for(i=0;i<11;i++)
				j += kbits[i]*pow(2,i);
			j -= 0x01;
			i=0;
			while(j!=0 && i<=11)
			{
				kbits[i] = j % 2;
				i++;
				j /= 2;
			}
			for(i=0;i<11;i++)
				kbits[i] = !kbits[i];

			for(i=0;i<11;i++)
				jump += kbits[i]*pow(2,i);
			jump *= -2;
		}
		else
		{
			for(i=0;i<11;i++)
				jump += kbits[i]*pow(2,i);
			jump *= 2;
		}
		if(debugMode == 1)
			printf("\nJumping to PC: %X",PC+jump+0x02);

		PC += jump + 0x02;
		//wait_Clocks = 1;
	}


/************************************************************************************************/
//	BRNE by AJ			30/4/2020
	else if(b1==0xf && b2>=4 && b2<=7 && (b4 == 0x09 || b4 == 0x01))
	{
		int kbits[7],jump=0;
		char temp=0x0;
		if(debugMode==1)
			printf("\nBRNE instruction decoded\n");
		if(SREG[1].data == 0)
		{
			//For getting Kbits
			Hex2Bin(0,b2);
			Hex2Bin(1,b3);
			Hex2Bin(2,b4);
			kbits[6] = bin[0].arr[0];
			kbits[5] = bin[0].arr[1];
			for(i=0;i<4;i++)
				kbits[i+1] = bin[1].arr[i];
			kbits[0] = bin[2].arr[3];

			if(kbits[6] == 1)	//Signed bit set (k is negative)
			{
				for(i=0;i<6;i++)
					temp += kbits[i]*pow(2,i);
				temp -= 0x01;
				i=0;
				while(temp!=0 && i<=6)
				{
					kbits[i] = temp % 2;
					i++;
					temp /= 2;
				}
				for(i=0;i<6;i++)
					kbits[i] = !kbits[i];

				for(i=0;i<6;i++)
					jump += kbits[i]*pow(2,i);
				jump *= -2;
			}
			else
			{
				for(i=0;i<6;i++)
					jump += kbits[i]*pow(2,i);
				jump *= 2;
			}
			if(debugMode == 1)
				printf("\nJumping to PC: %X",PC+jump+0x02);
			PC += jump + 0x02;

		}
		else
			PC += 0x2;

	}

/************************************************************************************************/
//	BREQ by AJ		27/04/20
	else if(b1==0xf && b2>=3 && (b4==0x01||b4==0x09))
	{
		if(debugMode == 1)
			printf("\nBREQ instruction decoded\n");
		int kbits[7],jump=0;
		char temp=0x0;
		if(SREG[1].data == 1)
		{
			// For getting Kbits
			Hex2Bin(0,b2);
			Hex2Bin(1,b3);
			Hex2Bin(2,b4);
			kbits[6] = bin[0].arr[0];
			kbits[5] = bin[0].arr[1];
			for(i=0;i<4;i++)
				kbits[i+1] = bin[1].arr[i];
			kbits[0] = bin[2].arr[3];

			if(kbits[6] == 1)	//Signed bit set (k is negative)
			{
				for(i=0;i<6;i++)
					temp += kbits[i]*pow(2,i);
				temp -= 0x01;
				i=0;
				while(temp!=0 && i<=6)
				{
					kbits[i] = temp % 2;
					i++;
					temp /= 2;
				}
				for(i=0;i<6;i++)
					kbits[i] = !kbits[i];

				for(i=0;i<6;i++)
					jump += kbits[i]*pow(2,i);
				jump *= -2;
			}
			else
			{
				for(i=0;i<6;i++)
					jump += kbits[i]*pow(2,i);
				jump *= 2;
			}
			if(debugMode == 1)
				printf("\nJumping to PC: %X",PC+jump+0x02);
			PC += jump + 0x02;
		}
	else
		PC += 0x2;

		ClearBins(0); ClearBins(1); ClearBins(2);

	}

/************************************************************************************************/
//	CPSE by AJ		01/05/20
	else if(b1==0x01 && (b2>=0x0 || b2<=0x03))
	{
		if(debugMode == 1)
			printf("\nCPSE instrution decoded\n");
		if(GPR[b3+16].data == GPR[b4+16].data)
		{
			if(debugMode == 1)
				printf("\nSkipping to PC = %X\n",PC+0x04);
			PC += 0x04;
		}
		else
			PC += 0x02;
	}

/************************************************************************************************/
//	DEC by AJ		30/4/2020
	else if(b1==0x09 && (b2==0x05 || b2==0x04) && b4==0x0A)
	{
		if(debugMode == 1)
			{
				printf("\nDEC instruction decoded\n");
				printf("\nBefore execution: Reg[%d]: %X",b3+16,GPR[b3+16].data);
			}
		GPR[b3+16].data -= 0x01;
		if(GPR[b3+16].data == 0x0)
			SREG[1].data = 1;
		else
			SREG[1].data = 0;

		if(debugMode == 1)
			printf("\nAfter execution: Reg[%d]: %X",b3+16,GPR[b3+16].data);

		PC += 0x02;
	}

/************************************************************************************************/
//  ICALL BY SM   on  14/05/20 

	else if(b1==0x9 && b2==0x5 && b3==0 && b4==0x9)
	{	
        if(debugMode==1)
           	printf("\nICALL instruction decoded\n");

		int k=PC+2;
		SRAM[SP-1].data=k;				//lower 8 bits
		SRAM[SP].data=k>>8;			//upper 8 bits

		SP -= 2;

		PC = GPR[30].data + GPR[31].data*256;	// Z pointer register         
	}

/************************************************************************************************/
//  IJMP BY SM   on  14/05/20 
	else if(b1==0x9 && b2==0x4 && b3==0 && b4==0x9)
	{
        if(debugMode==1)
           	printf("\nIJMP instruction decoded\n");

		PC = GPR[30].data + GPR[31].data*256;	// Z pointer register

	}

/************************************************************************************************/	
//	SBI by SM 	07/05/20
	else if(b1==0x9 && b2==0xA)
	{	
		char b,bits; 
	    if (b4>7)
			b=b4-8;
	    else
			b=b4;
	    char A= b3*2 + (b4>>3);
		if(debugMode==1){
	    	printf("\nSBI instruction decoded\n");
			printf("\nBefore execution: Reg[%d] = %X\n",A,IOREG[A].data);
			}

	    if(b==0)
			bits=1;
	    else if(b==1)
			bits=2;
	    else if(b==2)
			bits=4;
	    else if(b==3)
			bits=8;
	    else if(b==4)
			bits=16;
	    else if(b==5)
			bits=32;
	    else if(b==6)
			bits=64;
	    else if(b==7)
			bits=128;
 
	    IOREG[A].data = IOREG[A].data | bits;

	    if (debugMode==1)
			printf("\nAfter execution: Reg[%d] = %X\n",A,IOREG[A].data);
	    PC += 0x2;
	}

/************************************************************************************************/	
//	CBI by SM 	07/05/20
	else if(b1==0x9 && b2==8)
	{	 
		char b,bits;
	    if (b4>7)
			b=b4-8;
	    else
			b=b4;
	    char A= b3*2 + (b4>>3);
		if(debugMode==1){
	    	printf("\nCBI instruction decoded\n");
			printf("\nBefore execution: Reg[%d] = %X\n",A,IOREG[A].data);
			}

	    if(b==0)
			bits=0xfe;
	    else if(b==1)
			bits=0xfd;
	    else if(b==2)
			bits=0xfb;
	    else if(b==3)
			bits=0xf7;
	    else if(b==4)
			bits=0xef;
	    else if(b==5)
			bits=0xdf;
	    else if(b==6)
			bits=0xbf;
	    else if(b==7)
			bits=0x7f;
 
	    IOREG[A].data = IOREG[A].data & bits;

	    if (debugMode==1)
			printf("\nAfter execution: Reg[%d] = %X\n",A,IOREG[A].data);
	    PC += 0x2;
	}

/************************************************************************************************/ 
//  ASR BY SM   on  06/05/20          Modified on 9/05/20
    else if(b1==0x9 && b2>=4 && b4==5)
    {	
        unsigned char k=(b2&1)*16+b3;			//set reg k according to opcode 
        if(debugMode==1){
           	printf("\nASR instruction decoded\n");
            printf("\nBefore execution: Reg[%d] = %X\n",k,GPR[k].data);
            }
                
                
	    SREG[0].data=GPR[k].data & 1;            // set or reset carry flag
	    char l=GPR[k].data & 10000000;
		GPR[k].data=((GPR[k].data) >> 1) | l;				//right shifting by 1
	    // update flags
		if (GPR[k].data==0)
			SREG[1].data=1;                  // zero flag
		else
			SREG[1].data=0;
		if (l==10000000)
			SREG[2].data=1;			//negative flag
		else
			SREG[2].data=0;
		SREG[3].data=SREG[0].data ^ SREG[2].data;	//overflow flag
		SREG[4].data=SREG[3].data ^ SREG[2].data;	//signed flag

		if(debugMode==1)
        	printf("\nAfter execution: Reg[%d] = %X\n",k,GPR[k].data);
        PC += 0x2;

    }

/************************************************************************************************/
/*//  ROL BY SM   on    13/05/20		!!COVERED IN ADC!!
    else if(b1==0x1 && b2>=0xC && ((b2&1)*16+b3)==(((b2>>1)&1)*16+b4))
    {
		unsigned char k=(b2&1)*16+b3, r=((b2>>1)&1)*16+b4;
	    if(debugMode==1){
	        printf("\nROL instruction decoded\n");
	        printf("\nBefore execution: Reg[%d] = %X\n",k,GPR[k].data);
	        }
	                
	    unsigned char zbit=SREG[0].data;            
	    SREG[0].data=GPR[k].data & 10000000;
	    GPR[k].data=((GPR[k].data) << 1);
		GPR[k].data=GPR[k].data + zbit;

        // update flags
		if (GPR[k].data==0)
			SREG[1].data=1;                  // zero flag
		else
			SREG[1].data=0;
		if (GPR[k].data >= 0x80)
			SREG[2].data=1;			//negative flag
		else
			SREG[2].data=0;
		SREG[3].data=SREG[0].data ^ SREG[2].data;	//overflow flag
		SREG[4].data=SREG[3].data ^ SREG[2].data;	//signed flag

		if(debugMode==1)
        	printf("\nAfter execution: Reg[%d] = %X\n",k,GPR[k].data);
        PC += 0x2;

    }*/

/************************************************************************************************/
//  SER BY SM                5/05/20
    else if(b1==0xD && b2==0xF && b4==0xF)
    {
        if(debugMode==1){
           	printf("SER instruction decoded\n");
            printf("\nBefore execution: Reg[%d] = %X\n",b3+16,GPR[b3+16].data);
            }
		GPR[b3+16].data = 0xFF;
		if(debugMode==1)
        	printf("\nAfter execution: Reg[%d] = %X\n",b3+16,GPR[b3+16].data);
		PC += 0x2;
    }

/************************************************************************************************/
/*//  CLR BY SM  on   5/05/20      !!!COVERED IN EOR!!!
    else if(b1==2 && b2>=4)
    {
		unsigned char R=b3*16+b2;
        if(debugMode==1){
           	printf("CLR instruction decoded\n");
            printf("\nBefore execution: Reg[%d] = %X\n",R,GPR[R].data);
            }
     		
		GPR[R].data = GPR[R].data ^ GPR[R].data ;

		//update flags
		SREG[1].data=1;
		SREG[2].data=0;
		SREG[3].data=0;
		SREG[4].data=0;

		printf("\nAfter execution: Reg[%d] = %X\n",R,GPR[R].data);
        PC += 0x2;

    }*/

/************************************************************************************************/
/*//	SES BY SM                5/05/20		!!!COVERED IN BSET!!!
	else if(b1==0x9 && b2==4 && b3==4 && b4==8)
	{
	   if(debugMode==1)
	   		printf("\nSES instruction decoded\n");
	   SREG[4].data = 1;
	   PC += 0x2;
	}*/

/************************************************************************************************/
/*//	SEN BY SM                5/05/20		!!!COVERED IN BSET!!!
	else if(b1==0x9 && b2==4 && b3==2 && b4==8)
	{
	   if(debugMode==1)
	   		printf("\nSEN instruction decoded\n");
	   SREG[2].data = 1;
	   PC += 0x2;
	}*/

/************************************************************************************************/
/*//	SEH BY SM                5/05/20		!!!COVERED IN BSET!!!
	else if(b1==0x9 && b2==4 && b3==5 && b4==8)
	{
	   if(debugMode==1)
	   		printf("\nSEH instruction decoded\n");
	   SREG[5].data = 1;
	   PC += 0x2;
	}*/

/************************************************************************************************/	
//	AND by SM  		12/5/20

	else if(b1==0x2 && b2>=0 && b2<=3)
	{	    	  
	    unsigned char r=((b2>>1)&1)*16+b4,d=(b2&1)*16+b3;		//setting r and d as per opcode
	    if(r == d && debugMode==1)
	    	printf("\nTST instruction decoded\n");		//TST = AND Rd,Rd
	    else if (r != d && debugMode==1)
	    	printf("\nAND instruction decoded\n");

        if(debugMode == 1)
			printf("\n%X AND %X = %X\n",GPR[d].data,GPR[r].data,GPR[d].data & GPR[r].data);

		GPR[d].data = GPR[d].data & GPR[r].data;

	    //UPDATE FLAGS
	    if(GPR[d].data == 0X0)
	    	SREG[1].data = 1;		      		//zero flag
	    else
		SREG[1].data=0;

	    unsigned char dl=GPR[d].data & 0x80;
	    if (dl==0x80)					// negative flag	
		SREG[2].data=1;
	    else
		SREG[2].data=0;
								
	    SREG[3].data=0;					//overflow flag
	    
	    SREG[4].data=SREG[3].data ^ SREG[2].data;		//signed flag


	    PC += 0x2;
	}

/************************************************************************************************/	
//	EOR by SM  		12/5/20

	else if(b1==0x2 && b2>=4 && b2<=7)
	{
	    unsigned char r=((b2>>1)&1)*16+b4, d=(b2&1)*16+b3;	//setting r and d as per opcode
	    if(r==d && debugMode==1)
	    	printf("\nCLR instruction decoded\n");		//CLR = EOR Rd,Rd
	    else if(r !=d && debugMode==1)
	    	printf("\nEOR instruction decoded\n");

        if(debugMode == 1)
		   printf("\n%X XOR %X = %X\n",GPR[d].data,GPR[r].data,GPR[d].data ^ GPR[r].data);

	    GPR[d].data = GPR[d].data ^ GPR[r].data;
	    //UPDATE FLAGS
	    if(GPR[d].data == 0)
	    	SREG[1].data = 1;		    //zero flag
	    else
		SREG[1].data=0;

	    unsigned char dl=GPR[d].data & 0x80;
	    if (dl==0x80)					// negative flag	
		SREG[2].data=1;
	    else
		SREG[2].data=0;
								
	    SREG[3].data=0;					//overflow flag
	    
	    SREG[4].data=SREG[3].data ^ SREG[2].data;		//signed flag

	    PC += 0x2;
	}

/************************************************************************************************/	
//	CPC by SM on    06/05/20     Modified on 9/05/20  Modified on 11/05/2020

	else if(b1==0x0 && b2>=4)
	{
	    if(debugMode==1)
	    printf("\nCPC instruction decoded\n");
        unsigned char r=((b2>>1)&1)*16+b4;			//setting r and d as per opcode
	    unsigned char d=(b2&1)*16+b3;
	    unsigned char c=SREG[0].data;
		
	    if(debugMode == 1)
			printf("\n%X - %X -%X = %d\n",GPR[d].data,GPR[r].data,c,GPR[d].data -GPR[r].data- c);

	    //UPDATE FLAGS
	    if(GPR[d].data < (GPR[r].data + c))			// carry flag
	    	SREG[0].data = 1;				
            else
	    	SREG[0].data = 0;

	    if(GPR[d].data - GPR[r].data - c == 0)		//unchanged zero flag
	    	SREG[1].data = SREG[1].data;		      
	    else
		SREG[1].data=0;
            
	    unsigned char dl=GPR[d].data & 0x80;
	    unsigned char rl=GPR[r].data & 0x80;
            unsigned char fl=(GPR[d].data - GPR[r].data - c) & 0x80;
	    if (fl== 0x80)					// negative flag	
		SREG[2].data=1;
	    else
		SREG[2].data=0;

	    unsigned char f=GPR[d].data & 0xf; 			//half carry flag
            unsigned char g=GPR[r].data & 0xf;
	    if (f<(g+c))
		SREG[5].data=1;
	    else
		SREG[5].data=0;
								//overflow flag
	    if(dl==0 && rl==0x80 && fl==0x80)	
		SREG[3].data=1;
	    else if(dl==0x80 && rl==0 && fl==0)
		SREG[3].data=1;
	    else
		SREG[3].data=0;
	    
	    SREG[4].data=SREG[3].data ^ SREG[2].data;		//signed flag

	    PC += 0x2;
	}

/************************************************************************************************/
//	BRCC by SM		 07/5/2020           BRSH also same
	else if(b1==0xf && b2>=4 && b2<=7 && (b4 == 0x08 || b4 == 0x00))
	{
		int kbits[7],jump=0;
		char temp=0x0;
		if(debugMode==1)
			printf("\nBRCC instruction decoded\n");
		if(SREG[0].data == 0)
		{	
			Hex2Bin(0,b2);			//setting the k value
			Hex2Bin(1,b3);
			Hex2Bin(2,b4);
			kbits[6] = bin[0].arr[1];
			kbits[5] = bin[0].arr[0];
			for(i=0;i<4;i++)
				kbits[i+1] = bin[1].arr[i];
			kbits[0] = bin[2].arr[3];

			if(kbits[6] == 1)		//Signed bit set (k is negative)
			{									//branching
				for(i=0;i<6;i++)				
					temp += kbits[i]*pow(2,i);		//perform two's compliment
				temp -= 0x01;
				i=0;
				while(temp!=0 && i<=6)
				{
					kbits[i] = temp % 2;		
					i++;
					temp /= 2;
				}
				for(i=0;i<6;i++)
					kbits[i] = !kbits[i];

				for(i=0;i<6;i++)
					jump += kbits[i]*pow(2,i);
				jump *= -2;
			}
			else				//if k is positive
			{
				for(i=0;i<6;i++)
					jump += kbits[i]*pow(2,i);
				jump *= 2;
			}
			if(debugMode == 1)
				printf("\nJumping to PC: %X",PC+jump+0x02);
			PC += jump + 0x02;

		}
		else
			PC += 0x2;

	ClearBins(0); ClearBins(1); ClearBins(2);

	}


/************************************************************************************************/
/*//	BRLO by SM		 11/5/2020			!!!EXACTLY SAME AS BRCS!!!
	else if(b1==0xf && b2>=0 && b2<=3 && (b4 == 0x08 || b4 == 0x00))
	{	//PC=0x3A;					//JUMP TO 2A,OPCODE=F048 K=9
		//SREG[0].data = 1;		
		int kbits[7],jump=0;
		char temp=0x0;
		if(debugMode==1)
			printf("\nBRLO instruction decoded\n");
		if(SREG[0].data == 1)
		{	PC=0x3A;
			//For getting Kbits
			Hex2Bin(0,b2);
			Hex2Bin(1,b3);
			Hex2Bin(2,b4);
			kbits[6] = bin[0].arr[1];
			kbits[5] = bin[0].arr[0];
			for(i=0;i<4;i++)
				kbits[i+1] = bin[1].arr[i];
			kbits[0] = bin[2].arr[3];

			if(kbits[6] == 1)	//Signed bit set (k is negative)
			{
				for(i=0;i<6;i++)
					temp += kbits[i]*pow(2,i);
				temp -= 0x01;
				i=0;
				while(temp!=0 && i<=6)
				{
					kbits[i] = temp % 2;
					i++;
					temp /= 2;
				}
				for(i=0;i<6;i++)
					kbits[i] = !kbits[i];

				for(i=0;i<6;i++)
					jump += kbits[i]*pow(2,i);
				jump *= -2;
			}
			else
			{
				for(i=0;i<6;i++)
					jump += kbits[i]*pow(2,i);
				jump *= 2;
			}
			if(debugMode == 1)
				printf("\nJumping to PC: %X",PC+jump+0x02);
			PC += jump + 0x02;

		}
		else
			PC += 0x2;

	ClearBins(0); ClearBins(1); ClearBins(2);

	}*/

/************************************************************************************************/
//	BRHC by SM		 07/5/2020
	else if(b1==0xf && b2>=4 && b2<=7 && (b4 == 0xD || b4 == 0x5))
	{	//PC=0x3A;						// opcode F43D jumps to 4A k=7
 		int kbits[7],jump=0;
		char temp=0x0;
		if(debugMode==1)
			printf("\nBRHC instruction decoded\n");
		if(SREG[5].data == 0)
		{
			//For getting Kbits
			Hex2Bin(0,b2);			
			Hex2Bin(1,b3);
			Hex2Bin(2,b4);
			kbits[6] = bin[0].arr[1];
			kbits[5] = bin[0].arr[0];
			for(i=0;i<4;i++)
				kbits[i+1] = bin[1].arr[i];
			kbits[0] = bin[2].arr[3];

			if(kbits[6] == 1)	//Signed bit set (k is negative)	//branching
			{
				for(i=0;i<6;i++)					//perform two's compliment
					temp += kbits[i]*pow(2,i);
				temp -= 0x01;
				i=0;
				while(temp!=0 && i<=6)
				{
					kbits[i] = temp % 2;
					i++;
					temp /= 2;
				}
				for(i=0;i<6;i++)
					kbits[i] = !kbits[i];

				for(i=0;i<6;i++)
					jump += kbits[i]*pow(2,i);
				jump *= -2;
			}
			else					//if k is positive
			{
				for(i=0;i<6;i++)
					jump += kbits[i]*pow(2,i);
				jump *= 2;
			}
			if(debugMode == 1)
				printf("\nJumping to PC: %X",PC+jump+0x02);
			PC += jump + 0x02;

		}
		else
			PC += 0x2;

	ClearBins(0); ClearBins(1); ClearBins(2);

	}


/************************************************************************************************/
//	BRHS by SM		 07/5/2020
	else if(b1==0xf && b2>=0 && b2<=3 && (b4 == 0xD || b4 == 0x5))
	{	//PC=0x3A;					//opcode=F03D jumps to 4A k=7
		//SREG[5].data=1;
		int kbits[7],jump=0;
		char temp=0x0;
		if(debugMode==1)
			printf("\nBRHS instruction decoded\n");
		if(SREG[5].data == 1)
		{
			//For getting Kbits
			Hex2Bin(0,b2);
			Hex2Bin(1,b3);
			Hex2Bin(2,b4);
			kbits[6] = bin[0].arr[1];
			kbits[5] = bin[0].arr[0];
			for(i=0;i<4;i++)
				kbits[i+1] = bin[1].arr[i];
			kbits[0] = bin[2].arr[3];

			if(kbits[6] == 1)	//Signed bit set (k is negative)		//branching
			{
				for(i=0;i<6;i++)				//perform two's compliment
					temp += kbits[i]*pow(2,i);
				temp -= 0x01;
				i=0;
				while(temp!=0 && i<=6)
				{
					kbits[i] = temp % 2;
					i++;
					temp /= 2;
				}
				for(i=0;i<6;i++)
					kbits[i] = !kbits[i];

				for(i=0;i<6;i++)
					jump += kbits[i]*pow(2,i);
				jump *= -2;
			}
			else					//if k is positive
			{
				for(i=0;i<6;i++)
					jump += kbits[i]*pow(2,i);
				jump *= 2;
			}
			if(debugMode == 1)
				printf("\nJumping to PC: %X",PC+jump+0x02);
			PC += jump + 0x02;

		}
		else
			PC += 0x2;

	ClearBins(0); ClearBins(1); ClearBins(2);

	}

/************************************************************************************************/
//	BRTC by SM		 13/5/2020
	else if(b1==0xf && b2>=4 && b2<=7 && (b4 == 0xE || b4 == 0x6))
	{							// opcode F43D jumps to 4A k=7
 		int kbits[7],jump=0;
		char temp=0x0;
		if(debugMode==1)
			printf("\nBRTC instruction decoded\n");
		if(SREG[6].data == 0)
		{
			//For getting Kbits
			Hex2Bin(0,b2);
			Hex2Bin(1,b3);
			Hex2Bin(2,b4);
			kbits[6] = bin[0].arr[1];
			kbits[5] = bin[0].arr[0];
			for(i=0;i<4;i++)
				kbits[i+1] = bin[1].arr[i];
			kbits[0] = bin[2].arr[3];

			if(kbits[6] == 1)	//Signed bit set (k is negative)		//branching
			{
				for(i=0;i<6;i++)				//perform two's compliment
					temp += kbits[i]*pow(2,i);
				temp -= 0x01;
				i=0;
				while(temp!=0 && i<=6)
				{
					kbits[i] = temp % 2;
					i++;
					temp /= 2;
				}
				for(i=0;i<6;i++)
					kbits[i] = !kbits[i];

				for(i=0;i<6;i++)
					jump += kbits[i]*pow(2,i);
				jump *= -2;
			}
			else				//if k is positive
			{
				for(i=0;i<6;i++)
					jump += kbits[i]*pow(2,i);
				jump *= 2;
			}
			if(debugMode == 1)
				printf("\nJumping to PC: %X",PC+jump+0x02);
			PC += jump + 0x02;

		}
		else
			PC += 0x2;

	ClearBins(0); ClearBins(1); ClearBins(2);

	}


/************************************************************************************************/
//	BRTS by SM		 13/5/2020
	else if(b1==0xf && b2>=0 && b2<=3 && (b4 == 0xE || b4 == 0x6))
	{							//opcode=F03D jumps to 4A k=7
		//SREG[6].data=1;
		int kbits[7],jump=0;
		char temp=0x0;
		if(debugMode==1)
			printf("\nBRHS instruction decoded\n");
		if(SREG[6].data == 1)
		{
			//For getting Kbits
			Hex2Bin(0,b2);
			Hex2Bin(1,b3);
			Hex2Bin(2,b4);
			kbits[6] = bin[0].arr[1];
			kbits[5] = bin[0].arr[0];
			for(i=0;i<4;i++)
				kbits[i+1] = bin[1].arr[i];
			kbits[0] = bin[2].arr[3];

			if(kbits[6] == 1)	//Signed bit set (k is negative)		//branching 
			{
				for(i=0;i<6;i++)			//perform two's compliment
					temp += kbits[i]*pow(2,i);
				temp -= 0x01;
				i=0;
				while(temp!=0 && i<=6)
				{
					kbits[i] = temp % 2;
					i++;
					temp /= 2;
				}
				for(i=0;i<6;i++)
					kbits[i] = !kbits[i];

				for(i=0;i<6;i++)
					jump += kbits[i]*pow(2,i);
				jump *= -2;
			}
			else				//if k is positive
			{
				for(i=0;i<6;i++)
					jump += kbits[i]*pow(2,i);
				jump *= 2;
			}
			if(debugMode == 1)
				printf("\nJumping to PC: %X",PC+jump+0x02);
			PC += jump + 0x02;

		}
		else
			PC += 0x2;

	ClearBins(0); ClearBins(1); ClearBins(2);

	}

/************************************************************************************************/
//	BRVC by SM		 13/5/2020
	else if(b1==0xf && b2>=4 && b2<=7 && (b4 == 0xB || b4 == 0x3))
	{								// opcode F43D jumps to 4A k=7
 		int kbits[7],jump=0;
		char temp=0x0;
		if(debugMode==1)
			printf("\nBRVC instruction decoded\n");
		if(SREG[3].data == 0)
		{
			//For getting Kbits
			Hex2Bin(0,b2);
			Hex2Bin(1,b3);
			Hex2Bin(2,b4);
			kbits[6] = bin[0].arr[1];
			kbits[5] = bin[0].arr[0];
			for(i=0;i<4;i++)
				kbits[i+1] = bin[1].arr[i];
			kbits[0] = bin[2].arr[3];

			if(kbits[6] == 1)	//Signed bit set (k is negative)		//branching
			{
				for(i=0;i<6;i++)			//perform two's compliment
					temp += kbits[i]*pow(2,i);
				temp -= 0x01;
				i=0;
				while(temp!=0 && i<=6)
				{
					kbits[i] = temp % 2;
					i++;
					temp /= 2;
				}
				for(i=0;i<6;i++)
					kbits[i] = !kbits[i];

				for(i=0;i<6;i++)
					jump += kbits[i]*pow(2,i);
				jump *= -2;
			}
			else				//if k is positive
			{
				for(i=0;i<6;i++)
					jump += kbits[i]*pow(2,i);
				jump *= 2;
			}
			if(debugMode == 1)
				printf("\nJumping to PC: %X",PC+jump+0x02);
			PC += jump + 0x02;

		}
		else
			PC += 0x2;

	ClearBins(0); ClearBins(1); ClearBins(2);

	}


/************************************************************************************************/
//	BRVS by SM		 13/5/2020
	else if(b1==0xf && b2>=0 && b2<=3 && (b4 == 0xB || b4 == 0x3))
	{						//opcode=F03D jumps to 4A k=7
		//SREG[3].data=1;
		int kbits[7],jump=0;
		char temp=0x0;
		if(debugMode==1)
			printf("\nBRVS instruction decoded\n");
		if(SREG[3].data == 1)
		{
			//For getting Kbits
			Hex2Bin(0,b2);
			Hex2Bin(1,b3);
			Hex2Bin(2,b4);
			kbits[6] = bin[0].arr[1];
			kbits[5] = bin[0].arr[0];
			for(i=0;i<4;i++)
				kbits[i+1] = bin[1].arr[i];
			kbits[0] = bin[2].arr[3];

			if(kbits[6] == 1)	//Signed bit set (k is negative)		//branching
			{
				for(i=0;i<6;i++)				//perform two's compliment
					temp += kbits[i]*pow(2,i);
				temp -= 0x01;
				i=0;
				while(temp!=0 && i<=6)
				{
					kbits[i] = temp % 2;
					i++;
					temp /= 2;
				}
				for(i=0;i<6;i++)
					kbits[i] = !kbits[i];

				for(i=0;i<6;i++)
					jump += kbits[i]*pow(2,i);
				jump *= -2;
			}
			else				//if k is positive
			{
				for(i=0;i<6;i++)
					jump += kbits[i]*pow(2,i);
				jump *= 2;
			}
			if(debugMode == 1)
				printf("\nJumping to PC: %X",PC+jump+0x02);
			PC += jump + 0x02;

		}
		else
			PC += 0x2;

	ClearBins(0); ClearBins(1); ClearBins(2);

	}

/************************************************************************************************/
//	BRCS by SM		 07/5/2020		BRLO same
	else if(b1==0xf && b2>=0 && b2<=3 && (b4 == 0x08 || b4 == 0x00))
	{	
		int kbits[7],jump=0;
		char temp=0x0;
		if(debugMode==1)
			printf("\nBRCS instruction decoded\n");
		if(SREG[0].data == 1)
		{
			//For getting Kbits
			Hex2Bin(0,b2);
			Hex2Bin(1,b3);
			Hex2Bin(2,b4);
			kbits[6] = bin[0].arr[1];
			kbits[5] = bin[0].arr[0];
			for(i=0;i<4;i++)
				kbits[i+1] = bin[1].arr[i];
			kbits[0] = bin[2].arr[3];

			if(kbits[6] == 1)	//Signed bit set (k is negative)		//branching
			{
				for(i=0;i<6;i++)			//perform two's compliment
					temp += kbits[i]*pow(2,i);
				temp -= 0x01;
				i=0;
				while(temp!=0 && i<=6)
				{
					kbits[i] = temp % 2;
					i++;
					temp /= 2;
				}
				for(i=0;i<6;i++)
					kbits[i] = !kbits[i];

				for(i=0;i<6;i++)
					jump += kbits[i]*pow(2,i);
				jump *= -2;
			}
			else				//if k is positive
			{
				for(i=0;i<6;i++)
					jump += kbits[i]*pow(2,i);
				jump *= 2;
			}
			if(debugMode == 1)
				printf("\nJumping to PC: %X",PC+jump+0x02);
			PC += jump + 0x02;

		}
		else
			PC += 0x2;

	ClearBins(0); ClearBins(1); ClearBins(2);
	}

/********************************************************************************************/
//	SBC by VY		13/05/20
	else if(b1==0x00 && b2 >= 0x08 && b2 <= 0x0B)
	{
		if(debugMode==1)
			printf("\nSBC instruction decoded\n");
		int dbits[5],rbits[5],Rd=0,Rr=0;
		//For finding Rd and Rr
		Hex2Bin(0,b2);
		Hex2Bin(1,b3);
		Hex2Bin(2,b4);
		rbits[4] = bin[0].arr[1];
		dbits[4] = bin[0].arr[0];
		for(i=0;i<4;i++)
		{
			dbits[i] = bin[1].arr[i];
			rbits[i] = bin[2].arr[i];
		}
		for(i=0;i<5;i++)
			{
				Rd += dbits[i]*pow(2,i);
				Rr += rbits[i]*pow(2,i);
			}
		ClearBins(0); ClearBins(1); ClearBins(2);
		if(debugMode == 1)
			printf("\nBefore execution\nReg[%d] = %X\nReg[%d] = %X\n",Rd,GPR[Rd].data,Rr,GPR[Rr].data);			
		//For finding difference
		Hex2Bin(0,GPR[Rd].data);
		Hex2Bin(1,GPR[Rr].data);
		//Rd = Rd - Rr
		GPR[Rd].data =GPR[Rd].data - GPR[Rr].data - SREG[0].data;
		Hex2Bin(2,GPR[Rd].data);

		//For setting SREG
		//For setting Carry flag bit
		SREG[0].data = (!bin[0].arr[7]&bin[1].arr[7]) | (bin[1].arr[7]&bin[2].arr[7]) |
						(bin[2].arr[7]&!bin[0].arr[7]);
		//For setting Zero flag bit
		SREG[1].data = !bin[2].arr[0] & !bin[2].arr[1] & !bin[2].arr[2] & !bin[2].arr[3] &
						!bin[2].arr[4] & !bin[2].arr[5] & !bin[2].arr[6] & !bin[2].arr[7];
		//For setting Negative flag bit
		SREG[2].data = bin[2].arr[7];
		//For setting Overflow flag bit
		SREG[3].data = (bin[0].arr[7]&!bin[1].arr[7]&!bin[2].arr[7]) |
						(!bin[0].arr[7]&bin[1].arr[7]&bin[2].arr[7]);
		//For setting Signed bit
		SREG[4].data = SREG[2].data ^ SREG[3].data;
		//For setting Half Carry flag bit
		SREG[5].data = (!bin[0].arr[3]&bin[1].arr[3]) | (bin[1].arr[3]&bin[2].arr[3]) |
						(bin[2].arr[3]&!bin[0].arr[3]);

		if(debugMode==1)
			printf("\nAfter execution\nReg[%d] = %X\nReg[%d] = %X\n",Rd,GPR[Rd].data,Rr,GPR[Rr].data);

		ClearBins(0); ClearBins(1); ClearBins(2);			

		PC += 0x2;
	}

/************************************************************************************************/
//SBR by VY        13/05/20 mod on 18/05/2020
    else if(b1==0x06)
    {
        unsigned char k = b2*16 + b4;//constant value
        if(debugMode == 1)
        {
            printf("\nSBR instruction decoded\n");
            printf("\nReg[%d](%X) or %X = %X\n",b3+16,GPR[b3+16].data,k,GPR[b3+16].data | k);
        }
        //Rd=Rd or k(to set specific bit in Rd)
        GPR[b3+16].data = GPR[b3+16].data | k;
        //overflow is always zero
                SREG[3].data=0;
                if(GPR[b3+16].data>=80) //setting negative flag
                        SREG[2].data = 1;
        else
            SREG[2].data = 0;
           //sign flag(as v=0,signflag=negflag)      
                SREG[4].data = SREG[2].data;
                if(GPR[b3+16].data == 0x0) //checking zero flag
            SREG[1].data = 1;
                else
                        SREG[1].data = 0;
        PC += 0x02;
    }


/************************************************************************************************/
/*	CBR by VY		07/05/20			!!!COVERED IN ANDI!!
	else if(b1==0x07)
	{
		unsigned char k = b2*16 + b4;
        k = 0xFF - k;
		if(debugMode == 1)
		{
			printf("\nCBR instruction decoded\n");
			printf("\nReg[%d](%X) or %X = %X\n",b3+16,GPR[b3+16].data,k,GPR[b3+16].data&k);
		}
		GPR[b3+16].data = GPR[b3+16].data & k;

        SREG[3].data=0;
        if(GPR[b3+16].data>=80)
            SREG[2].data = 1;
		else
			SREG[2].data = 0;
                
        SREG[4].data = SREG[2].data;
        if(GPR[b3+16].data == 0x0)
			SREG[1].data = 1;
        else
            SREG[1].data = 0;

		PC += 0x02;
	}*/

/***********************************************************************************************/	
//	SBRC by VY		13/05/20
	else if(b1==0x0F && (b2==0x0C || b2==0x0D) && (b4>=0x00 && b4<=0x07))
	{       
		if(debugMode == 1)
			printf("\nSBRC instruction decoded\n");     
		unsigned char r= (b2&1)*16+b3;//Rr value
        unsigned char Rd=0;
        //finding b value 
        bin[0].arr[b4]=1;
        for(i=0;i<8;i++)
            Rd += bin[0].arr[i]*pow(2,i);//converting decimal
            //checking if Rr(b) = 0
        unsigned char k= GPR[r].data & Rd;
        if (k == 0)
            PC += 0x04;
        else
            PC += 0x02;
        ClearBins(0);
    }

/***********************************************************************************************/	
//	SBRS by VY		13/05/20
	else if(b1==0x0F && (b2==0x0E || b2==0x0F) && (b4>=0x00 && b4<=0x07))
	{
                
		if(debugMode == 1)
			printf("\nSBRS instruction decoded\n"); 
		unsigned char r= (b2&1)*16+b3;
        unsigned char Rd=0;
        //finding b value 
        bin[0].arr[b4]=1;
        for(i=0;i<8;i++)
            Rd += bin[0].arr[i]*pow(2,i);//converting decimal
        //checking if Rr(b) = 1
        unsigned char k= GPR[r].data & Rd;

        if (k == Rd)
            PC += 0x04;
        else
            PC += 0x02;

        ClearBins(0);
    }

/***********************************************************************************************/
//	SBIC by VY		13/05/20
	else if(b1==0x09 && b2==0x09)
	{    
		if(debugMode == 1)
			printf("\nSBIC instruction decoded\n");
		unsigned char r= b3*2+(b4&8)%8;//   finding IO register  A
        unsigned char Rd=0;
        //finding b value
        bin[0].arr[b4 & 7]=1;
        for(i=0;i<8;i++)
            Rd += bin[0].arr[i]*pow(2,i);
        //checking id A(b)=0
        unsigned char k= IOREG[r].data & Rd;
        if (k == 0)
            PC += 0x04;
        else
            PC += 0x02;
        ClearBins(0);
    }

/***********************************************************************************************/	
//	PUSH by VY		14/05/20
	else if(b1==0x09 && (b2==0x02 || b2== 0x03) && b4==0x0F)
	{
		if(debugMode==1)
			printf("\nPUSH instruction decoded\n");
        unsigned char k=(b2&1)*16+b3;//Rd value
        SRAM[SP].data=GPR[k].data;//pushing into stack

        if(debugMode==1)
        	printf("\n SRAM[SP]: %X \n",SRAM[SP].data);
        
        SP -= 1;
        PC += 0x02;
    }

/***********************************************************************************************/	
//	POP by VY		14/05/20
	else if(b1==0x09 && (b2==0x00 || b2== 0x01) && b4==0x0F)
	{
		if(debugMode==1)
			printf("\nPOP instruction decoded\n");
        unsigned char k=(b2&1)*16+b3;//finding Rd
        GPR[k].data=SRAM[SP+1].data;//getting values from stack

        if(debugMode==1)
        	printf("\n after execution GPR[k]: %X",GPR[k].data);

        SP += 1;
        PC += 0x02;
    }


/***********************************************************************************************/	
//	RCALL by VY		14/05/20
	else if(b1==0x0D)
	{       
		if(debugMode==1)
			printf("\nRCALL instruction decoded\n");
        int k=b2*256+b3*16+b4;//k value
        SRAM[SP-1].data=PC+2;//storing next instructio PC(lsb 4 bits)
        SRAM[SP].data=(PC+2)/256;//msb 4 bits of pc

        if(debugMode==1)
            printf("\nSRAM value: %X %X",SRAM[SP].data,SRAM[SP-1].data);

        SP -= 2;
        PC += k*2 + 2;
    }

/***********************************************************************************************/	
//	RET by VY		14/05/20
	else if(b1==0x09 && b2==0x05 && b3== 0x00 && b4== 0x08)
	{
		if(debugMode==1)
			printf("\nRET instruction decoded\n");

        PC = SRAM[SP+1].data + SRAM[SP+2].data*256;//returning pc values from stack
        SP += 2;
    }


/************************************************************************************************/
//NEG  by VY         06-05-2020 modified on 18/05/2020
        else if(b1==0x09 && (b2==0x05 || b2==0x04) && b4==0x01)
        {
                unsigned char k=(b2 & 1)*16 + b3;//Rd value
        if(debugMode == 1)
            {
                printf("\nNEG instruction decoded\n");
                printf("\nBefore execution: Reg[%d]: %X",k,GPR[k].data);
            }
        GPR[k].data = 0x00-GPR[k].data;//2's complement
		//setting carry and zero flag
        if(GPR[k].data == 0x0)
                        {
            SREG[1].data = 1;
                        SREG[0].data = 0;
                        }
        else
                        {
            SREG[1].data = 0;
                        SREG[0].data = 1;
                        }
                if(GPR[k].data>=80)
                        SREG[2].data = 1;//neg flag
        else
            SREG[2].data = 0;
                if(GPR[k].data==80)
                        SREG[3].data = 1;
        else
            SREG[3].data = 0;
                
                if(SREG[3].data == SREG[2].data)
                        SREG[4].data = 0;//sign flag
        else
            SREG[4].data = 1;
                int s=GPR[k].data & 8;
                if(s == 8)
                        SREG[5].data = 1;//halfcarry
                else
            SREG[5].data = 0;
        if(debugMode == 1)
            printf("\nAfter execution: Reg[%d]: %X",k,GPR[k].data);

        PC += 0x02;
    }


/************************************************************************************************/
//  LSR by VY           6-05-2020
    else if(b1==0x09 && (b2==0x05 || b2==0x04) && b4==0x06)
    {
        unsigned char k=(b2&1)*16+b3;//Rd value
		if(debugMode == 1)
		{
			printf("\nLSR instruction decoded\n");
			printf("\nBefore execution: Reg[%d]: %X",k,GPR[k].data);
		}
		//giving Rd(0) to carry flag
        if(GPR[k].data & 0x01 == 0x01)
            SREG[0].data = 1;
		else
			SREG[0].data = 0;
        //right shiting
		GPR[k].data = GPR[k].data/0x02;
        if(GPR[k].data == 0x0)
			SREG[1].data = 1;//setting zero flag
		else
			SREG[1].data = 0;

        SREG[2].data = 0;//neg flag
        SREG[3].data = SREG[0].data;//overflow flag
        SREG[4].data = SREG[3].data;//sign flag
        if(debugMode == 1)
			printf("\nAfter execution: Reg[%d]: %X",k,GPR[k].data);

		PC += 0x02;
	}

/************************************************************************************************/
//  BSET by VY           7-05-2020
    else if(b1==0x09 && b2==0x04 && b4==0x08 && b3>=0 && b3<=7)
    {
		if(debugMode == 1)
		{
			if(b3 == 0x0)
				printf("\nSEC instruction decoded\n");
			else if(b3 == 0x01)
				printf("\nSEZ instruction decoded\n");
			else if(b3 == 0x02)
				printf("\nSEN instruction decoded\n");
			else if(b3 == 0x03)
				printf("\nSEV instruction decoded\n");
			else if(b3 == 0x04)
				printf("\nSES instruction decoded\n");
			else if(b3 == 0x05)
				printf("\nSEH instruction decoded\n");
			else if(b3 == 0x06)
				printf("\nSET instruction decoded\n");
			else if(b3 == 0x07)
				printf("\nSEI instruction decoded\n");
		}
        SREG[b3].data = 0x01;
        PC += 0x2;
	}

/************************************************************************************************/ 
//  OR by VY           12-05-2020
    else if(b1==0x02 && b2>=0X08 && b2<=0X0B)
    {
        unsigned char d= (b2 & 1)*16+b3;//Rd
        unsigned char r= (b2 & 2)*8+b4;//Rr
		if(debugMode == 1)
		{
			printf("\nOR instruction decoded\n");
            printf("\nbefore execution: Reg[%d]: %X",d,GPR[d].data);
		}
                
        GPR[d].data = GPR[d].data | GPR[r].data;//Rd OR Rr

        if(debugMode == 1)
			printf("\nAfter execution with Reg[%d]: %X = Reg[%d]: %X",r,GPR[r].data,d,GPR[d].data);

        SREG[3].data=0;//overflow flag

        if(GPR[b3+16].data>=80)
            SREG[2].data = 1;//neg flag
		else
			SREG[2].data = 0;
                
        SREG[4].data = SREG[2].data;//sign flag 

        if(GPR[b3+16].data == 0x0)//changing carry falg
			SREG[1].data = 1;
        else
            SREG[1].data = 0;
        
        PC += 0x2;
	}
/************************************************************************************************/ 
//  BCLR by VY           12-05-2020
    else if(b1==0x09 && b2==0x04 && b4==0x08 && b3>=0X08 && b3<=0X0F)
    {
		if(debugMode == 1)
		{
			if(b3 == 0x0)
				printf("\nCEC instruction decoded\n");
			else if(b3 == 0x01)
				printf("\nCEZ instruction decoded\n");
			else if(b3 == 0x02)
				printf("\nCEN instruction decoded\n");
			else if(b3 == 0x03)
				printf("\nCEV instruction decoded\n");
			else if(b3 == 0x04)
				printf("\nCES instruction decoded\n");
			else if(b3 == 0x05)
				printf("\nCEH instruction decoded\n");
			else if(b3 == 0x06)
				printf("\nCET instruction decoded\n");
			else if(b3 == 0x07)
				printf("\nCEI instruction decoded\n");
		}
        unsigned char k = b3 & 7;
        SREG[k].data = 0x00;
        
        PC += 0x2;
	}
/************************************************************************************************/  
//      BRID by VY           7-05-2020
        else if(b1==0xf && b2>=4 && b2<=7 && (b4 == 0x07 || b4 == 0x0F))
	{       
                PC=0x3A;
                int kbits[7],jump=0;
		char temp=0x0;
		if(debugMode == 1)
				printf("\nBRID instruction decoded\n");
		if(SREG[7].data == 0)//checking interupt flag
		{	
		//for k bits
		Hex2Bin(0,b2);
			Hex2Bin(1,b3);
			Hex2Bin(2,b4);
			kbits[6] = bin[0].arr[0];
			kbits[5] = bin[0].arr[1];
			for(i=0;i<4;i++)
				kbits[i+1] = bin[1].arr[i];
			kbits[0] = bin[2].arr[3];

                
                if(kbits[6] == 1)	//Signed bit set (k is negative)
			{
			   //finding 2's complement(subtract 1 find once complement)
				for(i=0;i<6;i++)
					temp += kbits[i]*pow(2,i);//decimal conversion
				temp -= 0x01;
				i=0;
				while(temp!=0 && i<=6)
				{
					kbits[i] = temp % 2;//getting binary
					i++;
					temp /= 2;
				}
				for(i=0;i<6;i++)
					kbits[i] = !kbits[i];

				for(i=0;i<6;i++)
					jump += kbits[i]*pow(2,i);
				jump *= -2;
                                printf("\n jump: %d",jump);      
			}
			else
			{
				for(i=0;i<6;i++)
					jump += kbits[i]*pow(2,i);
                                        
				jump *= 2;
                                printf("\n jump: %d",jump);
			}
			if(debugMode == 1)
				printf("\nJumping to PC: %X from %X",PC+jump+0x02,PC);
			PC += jump + 0x02;//jumping to 2k+2

		}
		else
			PC += 0x2;

	} 

/**********************************************************************************************/     
//      BRIE by VY           7-05-2020
        else if(b1==0xf && b2>=0 && b2<=3 && (b4 == 0x07 || b4 == 0x0F))
	{
               PC=0X3A;
               int kbits[7],jump=0;
		char temp=0x0;
		if(debugMode == 1)
				printf("\nBRIE instruction decoded\n");
		if(SREG[7].data == 1)//checking interept flag
		{	
		Hex2Bin(0,b2);
			Hex2Bin(1,b3);
			Hex2Bin(2,b4);
			kbits[6] = bin[0].arr[0];
			kbits[5] = bin[0].arr[1];
			for(i=0;i<4;i++)
				kbits[i+1] = bin[1].arr[i];
			kbits[0] = bin[2].arr[3];
                if(kbits[6] == 1)	//Signed bit set (k is negative)
			{
			    //finding 2's complement(subtract 1 find once complement)
				for(i=0;i<6;i++)
					temp += kbits[i]*pow(2,i);//decimal conversion
				temp -= 0x01; 
				i=0;
				while(temp!=0 && i<=6)
				{
					kbits[i] = temp % 2;
					i++;
					temp /= 2;
				}
				for(i=0;i<6;i++)
					kbits[i] = !kbits[i];

				for(i=0;i<6;i++)
					jump += kbits[i]*pow(2,i);
				jump *= -2;
			}
			else
			{
				for(i=0;i<6;i++)
					jump += kbits[i]*pow(2,i);
				jump *= 2;
			}
			if(debugMode == 1)
				printf("\nJumping to PC: %X",PC+jump+0x02);
			PC += jump + 0x02;

		}
		else
			PC += 0x2;

	}
/************************************************************************************/
//	BRLT by VY 11/05/2020
	else if(b1==0xf && b2>=0 && b2<=3 && (b4 == 0x04 || b4 == 0x0C))
	{
		int kbits[7],jump=0;
		char temp=0x0;
		if(debugMode==1)
			printf("\nBRLT instruction decoded\n");
		if(SREG[4].data == 1)//checking overflow flag
		{
			//For getting Kbits
			Hex2Bin(0,b2);
			Hex2Bin(1,b3);
			Hex2Bin(2,b4);
			kbits[6] = bin[0].arr[0];
			kbits[5] = bin[0].arr[1];
			for(i=0;i<4;i++)
				kbits[i+1] = bin[1].arr[i];
			kbits[0] = bin[2].arr[3];

			if(kbits[6] == 1)	//Signed bit set (k is negative)
			{
			    //finding 2's complement(subtract 1 find once complement)
				for(i=0;i<6;i++)
					temp += kbits[i]*pow(2,i);
				temp -= 0x01;
				i=0;
				while(temp!=0 && i<=6)
				{
					kbits[i] = temp % 2;
					i++;
					temp /= 2;
				}
				for(i=0;i<6;i++)
					kbits[i] = !kbits[i];

				for(i=0;i<6;i++)
					jump += kbits[i]*pow(2,i);
				jump *= -2;
			}
			else
			{
				for(i=0;i<6;i++)
					jump += kbits[i]*pow(2,i);
				jump *= 2;
			}
			if(debugMode == 1)
				printf("\nJumping to PC: %X",PC+jump+0x02);
			PC += jump + 0x02;

		}
		else
			PC += 0x2;

	}
/*********************************************************************************/
//	BRMI by VY 11/05/2020
	else if(b1==0xf && b2>=0 && b2<=3 && (b4 == 0x02 || b4 == 0x0A))
	{
		int kbits[7],jump=0;
		char temp=0x0;
		if(debugMode==1)
			printf("\nBRMI instruction decoded\n");
		if(SREG[2].data == 1)//checking negative flag
		{
			//For getting Kbits
			Hex2Bin(0,b2);
			Hex2Bin(1,b3);
			Hex2Bin(2,b4);
			kbits[6] = bin[0].arr[0];
			kbits[5] = bin[0].arr[1];
			for(i=0;i<4;i++)
				kbits[i+1] = bin[1].arr[i];
			kbits[0] = bin[2].arr[3];

			if(kbits[6] == 1)	//Signed bit set (k is negative)
			{
			//finding 2's complement(subtract 1 find once complement)
				for(i=0;i<6;i++)
					temp += kbits[i]*pow(2,i);
				temp -= 0x01;
				i=0;
				while(temp!=0 && i<=6)
				{
					kbits[i] = temp % 2;
					i++;
					temp /= 2;
				}
				for(i=0;i<6;i++)
					kbits[i] = !kbits[i];

				for(i=0;i<6;i++)
					jump += kbits[i]*pow(2,i);
				jump *= -2;
			}
			else
			{
				for(i=0;i<6;i++)
					jump += kbits[i]*pow(2,i);
				jump *= 2;
			}
			if(debugMode == 1)
				printf("\nJumping to PC: %X",PC+jump+0x02);
			PC += jump + 0x02;

		}
		else
			PC += 0x2;

	}
/************************************************************************************************/
//	RETI by SK, modified by SK on 13/5/2020
	else if(b1==0x9 && b2==0x5 && b3==0x1 && b4==0x8)
	{	
		if(debugMode==1)
			printf("\nRETI instruction decoded\n");
		
		PC = (SRAM[SP-1].data << 4 | SRAM[SP].data);					//storing the value of PC from the stack
	}

/************************************************************************************************/
/*//	SEZ by SK	04/05/2020		!!!COVERED IN BSET!!!
	else if(b1==0x9 && b2==4 && b3==0x1 && b4==8)
	{
		if(debugMode==1)
	    	printf("\nSEZ instruction decoded\n");

	    SREG[1].data = 1;									//setting Z flag 
	    PC += 0x2;
	}*/

/************************************************************************************************/
/*//	SET by SK	04/05/2020		!!!COVERED IN BSET!!!
	else if(b1==0x9 && b2==4 && b3==0x6 && b4==8)
	{
		if(debugMode==1)
	    	printf("\nSET instruction decoded\n");

	    SREG[6].data = 1;									//setting T flag 
	    PC += 0x2;
	}*/

/************************************************************************************************/
/*//	SEV by SK	04/05/2020		!!!COVERED IN BSET!!!
	else if(b1==0x9 && b2==4 && b3==0x3 && b4==8)
	{
		if(debugMode==1)
	    	printf("\nSEV instruction decoded\n");

	    SREG[3].data = 1;									//setting V flag 
	    PC += 0x2;
	}*/

/************************************************************************************************/
//	MOV by SK	06/05/2020
	else if(b1==0x2 && b2>=12 && b2<=15)
	{	unsigned char b4=((b2>>1)&1)*16+b4;			//setting r and d as per opcode
	    unsigned char b3=(b2&1)*16+b3;

		int a=GPR[b3].data,b=GPR[b4].data;
		if(debugMode==1)
		{
			printf("\nMOV instruction decoded\n");
			printf("\nBefore execution: Reg[%d] = %X, Reg[%d] = %X\n",b3,GPR[b3].data,b4,GPR[b4].data);
		}

		GPR[b3].data = GPR[b4].data;						//moving the data to destination location 

		if(debugMode==1)
			printf("\nAfter execution Reg[%d] = %X, Reg[%d] = %X\n",b3,GPR[b3].data,b4,GPR[b4].data);

		PC += 0x2;									//Incrementing Program Counter
	}

/************************************************************************************************/
//	LD by SK 13/05/2020
	else if(b1==0x9 && (b2==0x1 || b2==0x0))								
	{	unsigned char b4=((b2>>1)&1)*16+b4;			//setting r and d as per opcode
	    unsigned char b3=(b2&1)*16+b3;
		if(debugMode==1)
		{	
			printf("\nLD instruction decoded\n");
			printf("\nBefore execution: Reg[%d] = %X\n",b3,GPR[b3].data);
		}
		int x;										//x is the value loaded in LDI instruction
		if(b4==0xC)
			GPR[b3].data=x;							//For normal LD
		else if(b4==0xD)
		{	
			x+=1;
			GPR[b3].data=GPR[x].data;						//For LD+ condition
		}
		else if(b4==0xE)
		{	
			x-=1;
			GPR[b3].data=GPR[x].data;						//For LD- condition
		}
		if(debugMode==1)
			printf("\nAfter execution Reg[%d] = %X\n",b3,GPR[b3].data);

		PC += 0x2;
	}

/************************************************************************************************/
//	ST by SK 13/05/2020
	else if(b1==0x9 && (b2==0x3 || b2==0x2))								
	{	unsigned char b4=((b2>>1)&1)*16+b4;			//setting r and d as per opcode
	    unsigned char b3=(b2&1)*16+b3;

		int x;										//x is the value loaded in LDI instruction
		if(debugMode==1)
		{	
			printf("\nST instruction decoded\n");
			printf("\nBefore execution: Reg[%d] = %X\n",x,GPR[x].data);
		}
		if(b4==0xC)
			GPR[x].data=GPR[b3].data;						//For normal ST
		else if(b4==0xD)
		{	
			x+=1;
			GPR[x].data=GPR[b3].data;						//For normal ST+
		}
		else if(b4==0xE)
		{	
			x-=1;
			GPR[x].data=GPR[b3].data;;					//For normal ST-
		}
		if(debugMode==1)
		{
			printf("\nAfter execution Reg[%d] = %X\n",x,GPR[x].data);
		}
		PC += 0x2;
	}

/************************************************************************************************/
//	STS by SK 13/05/2020
	else if(b1==0xA && (b2>=0x8 && b2<=0x16))							 				
	{	unsigned char b4=((b2>>1)&1)*16+b4;			//setting r and d as per opcode
	    unsigned char b3=(b2&1)*16+b3;

		int x;
		if(debugMode==1)
		{	
			printf("\nSTS instruction decoded\n");
			printf("\nBefore execution: Reg[%d] = %X\n", b4,GPR[b4].data);
		}
		GPR[b4].data=GPR[b3].data;						//Storing the data to a particular location
		x=b4;
		if(debugMode==1)
			printf("\nAfter execution Reg[%d] = %X\n",b4,GPR[b4].data);
		PC += 0x2;
	}
/************************************************************************************************/
//	LDS by SK 13/05/2020
	else if(b1==0xA && (b2>=0x0 && b2<=0x7))							 				
	{	unsigned char b4=((b2>>1)&1)*16+b4;			//setting r and d as per opcode
	    unsigned char b3=(b2&1)*16+b3;

		int x;
		if(debugMode==1)
			printf("\nLDS instruction decoded\n");
		GPR[b3].data = GPR[b4].data;						//Loading the data to a particular location
		x=b4;
		PC += 0x2;
	}

/************************************************************************************************/
//	ROR by SK 13/05/2020
	else if(b1==0x9 && (b2==0x4 || b2==0x5) && b4==0x07)							 				
	{	unsigned char b4=((b2>>1)&1)*16+b4;			//setting r and d as per opcode
	    unsigned char b3=(b2&1)*16+b3;
		int t;GPR[b3].data=0x55;
		if(debugMode==1)
		{  	
			printf("\nROR instruction decoded\n");
			printf("\nBefore execution: Reg[%d] = %X\n",b3,GPR[b3].data);
		}
		t=0x01 & GPR[b3].data;
		GPR[b3].data = GPR[b3].data>>1;		//Rotating Right
		if(SREG[0].data==1)
		GPR[b3].data = 0x80 | GPR[b3].data;
		SREG[0].data=t;					//updating C flag

		if(GPR[b3].data == 0x0)			//updating Z flag
			SREG[1].data = 1;
		else
			SREG[1].data = 0;

		if(GPR[b3].data>=0x08)			//updating N flag
			SREG[2].data = 1;
		else  	
			SREG[2].data = 0;

		SREG[3].data = SREG[0].data^SREG[2].data;	//updating V flag

		SREG[4].data = SREG[3].data^SREG[2].data;	//updating S flag

		if(debugMode==1)
		{
			printf("\nAfter execution Reg[%d] = %X\n",b3,GPR[b3].data);
		}
		PC += 0x2;
	}

/************************************************************************************************/
//	SBIS by SK 13/05/2020
	else if(b1==0x9 && b2==0x0B)									
	{	
		IOREG[((0x7 & b3)<<1|(0x8 & b4))].data=0x55;
		int temp=0x01;
		if(debugMode==1)
			printf("SBIS instruction decoded\n");
		temp=temp<<(0x7 & b4);	

		if((IOREG[((0x7 & b3)<<1|(0x8 & b4))].data & temp)!=0)				//Skipping the next instruction if a bit is set
			PC += 0x4;
		else
			PC+=0x02;
	}


/************************************************************************************************/
//BLD by SK, modified by SK on 7/5/2020
	else if(b1==0xf && b2>=8 && b2<=9 && b4<=7)
	{	unsigned char b4=((b2>>1)&1)*16+b4;			//setting r and d as per opcode
	    unsigned char b3=(b2&1)*16+b3;

		int a=GPR[b3].data,mask=0;
		if(debugMode==1)
		{
			printf("\nBLD instruction decoded\n");
			printf("\nBefore execution: Reg[%d] = %X\n",b3,GPR[b3].data);
		}

		mask=1<<b4;							//masking the bits
			 
		GPR[b3].data =(GPR[b3].data & ~mask)|(SREG[6].data<<b4) ; //Bit Load from the T Flag in SREG to a Bit in Register
		if(debugMode==1)
			printf("\nAfter execution Reg[%d] = %X\n",b3,GPR[b3].data);

		PC += 0x2;
	}

/************************************************************************************************/
//BST by SK, modified by SK on 7/5/2020
	else if(b1==0xf && b2>=0xA && b2<=0xB && b4<=7)
	{	unsigned char b4=((b2>>1)&1)*16+b4;			//setting r and d as per opcode
	    unsigned char b3=(b2&1)*16+b3;

		int a=GPR[b3].data;
		if(debugMode==1)
		{
			printf("\nBST instruction decoded\n");
			printf("\nBefore execution: Reg[%d] = %X\n",b3,GPR[b3].data);
		}
			 
		SREG[6].data = 1 & (GPR[b3].data>>(b4)) ;				//Bit Store from Bit in Register to T Flag in SREG
		PC += 0x2;
	}

/************************************************************************************************/
//	BRPL by SK  on 13/5/2020
	else if(b1==0xf && b2>=4 && b2<=7 && (b4==0x02||b4==0x0A))
	{	
		int kbits[7],jump=0;
		char temp=0x0;
		if(debugMode==1)
		printf("\nBRPL instruction decoded\n");
		if(SREG[2].data ==0)					//Branching if N is negative
		{
			//For getting Kbits
			Hex2Bin(0,b2);
			Hex2Bin(1,b3);
			Hex2Bin(2,b4);
			kbits[6] = bin[0].arr[0];
			kbits[5] = bin[0].arr[1];
			for(i=0;i<4;i++)
				kbits[i+1] = bin[1].arr[i];
			kbits[0] = bin[2].arr[3];

			if(kbits[6] == 1)	//Signed bit set (k is negative)
			{
				for(i=0;i<6;i++)
					temp += kbits[i]*pow(2,i);
				temp -= 0x01;
				i=0;
				while(temp!=0 && i<=6)
				{
					kbits[i] = temp % 2;
					i++;
					temp /= 2;
				}
				for(i=0;i<6;i++)
					kbits[i] = !kbits[i];

				for(i=0;i<6;i++)
					jump += kbits[i]*pow(2,i);
				jump *= -2;
			}
			else
			{
				for(i=0;i<6;i++)
					jump += kbits[i]*pow(2,i);
				jump *= 2;
			}
			if(debugMode == 1)
				printf("\nJumping from PC:%X to PC: %X",PC,PC+jump+0x02);
			PC += jump + 0x02;

		}

		else
			PC += 0x2;

		ClearBins(0); ClearBins(1); ClearBins(2);

	}

/************************************************************************************************/
//	BRBC by SK, modified by SK on 7/5/2020
	else if(b1==0xf && b2>=4 && b2<=7)
	{
		int kbits[7],jump=0,l=0;
		char temp=0x0;
		l=b4%0x8;
		if(debugMode == 1)				
		{
			if(l == 0x0)
				printf("\nBRCC instruction decoded\n");
			else if(l == 0x01)
				printf("\nBRNE instruction decoded\n");
			else if(l == 0x02)
				printf("\nBRPL instruction decoded\n");
			else if(l == 0x03)
				printf("\nBRVC instruction decoded\n");
			else if(l == 0x04)
				printf("\nBRGE instruction decoded\n");
			else if(l == 0x05)
				printf("\nBRHC instruction decoded\n");
			else if(l == 0x06)
				printf("\nBRTC instruction decoded\n");
			else if(l == 0x07)
				printf("\nBRID instruction decoded\n");
		}
		if(SREG[l].data == 0)				//Branch if a Bit in SREG is Cleared
		{
			//For getting Kbits
			Hex2Bin(0,b2);
			Hex2Bin(1,b3);
			Hex2Bin(2,b4);
			kbits[6] = bin[0].arr[0];
			kbits[5] = bin[0].arr[1];
			for(i=0;i<4;i++)
				kbits[i+1] = bin[1].arr[i];
			kbits[0] = bin[2].arr[3];

			if(kbits[6] == 1)	//Signed bit set (k is negative)
			{
				for(i=0;i<6;i++)
					temp += kbits[i]*pow(2,i);
				temp -= 0x01;
				i=0;
				while(temp!=0 && i<=6)
				{
					kbits[i] = temp % 2;
					i++;
					temp /= 2;
				}
				for(i=0;i<6;i++)
					kbits[i] = !kbits[i];

				for(i=0;i<6;i++)
					jump += kbits[i]*pow(2,i);
				jump *= -2;
			}
			else
			{
				for(i=0;i<6;i++)
					jump += kbits[i]*pow(2,i);
				jump *= 2;
			}
			if(debugMode == 1)
				printf("\nJumping from PC:%X to PC: %X",PC,PC+jump+0x02);
			PC += jump + 0x02;

		}
		else
			PC += 0x2;

		ClearBins(0); ClearBins(1); ClearBins(2);

	}

/************************************************************************************************/
//	BRGE by SK, modified by SK on 11/5/2020
	else if(b1==0xf && b2>=4 && b2<=7 && (b4==4||b4==12))
	{
		int kbits[7],jump=0;
		char temp=0x0;
		if(debugMode==1)
		printf("\nBRGE instruction decoded\n");
		if(SREG[4].data ==0)			//Branching if signed bit is reset
		{
			//For getting Kbits
			Hex2Bin(0,b2);
			Hex2Bin(1,b3);
			Hex2Bin(2,b4);
			kbits[6] = bin[0].arr[0];
			kbits[5] = bin[0].arr[1];
			for(i=0;i<4;i++)
				kbits[i+1] = bin[1].arr[i];
			kbits[0] = bin[2].arr[3];

			if(kbits[6] == 1)	//Signed bit set (k is negative)
			{
				for(i=0;i<6;i++)
					temp += kbits[i]*pow(2,i);
				temp -= 0x01;
				i=0;
				while(temp!=0 && i<=6)
				{
					kbits[i] = temp % 2;
					i++;
					temp /= 2;
				}
				for(i=0;i<6;i++)
					kbits[i] = !kbits[i];

				for(i=0;i<6;i++)
					jump += kbits[i]*pow(2,i);
				jump *= -2;
			}
			else
			{
				for(i=0;i<6;i++)
					jump += kbits[i]*pow(2,i);
				jump *= 2;
			}
			if(debugMode == 1)
				printf("\nJumping from PC:%X to PC: %X",PC,PC+jump+0x02);
			PC += jump + 0x02;

		}
		else
			PC += 0x2;

	ClearBins(0); ClearBins(1); ClearBins(2);

	}

/************************************************************************************************/
//	BRBS by SK, modified by SK on 7/5/2020
	else if(b1==0xf && b2>=0 && b2<=3)
	{
		int kbits[7],jump=0,l=0;
		char temp=0x0;
		l=b4%0x8;		//extracting the bit postion to be checked
		if(debugMode == 1)
		{
			if(l == 0x0)
				printf("\nBRCS instruction decoded\n");
			else if(l == 0x01)
				printf("\nBREQ instruction decoded\n");
			else if(l == 0x02)
				printf("\nBRMI instruction decoded\n");
			else if(l == 0x03)
				printf("\nBRVS instruction decoded\n");
			else if(l == 0x04)
				printf("\nBRLT instruction decoded\n");
			else if(l == 0x05)
				printf("\nBRHS instruction decoded\n");
			else if(l == 0x06)
				printf("\nBRTS instruction decoded\n");
			else if(l == 0x07)
				printf("\nBRIE instruction decoded\n");
		}
		if(SREG[l].data == 1)				//Branch if Bit in SREG is Set
		{
			//For getting Kbits
			Hex2Bin(0,b2);
			Hex2Bin(1,b3);
			Hex2Bin(2,b4);
			kbits[6] = bin[0].arr[0];
			kbits[5] = bin[0].arr[1];
			for(i=0;i<4;i++)
				kbits[i+1] = bin[1].arr[i];
			kbits[0] = bin[2].arr[3];

			if(kbits[6] == 1)	//Signed bit set (k is negative)
			{
				for(i=0;i<6;i++)
					temp += kbits[i]*pow(2,i);
				temp -= 0x01;
				i=0;
				while(temp!=0 && i<=6)
				{
					kbits[i] = temp % 2;
					i++;
					temp /= 2;
				}
				for(i=0;i<6;i++)
					kbits[i] = !kbits[i];

				for(i=0;i<6;i++)
					jump += kbits[i]*pow(2,i);
				jump *= -2;
			}
			else
			{
				for(i=0;i<6;i++)
					jump += kbits[i]*pow(2,i);
				jump *= 2;
			}
			if(debugMode == 1)
				printf("\nJumping from PC:%X to PC: %X",PC,PC+jump+0x02);
			PC += jump + 0x02;

		}
		else
			PC += 0x2;

	}

/************************************************************************************************/
//	SWAP BY SK		11/05/2020
	else if(b1==0x9 && (b2==0x4 || b2==0x5) && b4==0x2)
	{	unsigned char b4=((b2>>1)&1)*16+b4;			//setting r and d as per opcode
	    unsigned char b3=(b2&1)*16+b3;

		int temp;
		if(debugMode == 1)
			{
				printf("\nSWAP instruction decoded\n");
				printf("\nBefore execution: Reg[%d]: %X",b3,GPR[b3].data);
			}
		temp=GPR[b3].data;
		GPR[b3].data=((temp & 0x0F)<<4) | ((temp &0xF0)>>4);	// Swapping the nibbles
		if(debugMode == 1)
			printf("\nAfter execution: Reg[%d]: %X",b3,GPR[b3].data);
		
			PC += 0x02;
	}

/************************************************************************************************/
//	INC BY SK		11/05/2020
	else if(b1==0x09 && (b2==0x05 || b2==0x04) && b4==0x03)
	{	unsigned char b4=((b2>>1)&1)*16+b4;			//setting r and d as per opcode
	    unsigned char b3=(b2&1)*16+b3;

		if(debugMode == 1)
			{
				printf("\nINC instruction decoded\n");
				printf("\nBefore execution: Reg[%d]: %X",b3,GPR[b3].data);
			}
		
		if(GPR[b3].data==0x7F)			//updating V flag
			SREG[3].data = 1;
		else  	
			SREG[3].data = 0;

		GPR[b3].data += 0x01;			//incrementing data

		if(GPR[b3].data == 0x0)			//updating Z flag
			SREG[1].data = 1;
		else
			SREG[1].data = 0;

		if(GPR[b3].data>=0x08)			//updating N flag
			SREG[2].data = 1;
		else  	
			SREG[2].data = 0;

		SREG[4].data = SREG[3].data^SREG[2].data;	//updating S flag
		
		if(debugMode == 1)
			printf("\nAfter execution: Reg[%d]: %X",b3,GPR[b3].data);

		PC += 0x02;
	}


/************************************************************************************************/
//	NOP by AJ		date
	else if(b1==0x0 && b2==0x0 && b3==0x0 && b4==0x0)
	{
		if(debugMode == 1)
			printf("\nNOP instruction decoded\n");
		PC += 0x2;
	}

	timer(1);
	timer1(1);
	SetPins(1);
}


void output(int flag)			//Function to compute output for current instruction
{
	if(flag == 1)
	{
		printf("\nPC: %X\n",PC);
		Compute();
		if(debugMode==1)
			PrintSREG();
	}
}


