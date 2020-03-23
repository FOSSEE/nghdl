Ngspice Ghdl Interfacing Documentation 
====

It contains all the documenation for Ngspice and Ghdl related work.

> Note: This project is in beta version and has been tested for rings, counters and PWM controllers.


## How is Ngspice interfaced with GHDL?
Ngspice supports mixed mode simulation. It can simulate both digital and analog components.

Ngspice has something called code-model which defines the behavior of your component and can be used in the netlist. For example you can create a full-adder's code-model in Ngspice and use it in any circuit netlist of Ngspice.

Now the question is if we already have digital model creation in Ngspice, then why this interfacing?

Well, in Ngspice, it is difficult to write your own digital code-models. Though, many people are familiar with GHDL and can easily write the VHDL code.
So the idea of interfacing is just to write VHDL code for a model and use it as a dummy model in Ngspice. Thus, whenever Ngspice looks for that model, it will actually call GHDL to get the results.
GHDL's foreign language interface is used for this inter-process communication.


## Pre-requisites
1. Ubuntu 16.04-18.04 (You can try it on other version and let us know)
2. GHDL (LLVM)-0.36
3. Ngspice-31


## How to install?
This module is made available with eSim (Electronic Circuit Simulation). 
Refer https://esim.fossee.in/ for more information.


## Installed Code Structure
1. Ngspice will be installed in home directory $HOME.
2. Source code for all other file will be present in ~/.esim-nghdl
3. symlink "nghdl" is stored in /usr/local/bin


## Features
1. Support for 512 digital models.
2. Support for digital models upto 64 output ports/pins.
3. Multiple instances of same digital model.


## How to use the Examples provided with NGHDL?
1. Launch eSim --> Click on "NGHDL" icon from the left toolbar --> Click on "Browse" button --> Go to ../nghdl/Example/
2. Locate the example you wish to simulate, find the VHDL file within that example and click on "Open" button at the bottom of "Open File" window.
3. Click on "Upload" button in the NGHDL window. File will be processed in the backend for few seconds. Now exit the NGHDL window.
4. Open the desired example under eSim/Examples/NGHDL_Examples/ using the "Open Project" button, double click on the project when the project is loaded in the "Projects" window.
5. Click on the "Simulation" button on eSim Main window.

