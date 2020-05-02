#!/bin/bash

###This server run ghdl testebench for infinite time till ngspice send END signal to stop it

cd ~/ngspice-nghdl/src/xspice/icm/ghdl/attiny85_exp/DUTghdl/
chmod 775 sock_pkg_create.sh &&
./sock_pkg_create.sh $1 $2 &&
ghdl -a sock_pkg.vhdl &&

### The following lines (till line 23) are added by Ashutosh Jha
### Date - 3/3/2020

gcc -c tiny85_c.c -o tiny85_c.o &&  
# Compiles and generates object file of microcontroller C code

ghdl -a ghdl_access.vhdl attiny85_exp.vhdl &&
# Compiles and generates object files of VHDL code of helper function and the main model respectively

mv attiny85_exp.o attiny85_exp1.o &&
ld -r -o attiny85_exp.o tiny85_c.o attiny85_exp1.o &&
rm attiny85_exp1.o &&
# The object files of main VHDL and microcontroller C code need to be linked.
# The above three commands do that

ghdl -a attiny85_exp_tb.vhdl  &&
ghdl -e -Wl,ghdlserver.o attiny85_exp_tb &&
./attiny85_exp_tb