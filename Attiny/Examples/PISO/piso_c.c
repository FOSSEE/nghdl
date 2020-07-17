int pin1,pin2,pin3,pin4;

int * get_ptr() {
  return &pin1;
}

int * get_ptr1() {
  return &pin2;
}

int * get_ptr2() {
  return &pin3;
}

int * get_ptr3() {
  return &pin4;
}


void output(int flag)
{
	if(flag==0)
		{
			pin1=0;
			pin2=1;
			pin3=0;
			pin4=1;
		}
	else if(flag==1)
		{
			pin1=1;
			pin2=0;
			pin3=1;
			pin4=0;
		}
	else if(flag==2)
		{
			pin1=0;
			pin2=1;
			pin3=1;
			pin4=0;
		}
	else if(flag==3)
		{
			pin1=1;
			pin2=1;
			pin3=0;
			pin4=1;
		}
}