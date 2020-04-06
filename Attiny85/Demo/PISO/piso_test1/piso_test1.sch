EESchema Schematic File Version 2
LIBS:piso_test1-rescue
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:eSim_Analog
LIBS:eSim_Devices
LIBS:eSim_Digital
LIBS:eSim_Hybrid
LIBS:eSim_Miscellaneous
LIBS:eSim_Power
LIBS:eSim_Sources
LIBS:eSim_Subckt
LIBS:eSim_User
LIBS:eSim_Plot
LIBS:eSim_PSpice
LIBS:eSim_Nghdl
LIBS:piso_test1-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L dac_bridge_1 U4
U 1 1 5E462D72
P 6950 3000
F 0 "U4" H 6950 3000 60  0000 C CNN
F 1 "dac_bridge_1" H 6950 3150 60  0000 C CNN
F 2 "" H 6950 3000 60  0000 C CNN
F 3 "" H 6950 3000 60  0000 C CNN
	1    6950 3000
	1    0    0    -1  
$EndComp
$Comp
L eSim_R R1
U 1 1 5E463183
P 7700 3350
F 0 "R1" H 7750 3480 50  0000 C CNN
F 1 "1k" H 7750 3300 50  0000 C CNN
F 2 "" H 7750 3330 30  0000 C CNN
F 3 "" V 7750 3400 30  0000 C CNN
	1    7700 3350
	0    1    1    0   
$EndComp
$Comp
L GND #PWR01
U 1 1 5E46321B
P 6250 4300
F 0 "#PWR01" H 6250 4050 50  0001 C CNN
F 1 "GND" H 6250 4150 50  0000 C CNN
F 2 "" H 6250 4300 50  0001 C CNN
F 3 "" H 6250 4300 50  0001 C CNN
	1    6250 4300
	1    0    0    -1  
$EndComp
$Comp
L plot_v1 U5
U 1 1 5E4632C0
P 7750 3150
F 0 "U5" H 7750 3650 60  0000 C CNN
F 1 "plot_v1" H 7950 3500 60  0000 C CNN
F 2 "" H 7750 3150 60  0000 C CNN
F 3 "" H 7750 3150 60  0000 C CNN
	1    7750 3150
	1    0    0    -1  
$EndComp
Text GLabel 7850 2950 2    60   Input ~ 0
Serial_Out
Text GLabel 4000 3500 2    60   Input ~ 0
clock
$Comp
L plot_v1 U3
U 1 1 5E4633F5
P 3950 3550
F 0 "U3" H 3950 4050 60  0000 C CNN
F 1 "plot_v1" H 4150 3900 60  0000 C CNN
F 2 "" H 3950 3550 60  0000 C CNN
F 3 "" H 3950 3550 60  0000 C CNN
	1    3950 3550
	1    0    0    -1  
$EndComp
$Comp
L piso U1
U 1 1 5E6E0ABA
P 3000 4850
F 0 "U1" H 5850 6650 60  0000 C CNN
F 1 "piso" H 5850 6850 60  0000 C CNN
F 2 "" H 5850 6800 60  0000 C CNN
F 3 "" H 5850 6800 60  0000 C CNN
	1    3000 4850
	1    0    0    -1  
$EndComp
$Comp
L pulse v1
U 1 1 5E6E0CBF
P 3750 3800
F 0 "v1" H 3550 3900 60  0000 C CNN
F 1 "pulse" H 3550 3750 60  0000 C CNN
F 2 "R1" H 3450 3800 60  0000 C CNN
F 3 "" H 3750 3800 60  0000 C CNN
	1    3750 3800
	1    0    0    -1  
$EndComp
Text Notes 4650 3150 0    60   ~ 12
INPUTS\n(from C code)
$Comp
L adc_bridge_1 U2
U 1 1 5E6E0E27
P 4800 3400
F 0 "U2" H 4800 3400 60  0000 C CNN
F 1 "adc_bridge_1" H 4800 3550 60  0000 C CNN
F 2 "" H 4800 3400 60  0000 C CNN
F 3 "" H 4800 3400 60  0000 C CNN
	1    4800 3400
	1    0    0    -1  
$EndComp
Wire Wire Line
	7500 2950 7850 2950
Wire Wire Line
	7750 2950 7750 3250
Connection ~ 7750 2950
Wire Wire Line
	3950 3500 4000 3500
Wire Wire Line
	3950 3350 3950 3500
Connection ~ 3950 3350
Wire Wire Line
	3750 3350 4200 3350
Wire Wire Line
	3750 4250 7750 4250
Wire Wire Line
	7750 4250 7750 3550
Wire Wire Line
	6250 4300 6250 4250
Connection ~ 6250 4250
$EndSCHEMATC
