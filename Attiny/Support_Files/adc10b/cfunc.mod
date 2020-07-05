/*.......1.........2.........3.........4.........5.........6.........7.........8
================================================================================

FILE gain/cfunc.mod

Copyright 1991
Georgia Tech Research Corporation, Atlanta, Ga. 30332
All Rights Reserved

PROJECT A-8503-405
               

AUTHORS                      

    6 Jun 1991     Jeffrey P. Murray


MODIFICATIONS   

     2 Oct 1991    Jeffrey P. Murray
                                   

SUMMARY

    This file contains the model-specific routines used to
    functionally describe the gain code model.


INTERFACES       

    FILE                 ROUTINE CALLED     

    N/A                  N/A                     


REFERENCED FILES

    Inputs from and outputs to ARGS structure.
                     

NON-STANDARD FEATURES

    NONE

===============================================================================*/

/*=== INCLUDE FILES ====================*/


#include<math.h>                    

/*=== CONSTANTS ========================*/


/*=== MACROS ===========================*/



  
/*=== LOCAL VARIABLES & TYPEDEFS =======*/                         

int curr_step,bin[10],i,temp,decimal;
double in,step=5.0/pow(2,10);




           
/*=== FUNCTION PROTOTYPE DEFINITIONS ===*/




                   
/*==============================================================================

FUNCTION void cm_gain()

AUTHORS                      

     2 Oct 1991     Jeffrey P. Murray

MODIFICATIONS   

    NONE

SUMMARY

    This function implements the gain code model.

INTERFACES       

    FILE                 ROUTINE CALLED     

    N/A                  N/A


RETURNED VALUE
    
    Returns inputs and outputs via ARGS structure.

GLOBAL VARIABLES
    
    NONE

NON-STANDARD FEATURES

    NONE

==============================================================================*/


/*=== CM_ADC10B ROUTINE ===*/
                                                   

void cm_adc10b(ARGS)   /* structure holding parms, inputs, outputs, etc.     */
{
    //Mif_Complex_t ac_gain;

    in = INPUT(a_in);
    decimal = (int)(in/step);

    /*FOR DEBUGGING
    printf("in: %lf; dec: %d\n",in,decimal);*/

    temp = in;
    i=0;
    while(decimal!=0)
    {
        bin[i] = decimal % 2;
        i++;
        decimal /= 2;
    }
    /*FOR DEBUGGING
    printf("bin:\t");
    for(i=0;i<10;i++)
    printf("%d ",bin[i]);
    printf("\n");*/

    if(ANALYSIS != MIF_AC)
    {	
    	OUTPUT(d_out0) = bin[0]*1.0;
        OUTPUT(d_out1) = bin[1]*1.0;
        OUTPUT(d_out2) = bin[2]*1.0;
        OUTPUT(d_out3) = bin[3]*1.0;
        OUTPUT(d_out4) = bin[4]*1.0;
        OUTPUT(d_out5) = bin[5]*1.0;
        OUTPUT(d_out6) = bin[6]*1.0;
        OUTPUT(d_out7) = bin[7]*1.0;
        OUTPUT(d_out8) = bin[8]*1.0;
        OUTPUT(d_out9) = bin[9]*1.0;
    }
    else {
    }

}
