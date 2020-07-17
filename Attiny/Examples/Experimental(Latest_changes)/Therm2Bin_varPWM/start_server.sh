#!/bin/bash

###This server run ghdl testebench for infinite time till ngspice send END signal to stop it

cd ~/ngspice-nghdl/src/xspice/icm/ghdl/at85_latest/DUTghdl/
chmod 775 sock_pkg_create.sh &&
./sock_pkg_create.sh $1 $2 &&
ghdl -a sock_pkg.vhdl &&

### The following lines (till line 23) are added by Ashutosh Jha
### Date - 3/3/2020

gcc -c at85_latest_c.c -o at85_latest_c.o &&  
# Compiles and generates object file of microcontroller C code

ghdl -a ghdl_access.vhdl at85_latest.vhdl &&
# Compiles and generates object files of VHDL code of helper function and the main model respectively

mv at85_latest.o at85_latest1.o &&
ld -r -o at85_latest.o at85_latest_c.o at85_latest1.o &&
rm at85_latest1.o &&
# The object files of main VHDL and microcontroller C code need to be linked.
# The above three commands do that

ghdl -a at85_latest_tb.vhdl  &&
ghdl -e -Wl,ghdlserver.o at85_latest_tb &&
./at85_latest_tb