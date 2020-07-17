EESchema Schematic File Version 2
LIBS:attiny85-test-rescue
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
LIBS:attiny85-test-cache
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
L pulse v2
U 1 1 5E55F669
P 3200 3950
F 0 "v2" H 3000 4050 60  0000 C CNN
F 1 "pulse" H 3000 3900 60  0000 C CNN
F 2 "R1" H 2900 3950 60  0000 C CNN
F 3 "" H 3200 3950 60  0000 C CNN
	1    3200 3950
	1    0    0    -1  
$EndComp
$Comp
L R R1
U 1 1 5E55F6CC
P 7800 3600
F 0 "R1" V 7880 3600 50  0000 C CNN
F 1 "1k" V 7800 3600 50  0000 C CNN
F 2 "" V 7730 3600 50  0001 C CNN
F 3 "" H 7800 3600 50  0001 C CNN
	1    7800 3600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR01
U 1 1 5E55F777
P 5950 4700
F 0 "#PWR01" H 5950 4450 50  0001 C CNN
F 1 "GND" H 5950 4550 50  0000 C CNN
F 2 "" H 5950 4700 50  0001 C CNN
F 3 "" H 5950 4700 50  0001 C CNN
	1    5950 4700
	1    0    0    -1  
$EndComp
$Comp
L adc_bridge_1 U3
U 1 1 5E55F805
P 4150 3350
F 0 "U3" H 4150 3350 60  0000 C CNN
F 1 "adc_bridge_1" H 4150 3500 60  0000 C CNN
F 2 "" H 4150 3350 60  0000 C CNN
F 3 "" H 4150 3350 60  0000 C CNN
	1    4150 3350
	1    0    0    -1  
$EndComp
$Comp
L dac_bridge_1 U5
U 1 1 5E55F87B
P 6850 2950
F 0 "U5" H 6850 2950 60  0000 C CNN
F 1 "dac_bridge_1" H 6850 3100 60  0000 C CNN
F 2 "" H 6850 2950 60  0000 C CNN
F 3 "" H 6850 2950 60  0000 C CNN
	1    6850 2950
	1    0    0    -1  
$EndComp
$Comp
L plot_v1 U6
U 1 1 5E55FADB
P 7800 3100
F 0 "U6" H 7800 3600 60  0000 C CNN
F 1 "plot_v1" H 8000 3450 60  0000 C CNN
F 2 "" H 7800 3100 60  0000 C CNN
F 3 "" H 7800 3100 60  0000 C CNN
	1    7800 3100
	1    0    0    -1  
$EndComp
Text GLabel 7950 2900 2    60   Input ~ 0
pb0
$Comp
L adc_bridge_2 U1
U 1 1 5E55FBCB
P 4150 2950
F 0 "U1" H 4150 2950 60  0000 C CNN
F 1 "adc_bridge_2" H 4150 3100 60  0000 C CNN
F 2 "" H 4150 2950 60  0000 C CNN
F 3 "" H 4150 2950 60  0000 C CNN
	1    4150 2950
	1    0    0    -1  
$EndComp
$Comp
L DC-RESCUE-attiny85-test v1
U 1 1 5E55FCA0
P 2450 3700
F 0 "v1" H 2250 3800 60  0000 C CNN
F 1 "DC" H 2250 3650 60  0000 C CNN
F 2 "R1" H 2150 3700 60  0000 C CNN
F 3 "" H 2450 3700 60  0000 C CNN
	1    2450 3700
	1    0    0    -1  
$EndComp
Wire Wire Line
	7800 4550 7800 3750
Wire Wire Line
	2450 4550 7800 4550
Wire Wire Line
	7800 2900 7800 3450
Wire Wire Line
	5950 4700 5950 4550
Connection ~ 5950 4550
Wire Wire Line
	2450 4550 2450 4150
Wire Wire Line
	2800 3000 2800 4550
Connection ~ 2800 4550
Wire Wire Line
	4700 2900 5050 2900
Wire Wire Line
	4700 3000 5050 3000
Wire Wire Line
	6050 2900 6250 2900
$Comp
L attiny_85_nghdl U4
U 1 1 5E562E17
P 2700 4800
F 0 "U4" H 5550 6600 60  0000 C CNN
F 1 "attiny_85_nghdl" H 5550 6800 60  0000 C CNN
F 2 "" H 5550 6750 60  0000 C CNN
F 3 "" H 5550 6750 60  0000 C CNN
	1    2700 4800
	1    0    0    -1  
$EndComp
Wire Wire Line
	7400 2900 7950 2900
Connection ~ 7800 2900
Wire Wire Line
	2450 3250 2450 2900
Wire Wire Line
	2450 2900 3550 2900
Wire Wire Line
	2800 3000 3550 3000
Text Notes 6400 2500 0    60   ~ 0
I/O Ports
Text Notes 3950 2550 0    60   ~ 0
VCC, GND and CLK
Wire Wire Line
	5050 3100 5050 3300
Wire Wire Line
	5050 3300 4700 3300
Wire Wire Line
	3550 3300 3200 3300
Wire Wire Line
	3200 3300 3200 3500
Wire Wire Line
	3200 4400 3200 4550
Connection ~ 3200 4550
Wire Notes Line
	3600 2050 3600 4900
Wire Notes Line
	7500 2050 7500 4900
Text Notes 2900 2250 0    60   ~ 0
Analog
Text Notes 7800 2200 0    60   ~ 0
Analog
Text Notes 5450 2200 0    60   ~ 0
Digital
Wire Notes Line
	5000 2400 5000 3500
Wire Notes Line
	6150 2400 6150 3500
$EndSCHEMATC