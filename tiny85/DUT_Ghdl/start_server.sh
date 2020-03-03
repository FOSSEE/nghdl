#!/bin/bash

###This server run ghdl testebench for infinite time till ngspice send END signal to stop it

cd /home/fossee/ngspice-nghdl/src/xspice/icm/ghdl/attiny_85_nghdl/DUTghdl/
chmod 775 sock_pkg_create.sh &&
./sock_pkg_create.sh $1 $2 &&
ghdl -a sock_pkg.vhdl &&

gcc -c tiny85_c.c -o tiny85_c.o &&           					#Added by Ashutosh Jha
ghdl -a ghdl_access.vhdl attiny_85_nghdl.vhdl &&    			#Added by Ashutosh Jha
mv attiny_85_nghdl.o attiny_85_nghdl1.o                        	#Added by Ashutosh Jha
ld -r -o attiny_85_nghdl.o tiny85_c.o attiny_85_nghdl1.o &&		#Added by Ashutosh Jha
rm attiny_85_nghdl1.o &&                           				#Added by Ashutosh Jha

ghdl -a attiny_85_nghdl_tb.vhdl  &&
ghdl -e -Wl,ghdlserver.o attiny_85_nghdl_tb &&
./attiny_85_nghdl_tb