EESchema Schematic File Version 4
EELAYER 30 0
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
L Sensor:BME280 U1
U 1 1 6049A70A
P 1750 2550
F 0 "U1" H 1450 3000 50  0000 R CNN
F 1 "BME280" H 1600 2100 50  0000 R CNN
F 2 "Package_LGA:Bosch_LGA-8_2.5x2.5mm_P0.65mm_ClockwisePinNumbering" H 3250 2100 50  0001 C CNN
F 3 "https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BME280-DS002.pdf" H 1750 2350 50  0001 C CNN
	1    1750 2550
	1    0    0    -1  
$EndComp
$Comp
L Device:C C2
U 1 1 6049ADB6
P 1050 2550
F 0 "C2" V 1000 2350 50  0000 L CNN
F 1 "0.1µF" V 1000 2600 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 1088 2400 50  0001 C CNN
F 3 "~" H 1050 2550 50  0001 C CNN
	1    1050 2550
	1    0    0    -1  
$EndComp
$Comp
L Device:C C1
U 1 1 6049C2EB
P 750 2550
F 0 "C1" V 700 2350 50  0000 L CNN
F 1 "0.1µF" V 700 2600 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 788 2400 50  0001 C CNN
F 3 "~" H 750 2550 50  0001 C CNN
	1    750  2550
	1    0    0    -1  
$EndComp
Wire Wire Line
	1850 1950 1850 1700
Wire Wire Line
	1650 3150 1650 3300
Wire Wire Line
	1650 3300 1850 3300
Wire Wire Line
	1850 3300 1850 3150
Connection ~ 1650 3300
$Comp
L power:GND #PWR01
U 1 1 6049D8AC
P 1050 3550
F 0 "#PWR01" H 1050 3300 50  0001 C CNN
F 1 "GND" H 1055 3377 50  0000 C CNN
F 2 "" H 1050 3550 50  0001 C CNN
F 3 "" H 1050 3550 50  0001 C CNN
	1    1050 3550
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR02
U 1 1 6049DC7E
P 1650 1350
F 0 "#PWR02" H 1650 1200 50  0001 C CNN
F 1 "+3.3V" H 1600 1500 50  0000 C CNN
F 2 "" H 1650 1350 50  0001 C CNN
F 3 "" H 1650 1350 50  0001 C CNN
	1    1650 1350
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR03
U 1 1 6049DEA2
P 1850 1350
F 0 "#PWR03" H 1850 1200 50  0001 C CNN
F 1 "+3.3V" H 1900 1500 50  0000 C CNN
F 2 "" H 1850 1350 50  0001 C CNN
F 3 "" H 1850 1350 50  0001 C CNN
	1    1850 1350
	1    0    0    -1  
$EndComp
$Comp
L Homekit_Faceplate_v1.1-rescue:SolderJumper_3_Bridged12-Jumper JP1
U 1 1 6049F73F
P 2700 1650
F 0 "JP1" V 2600 1450 50  0000 L CNN
F 1 "EN ADR PULLUP" H 2200 1800 50  0000 L CNN
F 2 "Jumper:SolderJumper-3_P1.3mm_Bridged12_Pad1.0x1.5mm" H 2700 1650 50  0001 C CNN
F 3 "~" H 2700 1650 50  0001 C CNN
	1    2700 1650
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR05
U 1 1 604A0D23
P 2700 1950
F 0 "#PWR05" H 2700 1700 50  0001 C CNN
F 1 "GND" H 2705 1777 50  0000 C CNN
F 2 "" H 2700 1950 50  0001 C CNN
F 3 "" H 2700 1950 50  0001 C CNN
	1    2700 1950
	1    0    0    -1  
$EndComp
$Comp
L Device:R R1
U 1 1 604A183A
P 2700 1150
F 0 "R1" V 2650 900 50  0000 L CNN
F 1 "4k7" V 2700 1050 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 2630 1150 50  0001 C CNN
F 3 "~" H 2700 1150 50  0001 C CNN
	1    2700 1150
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR04
U 1 1 604A4B04
P 2700 850
F 0 "#PWR04" H 2700 700 50  0001 C CNN
F 1 "+3.3V" H 2715 1023 50  0000 C CNN
F 2 "" H 2700 850 50  0001 C CNN
F 3 "" H 2700 850 50  0001 C CNN
	1    2700 850 
	1    0    0    -1  
