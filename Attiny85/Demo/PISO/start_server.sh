#!/bin/bash

###This server run ghdl testebench for infinite time till ngspice send END signal to stop it

cd ~/ngspice-nghdl/src/xspice/icm/ghdl/piso/DUTghdl/
chmod 775 sock_pkg_create.sh &&
./sock_pkg_create.sh $1 $2 &&
ghdl -a sock_pkg.vhdl &&

gcc -c piso_c.c -o piso_c.o &&           #Added by Ashutosh Jha
ghdl -a ghdl_access.vhdl piso.vhdl &&    #Added by Ashutosh Jha
mv piso.o piso1.o                        #Added by Ashutosh Jha
ld -r -o piso.o piso_c.o piso1.o &&      #Added by Ashutosh Jha
rm piso1.o &&                            #Added by Ashutosh Jha

ghdl -a piso_tb.vhdl  &&
ghdl -e -Wl,ghdlserver.o piso_tb &&
./piso_tb