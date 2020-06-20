This folder contains code models of 10 BIT ADC & DAC.
Accuracy ~ 99.8%

To use - 
1. Make a new folder in /home/ash98/ngspice-nghdl/src/xspice/icm/analog or /home/ash98/ngspice-nghdl/src/xspice/icm/xtradev
2. Copy the cfunc.mod and ifspec.ifs files in the folder and add it in modpath.
3. Open terminal, go to /home/ash98/ngspice-nghdl and run - $make && make install
4. Paste the corresponding xml file in ~/eSim-2.0/library/modelParamXML (for eSim V2)
5. You can make the component in component editor in eeschema yourself or copy the already made part from the correspoding eSim_Analog.lib part.