# Code Documentation (GHDL Side)

This documentation will help you to know about the code written for communication between server and Digital logic written in VHDL

## Files generated automatically for GHDL side

* modelname_tb.vhdl
* Note: where modelname will be the name of your model

## modelname_tb.vhdl  

* This is a testbench created to send and receive values from server and gives the input values to the digital design.
* It uses VHPI library of VHDL, by including this library we can use functions written in C program, in VHDL.

### Functions used in modelname_tb.vhdl
#### Vhpi_Initialize
