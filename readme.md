Ngspice Ghdl Interfacing Documentation 
====

It contains all the documenation for Ngspice and Ghdl related work.

> Note: This project is in beta version and has been tested for rings, counters and PWM controllers.


## What exactly interfacing of ngspice ghdl do?
Ngspice support mixed mode simulation. It can simulate both digital and analog component. 

Ngspice has something called model which define the functionality of your circuit,which can be used in the netlist. For example you can create adder model in ngspice and use it in any circuit netlist of ngspice.

Now the question is if we already have digital model in ngspice why this interfacing ?
Well in ngspice it is little tediouse to write your digital model. But many people are familiar with ghdl and can easily write the vhdl code.
So the idea of interfacing is just to write ghdl code for a model and install it as dummy model in ngspice. So whenever ngspice look 
for that model it will actually call the ghdl to get the result.


##Pre-requisites
1. Ubuntu 12.04-16.04 (You can try it on other version and let us know)
2. Python 2.7
3. PyQt4


##How to install?
1. Clone this repository.
2. Run `./install-nghdl.sh` It will install ngspice from source code and put it in $HOME.
3. Set ngspice path in `.bashrc` file. Add `export PATH=/home/{your-username}/ngspice-nghdl/install_dir/bin:$PATH` line in .bashrc


##Few words about installed code structure.
1. Ngspice will be installed in home directory $HOME. If you already have ngspice-31 directory there it will take its backup.
2. Source code for all other file will be present in ~/.esim-nghdl
3. symlink nghdl is stored in /usr/local/bin


##FEATURES:
1. Support for 512 digital models.
2. Support for digital models upto 64 output ports/pins.
3. Multiple instances of same digital model.

##How to use?
1. Run nghdl in command terminal.
2. Upload your vhdl file.
3. Model will be created with name of your vhdl file. It can be seen under (~/ngspice-nghdl/src/xspice/icm/ghdl/)
4. You can use this model in your netlist.


##How to use the Examples provided with NGHDL?
1. Go to eSim main window -> Click on NGHDL icon from the left toolbar, click on the 'browse' button, go to ../nghdl/Example/ and locate which example you wish to simulate.
2. After opening the directory of desired example, locate the vhdl file , click on the "Open" button at the bottom of "Open File" window.
3. Click on 'upload' button in the NGHDL pop-up window. File will be processed in the backend for few seconds. Now exit the NGHDL window.
4. Open the desired example under eSim/Examples/NGHDL_Examples/ using the Open Project button, double click on the project when the project is loaded in the "Projects" window.
5. Click on the "Simulation" button on eSim Main window.