$EndComp
Wire Wire Line
	2550 1650 2500 1650
Wire Wire Line
	2500 1650 2500 2250
Wire Wire Line
	2700 1950 2700 1850
Wire Wire Line
	2700 1450 2700 1300
Wire Wire Line
	2700 1000 2700 850 
$Comp
L Device:R R2
U 1 1 604A7D26
P 3200 1650
F 0 "R2" V 3150 1400 50  0000 L CNN
F 1 "4k7" V 3200 1550 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 3130 1650 50  0001 C CNN
F 3 "~" H 3200 1650 50  0001 C CNN
	1    3200 1650
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR06
U 1 1 604A8F70
P 3200 850
F 0 "#PWR06" H 3200 700 50  0001 C CNN
F 1 "+3.3V" H 3215 1023 50  0000 C CNN
F 2 "" H 3200 850 50  0001 C CNN
F 3 "" H 3200 850 50  0001 C CNN
	1    3200 850 
	1    0    0    -1  
$EndComp
Wire Wire Line
	3200 2850 3200 1800
Wire Wire Line
	3200 2850 2350 2850
$Comp
L Homekit_Faceplate_v1.1-rescue:SolderJumper_2_Bridged-Jumper JP2
U 1 1 604AA918
P 3200 1150
F 0 "JP2" H 3300 1100 50  0000 L CNN
F 1 "EN I2C MODE" H 3200 1300 50  0000 L CNN
F 2 "Jumper:SolderJumper-2_P1.3mm_Bridged2Bar_Pad1.0x1.5mm" H 3200 1150 50  0001 C CNN
F 3 "~" H 3200 1150 50  0001 C CNN
	1    3200 1150
	0    1    1    0   
$EndComp
Wire Wire Line
	3200 1000 3200 850 
Wire Wire Line
	3200 1300 3200 1500
$Comp
L Device:R R3
U 1 1 604AD576
P 3750 1650
F 0 "R3" V 3700 1400 50  0000 L CNN
F 1 "4k7" V 3750 1550 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 3680 1650 50  0001 C CNN
F 3 "~" H 3750 1650 50  0001 C CNN
	1    3750 1650
	1    0    0    -1  
$EndComp
$Comp
L Device:R R4
U 1 1 604ADBDF
P 4250 1650
F 0 "R4" V 4200 1400 50  0000 L CNN
F 1 "4k7" V 4250 1550 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 4180 1650 50  0001 C CNN
F 3 "~" H 4250 1650 50  0001 C CNN
	1    4250 1650
	1    0    0    -1  
$EndComp
Wire Wire Line
	3750 2450 3750 1800
$Comp
L Homekit_Faceplate_v1.1-rescue:SolderJumper_3_Bridged123-Jumper JP3
U 1 1 604AF5B9
P 4000 1150
F 0 "JP3" H 4150 1050 50  0000 C CNN
F 1 "EN I2C PULLUP" V 3600 1400 50  0000 C CNN
F 2 "Jumper:SolderJumper-3_P1.3mm_Bridged123_RoundedPad1.0x1.5mm" H 4000 1150 50  0001 C CNN
F 3 "~" H 4000 1150 50  0001 C CNN
	1    4000 1150
	-1   0    0    1   
$EndComp
$Comp
L power:+3.3V #PWR07
U 1 1 604B2498
P 4000 850
F 0 "#PWR07" H 4000 700 50  0001 C CNN
F 1 "+3.3V" H 4015 1023 50  0000 C CNN
F 2 "" H 4000 850 50  0001 C CNN
F 3 "" H 4000 850 50  0001 C CNN
	1    4000 850 
	1    0    0    -1  
$EndComp
Wire Wire Line
	4000 850  4000 1000
Wire Wire Line
	1650 1350 1650 1850
Wire Wire Line
	1050 2400 1050 1850
Wire Wire Line
	1050 1850 1650 1850
Connection ~ 1650 1850
Wire Wire Line
	1650 1850 1650 1950
Wire Wire Line
	1850 1700 750  1700
