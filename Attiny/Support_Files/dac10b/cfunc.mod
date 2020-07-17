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

int decimal_dac,resolution=10,adc_up=5,i;
double out,step_dac;


           
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


/*=== CM_DAC10B ROUTINE ===*/
                                                   

void cm_dac10b(ARGS)   /* structure holding parms, inputs, outputs, etc.     */
{   
    step_dac = pow(2,resolution);
    decimal_dac = (int)(INPUT(d_in0)*pow(2,0)) + (int)(INPUT(d_in1)*pow(2,1)) +
                (int)(INPUT(d_in2)*pow(2,2)) + (int)(INPUT(d_in3)*pow(2,3)) +
                (int)(INPUT(d_in4)*pow(2,4)) + (int)(INPUT(d_in5)*pow(2,5)) +
                (int)(INPUT(d_in6)*pow(2,6)) + (int)(INPUT(d_in7)*pow(2,7)) +
                (int)(INPUT(d_in8)*pow(2,8)) + (int)(INPUT(d_in9)*pow(2,9));
    out = decimal_dac*5/step_dac;
    if(ANALYSIS != MIF_AC)
    {
        OUTPUT(a_out) = out;
    }
    else {
    }

}
