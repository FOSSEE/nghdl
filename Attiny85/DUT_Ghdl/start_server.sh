#!/bin/bash

###This server run ghdl testebench for infinite time till ngspice send END signal to stop it

cd ~/ngspice-nghdl/src/xspice/icm/ghdl/attiny_85_nghdl/DUTghdl/ 
chmod 775 sock_pkg_create.sh &&
./sock_pkg_create.sh $1 $2 &&
ghdl -a sock_pkg.vhdl &&

#---------------------------------------------------------------------------------------------
#---------------------------------------------------------------------------------------------
### The following lines (till line 23) are added by Ashutosh Jha
### Date - 3/3/2020

gcc -c tiny85_c.c -o tiny85_c.o &&   #@!
# Compiles and generates object file of microcontroller C code

ghdl -a ghdl_access.vhdl attiny_85_nghdl.vhdl &&
# Compiles and generates object files of VHDL code of helper function and the main model respectively

mv attiny_85_nghdl.o attiny_85_nghdl_temp.o && #explained below
ld -r -o attiny_85_nghdl.o tiny85_c.o attiny_85_nghdl_temp.o && #name of the object file of VHDL code & C code should be same as that of VHDl code uploaded via NGHDL.
rm attiny_85_nghdl_temp.o &&
# The object files of main VHDL and microcontroller C code need to be linked.
# The above three commands do that #@!
#---------------------------------------------------------------------------------------------
#---------------------------------------------------------------------------------------------

ghdl -a attiny_85_nghdl_tb.vhdl  &&
ghdl -e -Wl,ghdlserver.o attiny_85_nghdl_tb &&
./attiny_85_nghdl_tb