Wire Wire Line
	750  1700 750  2400
Connection ~ 1850 1700
Wire Wire Line
	1850 1350 1850 1700
Wire Wire Line
	1050 3550 1050 3300
Wire Wire Line
	1050 3300 1650 3300
Wire Wire Line
	1050 3300 1050 2700
Connection ~ 1050 3300
Wire Wire Line
	1050 3300 750  3300
Wire Wire Line
	750  3300 750  2700
Wire Wire Line
	2350 2450 3750 2450
Wire Wire Line
	3800 1150 3750 1150
Wire Wire Line
	3750 1150 3750 1500
Wire Wire Line
	4200 1150 4250 1150
Wire Wire Line
	4250 1150 4250 1500
Wire Wire Line
	2350 2650 4250 2650
Wire Wire Line
	4250 2650 4250 1800
Wire Wire Line
	2500 2250 2350 2250
Text Label 4950 2450 0    50   ~ 0
SCL
Wire Wire Line
	4950 2450 3750 2450
Connection ~ 3750 2450
Text Label 4950 2650 0    50   ~ 0
SDA
Wire Wire Line
	4950 2650 4250 2650
Connection ~ 4250 2650
Text Notes 1000 900  0    128  ~ 0
BME280 - I2C
$Comp
L Device:R R5
U 1 1 60503125
P 7350 1450
F 0 "R5" V 7300 1250 50  0000 C CNN
F 1 "220R" V 7350 1450 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 7280 1450 50  0001 C CNN
F 3 "~" H 7350 1450 50  0001 C CNN
	1    7350 1450
	0    1    1    0   
$EndComp
$Comp
L Device:R R6
U 1 1 60503C23
P 7350 1650
F 0 "R6" V 7300 1450 50  0000 C CNN
F 1 "220R" V 7350 1650 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 7280 1650 50  0001 C CNN
F 3 "~" H 7350 1650 50  0001 C CNN
	1    7350 1650
	0    1    1    0   
$EndComp
$Comp
L Device:R R7
U 1 1 605045F1
P 7350 1850
F 0 "R7" V 7300 1650 50  0000 C CNN
F 1 "220R" V 7350 1850 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 7280 1850 50  0001 C CNN
F 3 "~" H 7350 1850 50  0001 C CNN
	1    7350 1850
	0    1    1    0   
$EndComp
$Comp
L Device:R R8
U 1 1 60504A9F
P 7350 2050
F 0 "R8" V 7300 1850 50  0000 C CNN
F 1 "220R" V 7350 2050 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 7280 2050 50  0001 C CNN
F 3 "~" H 7350 2050 50  0001 C CNN
	1    7350 2050
	0    1    1    0   
$EndComp
$Comp
L Device:LED D1
U 1 1 6050537B
P 7850 1450
F 0 "D1" H 7950 1400 50  0000 C CNN
F 1 "LED GREEN" H 7650 1500 28  0000 C CNN
F 2 "LED_THT:LED_D3.0mm" H 7850 1450 50  0001 C CNN
F 3 "~" H 7850 1450 50  0001 C CNN
	1    7850 1450
	-1   0    0    1   
$EndComp
Wire Wire Line
	8250 1450 8000 1450
Wire Wire Line
	7700 1450 7500 1450
$Comp
L Device:LED D2
U 1 1 60507D18
P 7850 1650
F 0 "D2" H 7950 1600 50  0000 C CNN
F 1 "LED RED" H 7650 1700 28  0000 C CNN
F 2 "LED_THT:LED_D3.0mm" H 7850 1650 50  0001 C CNN
F 3 "~" H 7850 1650 50  0001 C CNN
	1    7850 1650
	-1   0    0    1   
$EndComp
$Comp
L Device:LED D3
U 1 1 6050B1FD
P 7850 1850
F 0 "D3" H 7950 1800 50  0000 C CNN
F 1 "LED ORANGE" H 7650 1900 28  0000 C CNN
F 2 "LED_THT:LED_D3.0mm" H 7850 1850 50  0001 C CNN
F 3 "~" H 7850 1850 50  0001 C CNN
	1    7850 1850
	-1   0    0    1   
