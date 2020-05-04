/* This C Code for ATTINY series (specifically ATTINY85) was developed by ASHUTOSH JHA
   Further modifications by Saurabh Bansode
   Latest edit - 8:00 PM, 3/5/2020 by AJ - 
   1.	Changed how the way RAM works to most accurate depiction
   2.	Modified BRNE,RJMP instructions to most accurate
   3.	Added I/O register file
   4.	Changed reg to GPR (General Purpose Register), RAM to prog_mem
   5. 	Added Timer0 FastPWM inverting Mode

   **NOTE	:-	The functions "MapToRam", "output" and "input" &
   				the variables PB0 ... PB5 
   				linked to the VHDL code of ATTINY85
   				by "ghdl_access.vhdl" file.	*/ 

// ************!!    LATEST    ************!!    

#include<stdio.h>
#include<math.h>
#define size 8192		//8kb ram size for attiny 85

int debugMode=1;
int PB0,PB1,PB2,PB3,PB4,PB5,wait_Clocks=0;
char PC=0;
struct memory			//Structure to store RAM and other registers
{
	unsigned char data;
}prog_mem[size],GPR[32],SREG[8],IOREG[64];

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
	int i=0,filesize,j;
	SetRam(0,size,0x0);
	if(flag==1)
	{
		FILE *fptr;
		unsigned char c,t=0x0;
		fptr = fopen("hex.txt", "r");

		fseek(fptr, 0L, SEEK_END);
	    filesize = ftell(fptr);

		rewind(fptr);
		c = fgetc(fptr); 
	    while (c != EOF && i<filesize) 
	    {
	    	for(j=1;j>=0;j--)
	    	{
		    	// to skip newline and ':' character in file
		    	if(c == '\n' || c == ':')
		    		c = fgetc(fptr);
		    	// the ascii eqivalent of char c is converted to hex
		    	if(c>=48 && c<=57)
		        	c-=48;
		    	else if(c>=65 && c<=70)
		       		c-=55;
		    	else if(c>=97 && c<=102)
		        	c-=87;
		        t += c*pow(16,j);
		        if(j==1)
		        	c = fgetc(fptr);
		    }
	        prog_mem[i].data = t;
	        t = 0x0;
	        i++;
	        c = fgetc(fptr); 
	    }
	    fclose(fptr);
	}
	for(i=0;i<8;i++)
		SREG[i].data = 0;

	if(debugMode==1)
		PrintRam(0,filesize);
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
// TIMER0 by AJ		03/05/20
	unsigned char temp = 0x0;	
	if(IOREG[0x2A].data >= 0x01)
	{
		if(debugMode == 1)
			printf("\n**!!TIMER 0 operation detected!!**\n");
		int FastMode = 0,OC0A_Con = 0;
		Hex2Bin(3,IOREG[0x2A].data);

		//If TOV0 is set, reset counter TCNT0
		temp = IOREG[0x38].data & 0x02;
		if(temp == 0x02)
			{
				printf("\n**!!TOV0 overflow!!**\n");
				IOREG[0x32].data = 0;
				IOREG[0x38].data = IOREG[0x38].data ^ 0x02;
				printf("\nTIFR after toggling TOV0: %X\n",IOREG[0x38].data);
				IOREG[0x38].data = IOREG[0x38].data & 0xEF;
				printf("\nTIFR after resetting OCF0A: %X\n",IOREG[0x38].data);
			}

		if(bin[3].arr[0] == 1 && bin[3].arr[0]==1)
			FastMode = 1;
		else
			FastMode = 0;

		if(bin[3].arr[7] == 1 && bin[3].arr[6] == 0)
			OC0A_Con = 1;
		else
			OC0A_Con = 0;

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

		printf("\nTCCNT0 : %X ,OCR0A: %X\n",IOREG[0x32].data,IOREG[0x29].data);
		SetPins(1);

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

//	ADD by AJ		date
	if(b1==0x0 && b2>=12 && b2<=15)
	{	
		int a=GPR[b3+16].data,b=GPR[b4+16].data;
		if(debugMode==1)
		{
			printf("ADD instruction decoded\n");
			printf("\nBefore execution: Reg[%d] = %x\n",b3+16,GPR[b3+16].data);
		}

		ClearBins(0);
		Hex2Bin(0,a); 
		ClearBins(1);
		Hex2Bin(1,b);
		ClearBins(2);

		Bin_Add(0,1,2,0,0);

	    UpdateSreg();

		GPR[b3+16].data += GPR[b4+16].data;

		if(debugMode==1)
		{
			printf("\nAfter execution Reg[%d] = %x\n",b3+16,GPR[b3+16].data);
		}
		PC += 0x2;
	}

/************************************************************************************************/	
//	ADC by AJ		date
	else if(b1==0x1 && b2>=12 && b2<=15)
	{
		
		int a=GPR[b3+16].data,b=GPR[b4+16].data;
		if(debugMode==1)
		{
			PrintReg(15,32);
			printf("ADC instruction decoded\n");
		}

		ClearBins(0);
		Hex2Bin(0,a); 
		ClearBins(1);
		Hex2Bin(1,b);
		ClearBins(2);

		Bin_Add(0,1,2,0,1);

	    UpdateSreg();

		GPR[b3+16].data += GPR[b4+16].data + SREG[0].data;

		if(debugMode==1)
		{
			printf("\nAfter Operation - \n");
			PrintReg(15,32);
		}
		PC += 0x2;
	}

/************************************************************************************************/
//	SUB by AJ		date
	else if(b1==0x1 && b2 >= 8 && b2 <= 11)
	{
		int a=GPR[b3+16].data,b=GPR[b4+16].data;
		if(debugMode==1)
		{
			PrintReg(15,32);
			printf("SUB instruction decoded\n");
		}
		ClearBins(0);
		Hex2Bin(0,a); 
		ClearBins(1);
		Hex2Bin(1,b);
		TwosComp(1);
		ClearBins(2);

		Bin_Add(0,1,2,1,0);

		UpdateSreg();

		GPR[b3+16].data -= GPR[b4+16].data;
		if(debugMode==1)
		{
			printf("\nAfter Operation - \n");
			PrintReg(15,32);
		}
		PC += 0x2;
	}

/************************************************************************************************/
//	SUBI by AJ		date
	else if(b1==0x5)
	{
		int a=b2*16 + b4,b=GPR[b3+16].data;
		if(debugMode==1)
		{
			printf("SUBI instruction decoded\n");
			PrintReg(15,32);
		}

		ClearBins(0);
		Hex2Bin(0,a); 
		ClearBins(1);
		Hex2Bin(1,b);
		TwosComp(0);
		ClearBins(2);

		Bin_Add(1,0,2,1,0);

		UpdateSreg();

		GPR[b3+16].data -= a;
		if(debugMode==1)
		{
			printf("\nAfter Operation - \n");
			PrintReg(15,32);
		}
		PC += 0x2;

	}

/************************************************************************************************/
//	SBCI by AJ		date
	else if(b1==0x4)
	{
		int a=b2*16 + b4,b=GPR[b3+16].data;
		if(debugMode==1)
		{
			printf("SBCI instruction decoded\n");
			PrintReg(15,32);
		}

		ClearBins(0);
		Hex2Bin(0,a); 
		ClearBins(1);
		Hex2Bin(1,b);
		TwosComp(0);
		ClearBins(2);

		Bin_Add(1,0,2,1,1);

		UpdateSreg();

		GPR[b3+16].data -= a - SREG[0].data;
		if(debugMode==1)
		{
			printf("\nAfter Operation - \n");
			PrintReg(15,32);
		}
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
	else if(b1==0x07)
	{
		unsigned char k = b2*16 + b4;
		if(debugMode == 1)
		{
			printf("\nANDI instruction decoded\n");
			printf("\nReg[%d](%X) or %X = %X\n",b3+16,GPR[b3+16].data,k,GPR[b3+16].data&k);
		}
		GPR[b3+16].data = GPR[b3+16].data & k;
		PC += 0x02;
	}

/************************************************************************************************/


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
//	CLC by AJ & SB	14/03/20
	else if(b1==0x9 && b2==4 && b3==8 && b4==8)
	{
		if(debugMode==1)
	    printf("CLC instruction decoded\n");
	    SREG[0].data = 0;
	    PC += 0x2;
	}
/************************************************************************************************/	
//	CP by SB		27/03/20
//	Modified by AJ	5/4/20
	else if(b1==0x1 && b2>=4 && b2<=7)
	{
		if(debugMode==1)
	    	printf("CP instruction decoded\n");
	    //Comparing Rd and Rr (Reg data doesn't have to be modified)
	    if(GPR[b3+16].data - GPR[b4+16].data == 0)
	    	SREG[0].data = 0;

	    PC += 0x2;
	}
/************************************************************************************************/	
//	CLH by SB		30/03/20
//	Modified by AJ	5/4/20
	else if(b1==0x9 && b2==4 && b3==0xD && b4==8)
	{
		if(debugMode==1)
	    printf("CLH instruction decoded\n");
	    SREG[5].data = 0;
	    PC += 0x2;
	}	

/************************************************************************************************/	
//	CLI by SB		30/03/20
//	Modified by AJ	5/4/20
	else if(b1==0x9 && b2==4 && b3==0xF && b4==8)
	{
	if(debugMode==1)
		printf("CLI instruction decoded\n");
	SREG[7].data = 0;
	PC += 0x2;
	}	

/************************************************************************************************/	
//	CLN by SB		30/03/20
//	Modified by AJ	5/4/20
	else if(b1==0x9 && b2==4 && b3==0xA && b4==8)
	{
		if(debugMode==1)
	    	printf("CLN instruction decoded\n");
	    SREG[5].data = 0;
	    PC += 0x2;
	}	

/************************************************************************************************/	
//	CLZ by SB		30/03/20
//	Modified by AJ	5/4/20
	else if(b1==0x9 && b2==4 && b3==0x9 && b4==8)
	{
		if(debugMode==1)
	    printf("CLZ instruction decoded\n");
	    SREG[1].data = 0;
	    PC += 0x2;
	}	

/************************************************************************************************/	
//	CLS by SB 		30/03/20
//	Modified by AJ	5/4/20
	else if(b1==0x9 && b2==4 && b3==0xC && b4==8)
	{
		if(debugMode==1)
	    	printf("CLS instruction decoded\n");
	    SREG[4].data = 0;
	    PC += 0x2;
	}

/************************************************************************************************/	
//	CLT by SB		30/03/20
//	Modified by AJ	5/4/20
	else if(b1==0x9 && b2==4 && b3==0xE && b4==8)
	{
		if(debugMode==1)
	    	printf("CLT instruction decoded\n");
	    SREG[6].data = 0;
	    PC += 0x2;
	}

/************************************************************************************************/	
//	CLV by SB		30/03/20
//	Modified by AJ	5/4/20
	else if(b1==0x9 && b2==4 && b3==0xB && b4==8)
	{
		if(debugMode==1)
	    	printf("CLV instruction decoded\n");
	    SREG[3].data = 0;
	    PC += 0x2;
	}


/************************************************************************************************/
//	SEC by SB & AJ	14/03/2020
	else if(b1==0x9 && b2==4 && b3==0 && b4==8)
	{
	   if(debugMode==1)
	   		printf("\nSEC instruction decoded\n");
	   SREG[0].data = 1;
	   PC += 0x2;
	}

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
	else if(b1==0x03)
	{
	   if(debugMode==1)
	    printf("\nCPI instruction decoded\n");
		unsigned char k = b2*16 + b4;
		if(debugMode == 1)
				printf("\n%X - %X = %d\n",GPR[b3+16].data,k,GPR[b3+16].data - k);

		if(GPR[b3+16].data == k)
			{
				SREG[1].data = 1;
				if(debugMode == 1)
					printf("\nZero flag set: %d\n",SREG[1].data);
			}
		else if(GPR[b3+16].data != k)
			{
				SREG[1].data = 0;
				if(debugMode == 1)
					printf("\nZero flag reset\n");
			}
	    PC += 0x2;
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
//	RJMP by AJ		date
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

		if(kbits[11] == 1)	//
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
//	BRNE by AJ, modified by AJ on 30/4/2020
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
	/*else if(b1==0xf && b2>=3 && (b4==0x01||b4==0x09))
	{
		if(debugMode == 1)
			printf("\nBREQ instruction decoded\n");
		if(SREG[1].data == 1)
			{
				PC = 0x34;
				printf("\nJumping to: %X",PC+b3*16+b4);
				PC += b3*16+b4;
			}
		else
			PC += 0x2;

	}*/

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
//	NOP by AJ		date
	else if(b1==0x0 && b2==0x0 && b3==0x0 && b4==0x0)
	{
		if(debugMode == 1)
			printf("\nNOP instruction decoded\n");
		PC += 0x2;
	}

	timer(1);
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