$EndComp
$Comp
L Device:LED D4
U 1 1 6050B823
P 7850 2050
F 0 "D4" H 7950 2000 50  0000 C CNN
F 1 "LED RED" H 7650 2100 28  0000 C CNN
F 2 "LED_THT:LED_D3.0mm" H 7850 2050 50  0001 C CNN
F 3 "~" H 7850 2050 50  0001 C CNN
	1    7850 2050
	-1   0    0    1   
$EndComp
Wire Wire Line
	8000 1650 8250 1650
Wire Wire Line
	8250 1650 8250 1450
Wire Wire Line
	8000 1850 8250 1850
Wire Wire Line
	8250 1850 8250 1650
Connection ~ 8250 1650
Wire Wire Line
	8000 2050 8250 2050
Wire Wire Line
	8250 2050 8250 1850
Connection ~ 8250 1850
Wire Wire Line
	7700 2050 7500 2050
Wire Wire Line
	7700 1850 7500 1850
Wire Wire Line
	7700 1650 7500 1650
Text Label 6850 1450 0    50   ~ 0
3
Wire Wire Line
	6850 1450 7200 1450
Text Label 6850 1650 0    50   ~ 0
6
Wire Wire Line
	6850 1650 7200 1650
Text Label 6850 1850 0    50   ~ 0
9
Wire Wire Line
	6850 1850 7200 1850
Text Label 6850 2050 0    50   ~ 0
11
Wire Wire Line
	6850 2050 7200 2050
Text Notes 6850 1100 0    128  ~ 0
STATUS LEDS
$Comp
L Switch:SW_Push SW1
U 1 1 6051FE00
P 1300 6350
F 0 "SW1" H 1200 6450 50  0000 R CNN
F 1 "SW PROG MODE" H 1600 6250 50  0000 R CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 1300 6550 50  0001 C CNN
F 3 "~" H 1300 6550 50  0001 C CNN
	1    1300 6350
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR09
U 1 1 60521325
P 1300 6800
F 0 "#PWR09" H 1300 6550 50  0001 C CNN
F 1 "GND" H 1305 6627 50  0000 C CNN
F 2 "" H 1300 6800 50  0001 C CNN
F 3 "" H 1300 6800 50  0001 C CNN
	1    1300 6800
	1    0    0    -1  
$EndComp
Wire Wire Line
	1300 6800 1300 6550
$Comp
L Device:C C3
U 1 1 6052257A
P 1650 6350
F 0 "C3" V 1600 6150 50  0000 L CNN
F 1 "0.1µF" V 1800 6250 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 1688 6200 50  0001 C CNN
F 3 "~" H 1650 6350 50  0001 C CNN
	1    1650 6350
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR011
U 1 1 605238EC
P 1650 6800
F 0 "#PWR011" H 1650 6550 50  0001 C CNN
F 1 "GND" H 1655 6627 50  0000 C CNN
F 2 "" H 1650 6800 50  0001 C CNN
F 3 "" H 1650 6800 50  0001 C CNN
	1    1650 6800
	1    0    0    -1  
$EndComp
Wire Wire Line
	1650 6800 1650 6500
$Comp
L Device:R R9
U 1 1 60524F63
P 1650 5600
F 0 "R9" V 1750 5550 50  0000 L CNN
F 1 "1k" V 1650 5550 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 1580 5600 50  0001 C CNN
F 3 "~" H 1650 5600 50  0001 C CNN
	1    1650 5600
	1    0    0    -1  
$EndComp
Wire Wire Line
	1650 6200 1650 5950
Wire Wire Line
	1650 5950 1300 5950
Wire Wire Line
	1300 5950 1300 6150
Connection ~ 1650 5950
Wire Wire Line
	1650 5950 1650 5750
$Comp
L power:+3.3V #PWR010
U 1 1 60527A0E
P 1650 5300
F 0 "#PWR010" H 1650 5150 50  0001 C CNN
F 1 "+3.3V" H 1665 5473 50  0000 C CNN
F 2 "" H 1650 5300 50  0001 C CNN
F 3 "" H 1650 5300 50  0001 C CNN
	1    1650 5300
	1    0    0    -1  
$EndComp
Wire Wire Line
	1650 5300 1650 5450
$Comp
L Switch:SW_Push SW2
U 1 1 6052BE51
P 2250 6350
F 0 "SW2" H 2150 6450 50  0000 R CNN
F 1 "SW RESET" H 2400 6250 50  0000 R CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 2250 6550 50  0001 C CNN
F 3 "~" H 2250 6550 50  0001 C CNN
	1    2250 6350
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR012
U 1 1 6052BE5B
P 2250 6800
F 0 "#PWR012" H 2250 6550 50  0001 C CNN
F 1 "GND" H 2255 6627 50  0000 C CNN
F 2 "" H 2250 6800 50  0001 C CNN
F 3 "" H 2250 6800 50  0001 C CNN
	1    2250 6800
	1    0    0    -1  
$EndComp
Wire Wire Line
	2250 6800 2250 6550
$Comp
L Device:C C4
U 1 1 6052BE66
P 2600 6350
F 0 "C4" V 2550 6150 50  0000 L CNN
F 1 "0.1µF" V 2750 6250 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 2638 6200 50  0001 C CNN
F 3 "~" H 2600 6350 50  0001 C CNN
	1    2600 6350
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR014
U 1 1 6052BE70
P 2600 6800
F 0 "#PWR014" H 2600 6550 50  0001 C CNN
F 1 "GND" H 2605 6627 50  0000 C CNN
F 2 "" H 2600 6800 50  0001 C CNN
F 3 "" H 2600 6800 50  0001 C CNN
	1    2600 6800
	1    0    0    -1  
$EndComp
Wire Wire Line
	2600 6800 2600 6500
$Comp
L Device:R R10
U 1 1 6052BE7B
P 2600 5600
F 0 "R10" V 2700 5550 50  0000 L CNN
F 1 "1k" V 2600 5550 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 2530 5600 50  0001 C CNN
F 3 "~" H 2600 5600 50  0001 C CNN
	1    2600 5600
	1    0    0    -1  
$EndComp
Wire Wire Line
	2600 6200 2600 5950
Wire Wire Line
	2600 5950 2250 5950
Wire Wire Line
	2250 5950 2250 6150
Connection ~ 2600 5950
Wire Wire Line
	2600 5950 2600 5750
$Comp
L power:+3.3V #PWR013
U 1 1 6052BE8A
P 2600 5300
F 0 "#PWR013" H 2600 5150 50  0001 C CNN
F 1 "+3.3V" H 2615 5473 50  0000 C CNN
F 2 "" H 2600 5300 50  0001 C CNN
F 3 "" H 2600 5300 50  0001 C CNN
	1    2600 5300
	1    0    0    -1  
$EndComp
Wire Wire Line
	2600 5300 2600 5450
Text Notes 2600 4950 2    128  ~ 0
PUSH BUTTONS
Text Notes 8300 1450 0    50   ~ 0
LED ON
Text Notes 8300 1650 0    50   ~ 0
LED PROG MODE
Text Notes 8300 1850 0    50   ~ 0
LED HOMEKIT CLIENT CONNECTED
Text Notes 8300 2050 0    50   ~ 0
LED ERROR
$Comp
L Connector_Generic:Conn_01x10 J5
U 1 1 6056F7B9
P 9550 5100
F 0 "J5" H 9550 4500 50  0000 C CNN
F 1 "J Flex Jumper to Teensy" V 9650 5050 50  0000 C CNN
F 2 "Connector_FFC-FPC:Molex_200528-0090_1x09-1MP_P1.00mm_Horizontal" H 9550 5100 50  0001 C CNN
F 3 "~" H 9550 5100 50  0001 C CNN
	1    9550 5100
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR022
U 1 1 60572C91
P 9900 5600
F 0 "#PWR022" H 9900 5350 50  0001 C CNN
F 1 "GND" H 9905 5427 50  0000 C CNN
F 2 "" H 9900 5600 50  0001 C CNN
F 3 "" H 9900 5600 50  0001 C CNN
	1    9900 5600
	1    0    0    -1  
$EndComp
Wire Wire Line
	9750 5500 9900 5500
$Comp
L power:+3.3V #PWR021
U 1 1 60574833
P 9900 4500
F 0 "#PWR021" H 9900 4350 50  0001 C CNN
F 1 "+3.3V" H 9915 4673 50  0000 C CNN
F 2 "" H 9900 4500 50  0001 C CNN
F 3 "" H 9900 4500 50  0001 C CNN
	1    9900 4500
	1    0    0    -1  
$EndComp
Wire Wire Line
	9750 4600 9900 4600
Text Label 9900 4700 0    50   ~ 0
3
Wire Wire Line
	9900 4700 9750 4700
Text Label 9900 4800 0    50   ~ 0
6
Wire Wire Line
	9900 4800 9750 4800
Text Label 9900 4900 0    50   ~ 0
9
Wire Wire Line
	9900 4900 9750 4900
Text Label 9900 5000 0    50   ~ 0
11
Wire Wire Line
	9900 5000 9750 5000
Text Notes 10200 4900 0    50   ~ 0
STATUS LEDS
Text Notes 10200 5400 0    50   ~ 0
PUSH BUTTONS
Text Notes 10200 5200 0    50   ~ 0
I2C
Text Notes 1700 7200 2    50   ~ 0
PROG MODE
Text Notes 2500 7200 2    50   ~ 0
RESET
Text Notes 6950 4200 0    128  ~ 0
CONNECTORS
Wire Notes Line
	10850 4650 9750 4650
Wire Notes Line
	9750 5050 10850 5050
Wire Notes Line
	9750 5250 10850 5250
Wire Notes Line
	9750 5450 10850 5450
$Comp
L SparkFun-Connectors:I2C_STANDARD J4
U 1 1 605A44E9
P 8350 5000
F 0 "J4" H 8250 5400 45  0000 C CNN
F 1 "J I2C" V 8100 5150 45  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 8350 5450 20  0001 C CNN
F 3 "" H 8350 5000 50  0001 C CNN
F 4 "XXX-00000" H 8308 5431 60  0001 C CNN "Field4"
	1    8350 5000
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR019
U 1 1 605A505F
P 8800 4500
F 0 "#PWR019" H 8800 4350 50  0001 C CNN
F 1 "+3.3V" H 8815 4673 50  0000 C CNN
F 2 "" H 8800 4500 50  0001 C CNN
F 3 "" H 8800 4500 50  0001 C CNN
	1    8800 4500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR020
U 1 1 605A597A
P 8800 5100
F 0 "#PWR020" H 8800 4850 50  0001 C CNN
F 1 "GND" H 8805 4927 50  0000 C CNN
F 2 "" H 8800 5100 50  0001 C CNN
F 3 "" H 8800 5100 50  0001 C CNN
	1    8800 5100
	1    0    0    -1  
$EndComp
Wire Wire Line
	8800 5100 8800 5000
Wire Wire Line
	8800 4500 8800 4900
Text Label 8550 4700 0    50   ~ 0
SCL
Wire Wire Line
	8550 4700 8450 4700
Text Label 8550 4800 0    50   ~ 0
SDA
Wire Wire Line
	8550 4800 8450 4800
Wire Wire Line
	8800 4900 8450 4900
Wire Wire Line
	8450 5000 8800 5000
$Comp
L Connector_Generic:Conn_01x04 J3
U 1 1 605C3CFE
P 8200 5950
F 0 "J3" H 8200 5650 50  0000 C CNN
F 1 "J Status LEDs" V 8300 5900 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 8200 5950 50  0001 C CNN
F 3 "~" H 8200 5950 50  0001 C CNN
	1    8200 5950
	-1   0    0    1   
$EndComp
Text Label 8550 5750 0    50   ~ 0
3
Wire Wire Line
	8550 5750 8400 5750
Text Label 8550 5850 0    50   ~ 0
6
Text Label 8550 5950 0    50   ~ 0
9
Text Label 8550 6050 0    50   ~ 0
11
Wire Wire Line
	8550 6050 8400 6050
Wire Wire Line
	8550 5950 8400 5950
Wire Wire Line
	8550 5850 8400 5850
$Comp
L Connector_Generic:Conn_01x02 J1
U 1 1 605CE7C1
P 7150 4900
F 0 "J1" H 7150 4700 50  0000 C CNN
F 1 "J Power" V 7250 4850 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 7150 4900 50  0001 C CNN
F 3 "~" H 7150 4900 50  0001 C CNN
	1    7150 4900
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x04 J2
U 1 1 605D0882
P 7150 5950
F 0 "J2" H 7150 5650 50  0000 C CNN
F 1 "J Push Buttons" V 7250 5900 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 7150 5950 50  0001 C CNN
F 3 "~" H 7150 5950 50  0001 C CNN
	1    7150 5950
	-1   0    0    1   
$EndComp
$Comp
L power:+3.3V #PWR015
U 1 1 605D4CBD
P 7450 4700
F 0 "#PWR015" H 7450 4550 50  0001 C CNN
F 1 "+3.3V" H 7465 4873 50  0000 C CNN
F 2 "" H 7450 4700 50  0001 C CNN
F 3 "" H 7450 4700 50  0001 C CNN
	1    7450 4700
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR016
U 1 1 605D5819
P 7450 5000
F 0 "#PWR016" H 7450 4750 50  0001 C CNN
F 1 "GND" H 7455 4827 50  0000 C CNN
F 2 "" H 7450 5000 50  0001 C CNN
F 3 "" H 7450 5000 50  0001 C CNN
	1    7450 5000
	1    0    0    -1  
$EndComp
Wire Wire Line
	7450 5000 7450 4900
Wire Wire Line
	7450 4900 7350 4900
Wire Wire Line
	7350 4800 7450 4800
Wire Wire Line
	7450 4800 7450 4700
$Comp
L power:GND #PWR018
U 1 1 605DCD41
P 7450 6150
F 0 "#PWR018" H 7450 5900 50  0001 C CNN
F 1 "GND" H 7455 5977 50  0000 C CNN
F 2 "" H 7450 6150 50  0001 C CNN
F 3 "" H 7450 6150 50  0001 C CNN
	1    7450 6150
	1    0    0    -1  
$EndComp
Wire Wire Line
	7450 6150 7450 6050
Wire Wire Line
	7450 6050 7350 6050
$Comp
L power:+3.3V #PWR017
U 1 1 605DFE13
P 7450 5650
F 0 "#PWR017" H 7450 5500 50  0001 C CNN
F 1 "+3.3V" H 7465 5823 50  0000 C CNN
F 2 "" H 7450 5650 50  0001 C CNN
F 3 "" H 7450 5650 50  0001 C CNN
	1    7450 5650
	1    0    0    -1  
$EndComp
Wire Wire Line
	7450 5650 7450 5750
Wire Wire Line
	7450 5750 7350 5750
Text Label 9900 5100 0    50   ~ 0
SCL
Text Label 9900 5200 0    50   ~ 0
SDA
Wire Wire Line
	9900 5400 9750 5400
Wire Wire Line
	9900 4600 9900 4500
Wire Wire Line
	9900 5600 9900 5500
$Comp
L power:GND #PWR0101
U 1 1 604FF989
P 8250 2200
F 0 "#PWR0101" H 8250 1950 50  0001 C CNN
F 1 "GND" H 8255 2027 50  0000 C CNN
F 2 "" H 8250 2200 50  0001 C CNN
F 3 "" H 8250 2200 50  0001 C CNN
	1    8250 2200
	1    0    0    -1  
$EndComp
Wire Wire Line
	8250 2200 8250 2050
Connection ~ 8250 2050
Text Label 7600 5850 0    50   ~ 0
PB1
Text Label 7600 5950 0    50   ~ 0
PB2
Wire Wire Line
	7600 5850 7350 5850
Wire Wire Line
	7600 5950 7350 5950
Text Label 1800 5950 0    50   ~ 0
PB1
Wire Wire Line
	1800 5950 1650 5950
Text Label 2750 5950 0    50   ~ 0
PB2
Wire Wire Line
	2750 5950 2600 5950
Wire Wire Line
	9900 5300 9750 5300
Wire Wire Line
	9900 5100 9750 5100
Wire Wire Line
	9900 5200 9750 5200
Text Label 9900 5300 0    50   ~ 0
PB2
Text Label 9900 5400 0    50   ~ 0
PB1
$EndSCHEMATC
