EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 3
Title "Homekit NCN5120 "
Date "2021-03-14"
Rev "0.1"
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Homekit_Library:NCN5120 U$?
U 1 1 605C8095
P 4900 2950
AR Path="/605C8095" Ref="U$?"  Part="1" 
AR Path="/605817AD/605C8095" Ref="U$1"  Part="1" 
F 0 "U$1" H 4000 3650 50  0000 L CNN
F 1 "NCN5120" H 4000 3750 50  0000 L CNN
F 2 "Package_DFN_QFN:QFN-40-1EP_6x6mm_P0.5mm_EP4.6x4.6mm" H 3050 3950 50  0001 C CNN
F 3 "" H 4450 3600 50  0001 C CNN
	1    4900 2950
	0    -1   -1   0   
$EndComp
$Comp
L SparkFun-Jumpers:JUMPER-SMT_2_NO JP?
U 1 1 605C809C
P 3400 3200
AR Path="/605C809C" Ref="JP?"  Part="1" 
AR Path="/605817AD/605C809C" Ref="JP1"  Part="1" 
F 0 "JP1" H 3300 3250 45  0000 R CNN
F 1 "SJ HIGH POWER" H 3650 3350 45  0000 R CNN
F 2 "Jumper:SolderJumper-2_P1.3mm_Open_RoundedPad1.0x1.5mm" H 3400 3400 20  0001 C CNN
F 3 "" H 3400 3200 60  0001 C CNN
F 4 "XXX-00000" V 3316 3096 60  0001 R CNN "PROD_ID"
	1    3400 3200
	-1   0    0    1   
$EndComp
Wire Wire Line
	4050 3200 3550 3200
Text Label 3850 3000 2    50   ~ 0
SAVE
Wire Wire Line
	3850 3000 4050 3000
Wire Wire Line
	4050 2700 3850 2700
Wire Wire Line
	3850 2700 3850 2500
Wire Wire Line
	3850 2000 4050 2000
Wire Wire Line
	4050 2500 3850 2500
Connection ~ 3850 2500
Wire Wire Line
	3850 2500 3850 2000
$Comp
L Device:C C?
U 1 1 605C80B1
P 3100 2900
AR Path="/605C80B1" Ref="C?"  Part="1" 
AR Path="/605817AD/605C80B1" Ref="C9"  Part="1" 
F 0 "C9" V 3050 2750 50  0000 L CNN
F 1 "9pF / 6.3V" V 2950 2700 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 3138 2750 50  0001 C CNN
F 3 "~" H 3100 2900 50  0001 C CNN
	1    3100 2900
	0    -1   -1   0   
$EndComp
$Comp
L Device:C C?
U 1 1 605C80B7
P 3100 2200
AR Path="/605C80B7" Ref="C?"  Part="1" 
AR Path="/605817AD/605C80B7" Ref="C10"  Part="1" 
F 0 "C10" V 3150 2000 50  0000 L CNN
F 1 "9pF / 6.3V" V 3250 2000 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 3138 2050 50  0001 C CNN
F 3 "~" H 3100 2200 50  0001 C CNN
	1    3100 2200
	0    -1   -1   0   
$EndComp
$Comp
L Device:Crystal_GND24 Y?
U 1 1 605C80BD
P 3350 2500
AR Path="/605C80BD" Ref="Y?"  Part="1" 
AR Path="/605817AD/605C80BD" Ref="Y1"  Part="1" 
F 0 "Y1" V 3400 2250 50  0000 L CNN
F 1 "16MHz" V 3500 2200 50  0000 L CNN
F 2 "Crystal:Crystal_SMD_2520-4Pin_2.5x2.0mm" H 3350 2500 50  0001 C CNN
F 3 "~" H 3350 2500 50  0001 C CNN
	1    3350 2500
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3550 2500 3650 2500
Wire Wire Line
	3650 2500 3650 2750
Wire Wire Line
	3650 2750 3050 2750
Wire Wire Line
	3050 2750 3050 2500
Wire Wire Line
	3050 2500 3150 2500
Wire Wire Line
	3350 2650 3350 2900
Connection ~ 3350 2900
Wire Wire Line
	3350 2900 4050 2900
Wire Wire Line
	4050 2800 3750 2800
Wire Wire Line
	3750 2800 3750 2200
Wire Wire Line
	3750 2200 3350 2200
Wire Wire Line
	3350 2350 3350 2200
Wire Wire Line
	3250 2900 3350 2900
Wire Wire Line
	3250 2200 3350 2200
Connection ~ 3350 2200
Wire Wire Line
	2950 2900 2850 2900
Wire Wire Line
	2850 2900 2850 2500
Wire Wire Line
	2850 2200 2950 2200
Wire Wire Line
	3050 2500 2850 2500
Connection ~ 3050 2500
Connection ~ 2850 2500
Wire Wire Line
	2850 2500 2850 2200
Wire Wire Line
	2650 2000 2650 2500
Wire Wire Line
	2650 2500 2850 2500
Wire Wire Line
	3250 3200 2650 3200
Wire Wire Line
	2650 3200 2650 2500
Connection ~ 2650 2500
Wire Wire Line
	2650 2000 3850 2000
Connection ~ 3850 2000
Wire Wire Line
	4550 1500 4550 2100
Wire Wire Line
	4850 2100 4850 1500
Connection ~ 4850 1500
Wire Wire Line
	4850 1500 4550 1500
Wire Wire Line
	4950 2100 4950 1500
Connection ~ 4950 1500
Wire Wire Line
	4950 1500 4850 1500
Wire Wire Line
	5050 2100 5050 1500
Connection ~ 5050 1500
Wire Wire Line
	5050 1500 4950 1500
Wire Wire Line
	5150 2100 5150 1500
Connection ~ 5150 1500
Wire Wire Line
	5150 1500 5050 1500
$Comp
L Device:C C?
U 1 1 605C80F7
P 4100 1500
AR Path="/605C80F7" Ref="C?"  Part="1" 
AR Path="/605817AD/605C80F7" Ref="C13"  Part="1" 
F 0 "C13" H 4215 1546 50  0000 L CNN
F 1 "100nF / 6.3V" H 4215 1455 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 4138 1350 50  0001 C CNN
F 3 "~" H 4100 1500 50  0001 C CNN
	1    4100 1500
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4450 2100 4450 1750
Wire Wire Line
	4450 1750 3850 1750
Wire Wire Line
	3850 1750 3850 1500
Wire Wire Line
	3850 1500 3950 1500
Wire Wire Line
	4250 1500 4550 1500
Connection ~ 4550 1500
Wire Wire Line
	3650 1500 3850 1500
Connection ~ 3850 1500
$Comp
L Device:C C?
U 1 1 605C8111
P 3700 4200
AR Path="/605C8111" Ref="C?"  Part="1" 
AR Path="/605817AD/605C8111" Ref="C7"  Part="1" 
F 0 "C7" H 3815 4246 50  0000 L CNN
F 1 "100nF / 6.3V" H 3815 4155 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 3738 4050 50  0001 C CNN
F 3 "~" H 3700 4200 50  0001 C CNN
	1    3700 4200
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3850 4200 3950 4200
Wire Wire Line
	4450 3800 4450 3950
Wire Wire Line
	4450 3950 3950 3950
Wire Wire Line
	3950 3950 3950 4200
Connection ~ 3950 4200
Wire Wire Line
	3950 4200 4050 4200
Wire Wire Line
	4550 3800 4550 3950
Wire Wire Line
	4550 3950 4450 3950
Connection ~ 4450 3950
Wire Wire Line
	3250 4200 3450 4200
Wire Wire Line
	3450 4200 3450 3400
Wire Wire Line
	3450 3400 4050 3400
Connection ~ 3450 4200
Wire Wire Line
	3450 4200 3550 4200
Wire Wire Line
	5300 1500 5250 1500
Wire Wire Line
	6000 3100 5750 3100
$Comp
L Device:R R?
U 1 1 605C813A
P 6400 3200
AR Path="/605C813A" Ref="R?"  Part="1" 
AR Path="/605817AD/605C813A" Ref="R4"  Part="1" 
F 0 "R4" V 6450 3000 50  0000 L CNN
F 1 "1R" V 6400 3100 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 6330 3200 50  0001 C CNN
F 3 "~" H 6400 3200 50  0001 C CNN
	1    6400 3200
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R?
U 1 1 605C8140
P 6400 2600
AR Path="/605C8140" Ref="R?"  Part="1" 
AR Path="/605817AD/605C8140" Ref="R5"  Part="1" 
F 0 "R5" V 6450 2400 50  0000 L CNN
F 1 "1R" V 6400 2500 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 6330 2600 50  0001 C CNN
F 3 "~" H 6400 2600 50  0001 C CNN
	1    6400 2600
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6250 3200 5750 3200
$Comp
L Device:L_Small L?
U 1 1 605C8147
P 6400 3000
AR Path="/605C8147" Ref="L?"  Part="1" 
AR Path="/605817AD/605C8147" Ref="L1"  Part="1" 
F 0 "L1" V 6450 2850 50  0000 L CNN
F 1 "220µH" V 6448 2955 50  0000 L CNN
F 2 "Inductor_SMD:L_7.3x7.3_H4.5" H 6400 3000 50  0001 C CNN
F 3 "~" H 6400 3000 50  0001 C CNN
	1    6400 3000
	0    -1   -1   0   
$EndComp
$Comp
L Device:L_Small L?
U 1 1 605C814D
P 6400 2800
AR Path="/605C814D" Ref="L?"  Part="1" 
AR Path="/605817AD/605C814D" Ref="L2"  Part="1" 
F 0 "L2" V 6450 2650 50  0000 L CNN
F 1 "220µH" V 6448 2755 50  0000 L CNN
F 2 "Inductor_SMD:L_7.3x7.3_H4.5" H 6400 2800 50  0001 C CNN
F 3 "~" H 6400 2800 50  0001 C CNN
	1    6400 2800
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6300 3000 5750 3000
Wire Wire Line
	6300 2800 5750 2800
Wire Wire Line
	6650 3200 6650 3300
Wire Wire Line
	6650 3300 6100 3300
Wire Wire Line
	6650 3200 6550 3200
Wire Wire Line
	6650 3200 6650 3000
Wire Wire Line
	6650 3000 6500 3000
Connection ~ 6650 3200
Wire Wire Line
	6500 2800 6650 2800
Wire Wire Line
	6650 2800 6650 2600
Wire Wire Line
	6650 2600 6550 2600
Wire Wire Line
	6650 2600 6650 2500
Wire Wire Line
	6650 2500 5750 2500
Connection ~ 6650 2600
Wire Wire Line
	5350 3800 5350 3900
Wire Wire Line
	5850 2900 5750 2900
Wire Wire Line
	5350 3900 5850 3900
Wire Wire Line
	5850 3900 5850 2900
Wire Wire Line
	5250 3800 5250 3900
Wire Wire Line
	5250 3900 5350 3900
Connection ~ 5350 3900
$Comp
L Device:C C?
U 1 1 605C8168
P 6400 2000
AR Path="/605C8168" Ref="C?"  Part="1" 
AR Path="/605817AD/605C8168" Ref="C11"  Part="1" 
F 0 "C11" V 6350 1800 50  0000 L CNN
F 1 "10µF / 6.3V" V 6250 1750 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 6438 1850 50  0001 C CNN
F 3 "~" H 6400 2000 50  0001 C CNN
	1    6400 2000
	0    -1   -1   0   
$EndComp
$Comp
L Device:C C?
U 1 1 605C816E
P 6400 1550
AR Path="/605C816E" Ref="C?"  Part="1" 
AR Path="/605817AD/605C816E" Ref="C12"  Part="1" 
F 0 "C12" V 6450 1350 50  0000 L CNN
F 1 "10µF / 6.3V" V 6550 1300 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 6438 1400 50  0001 C CNN
F 3 "~" H 6400 1550 50  0001 C CNN
	1    6400 1550
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5750 2600 6100 2600
Wire Wire Line
	6250 2000 6250 1750
Wire Wire Line
	6550 2000 6550 1750
Connection ~ 6550 1750
Wire Wire Line
	6550 1750 6550 1550
Wire Wire Line
	6100 1750 6100 2300
Connection ~ 6100 2600
Wire Wire Line
	6100 2600 6250 2600
Wire Wire Line
	5950 1750 6100 1750
Connection ~ 6250 1750
Wire Wire Line
	6250 1750 6250 1550
Connection ~ 6100 1750
Wire Wire Line
	6100 1750 6250 1750
$Comp
L SparkFun-Jumpers:JUMPER-SMT_2_NO JP?
U 1 1 605C818F
P 6900 2900
AR Path="/605C818F" Ref="JP?"  Part="1" 
AR Path="/605817AD/605C818F" Ref="JP2"  Part="1" 
F 0 "JP2" H 6800 2850 45  0000 R CNN
F 1 "SJ 3V3 ONLY" H 7150 2750 45  0000 R CNN
F 2 "Jumper:SolderJumper-2_P1.3mm_Open_RoundedPad1.0x1.5mm" H 6900 3100 20  0001 C CNN
F 3 "" H 6900 2900 60  0001 C CNN
F 4 "XXX-00000" V 6816 2796 60  0001 R CNN "PROD_ID"
	1    6900 2900
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6100 2300 6900 2300
Wire Wire Line
	6900 2300 6900 2750
Connection ~ 6100 2300
Wire Wire Line
	6100 2300 6100 2600
Wire Wire Line
	6900 3050 6900 3400
Wire Wire Line
	6900 3400 6650 3400
$Comp
L Device:C C?
U 1 1 605C819B
P 6400 4100
AR Path="/605C819B" Ref="C?"  Part="1" 
AR Path="/605817AD/605C819B" Ref="C8"  Part="1" 
F 0 "C8" V 6450 3950 50  0000 L CNN
F 1 "10µF / 6.3V" V 6550 3850 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 6438 3950 50  0001 C CNN
F 3 "~" H 6400 4100 50  0001 C CNN
	1    6400 4100
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6250 4550 6250 4300
Wire Wire Line
	6550 4550 6550 4300
Wire Wire Line
	6650 4300 6600 4300
Connection ~ 6550 4300
Wire Wire Line
	6550 4300 6550 4100
Wire Wire Line
	5950 4300 6100 4300
Connection ~ 6250 4300
Wire Wire Line
	6250 4300 6250 4100
Connection ~ 6100 4300
Wire Wire Line
	6100 4300 6250 4300
Wire Wire Line
	6100 3300 6100 3600
Connection ~ 6100 3300
Wire Wire Line
	6100 3300 5750 3300
$Comp
L Device:R R?
U 1 1 605C81B4
P 6400 3600
AR Path="/605C81B4" Ref="R?"  Part="1" 
AR Path="/605817AD/605C81B4" Ref="R2"  Part="1" 
F 0 "R2" V 6500 3550 50  0000 L CNN
F 1 "33k" V 6400 3500 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 6330 3600 50  0001 C CNN
F 3 "~" H 6400 3600 50  0001 C CNN
	1    6400 3600
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R?
U 1 1 605C81BA
P 6900 3600
AR Path="/605C81BA" Ref="R?"  Part="1" 
AR Path="/605817AD/605C81BA" Ref="R3"  Part="1" 
F 0 "R3" V 7000 3550 50  0000 L CNN
F 1 "180k" V 6900 3500 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 6830 3600 50  0001 C CNN
F 3 "~" H 6900 3600 50  0001 C CNN
	1    6900 3600
	0    -1   -1   0   
$EndComp
Wire Wire Line
	7150 3600 7100 3600
Wire Wire Line
	6750 3600 6650 3600
Wire Wire Line
	6250 3600 6100 3600
Connection ~ 6100 3600
Wire Wire Line
	6100 3600 6100 4300
Wire Wire Line
	6650 3600 6650 3400
Connection ~ 6650 3600
Wire Wire Line
	6650 3600 6550 3600
Connection ~ 6650 3400
Wire Wire Line
	6650 3400 5750 3400
$Comp
L Device:C C?
U 1 1 605C81D6
P 5100 4300
AR Path="/605C81D6" Ref="C?"  Part="1" 
AR Path="/605817AD/605C81D6" Ref="C6"  Part="1" 
F 0 "C6" H 4950 4400 50  0000 L CNN
F 1 "47nF / 25V" H 4550 4300 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 5138 4150 50  0001 C CNN
F 3 "~" H 5100 4300 50  0001 C CNN
	1    5100 4300
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5250 4050 5150 4050
Wire Wire Line
	5150 4050 5150 3800
Wire Wire Line
	5250 4300 5250 4050
Wire Wire Line
	4950 4050 5050 4050
Wire Wire Line
	5050 4050 5050 3800
Wire Wire Line
	4950 4300 4950 4050
$Comp
L Device:C C?
U 1 1 605C81E2
P 5500 4600
AR Path="/605C81E2" Ref="C?"  Part="1" 
AR Path="/605817AD/605C81E2" Ref="C3"  Part="1" 
F 0 "C3" V 5550 4450 50  0000 L CNN
F 1 "1µF / 50V" V 5650 4350 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 5538 4450 50  0001 C CNN
F 3 "~" H 5500 4600 50  0001 C CNN
	1    5500 4600
	0    -1   -1   0   
$EndComp
$Comp
L Device:C C?
U 1 1 605C81E8
P 5500 5300
AR Path="/605C81E8" Ref="C?"  Part="1" 
AR Path="/605817AD/605C81E8" Ref="C1"  Part="1" 
F 0 "C1" V 5450 5150 50  0000 L CNN
F 1 "100nF / 50V" V 5350 5050 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 5538 5150 50  0001 C CNN
F 3 "~" H 5500 5300 50  0001 C CNN
	1    5500 5300
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5350 5300 4850 5300
Wire Wire Line
	4850 5300 4850 3800
$Comp
L Device:CP C?
U 1 1 605C81F0
P 5500 4900
AR Path="/605C81F0" Ref="C?"  Part="1" 
AR Path="/605817AD/605C81F0" Ref="C2"  Part="1" 
F 0 "C2" V 5550 4750 50  0000 L CNN
F 1 "100µF / 35V" V 5650 4550 50  0000 L CNN
F 2 "Capacitor_SMD:C_Elec_6.3x7.7" H 5538 4750 50  0001 C CNN
F 3 "~" H 5500 4900 50  0001 C CNN
	1    5500 4900
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5250 3900 5250 4000
Wire Wire Line
	5250 4000 5300 4000
Wire Wire Line
	5300 4000 5300 4600
Wire Wire Line
	5300 4900 5350 4900
Connection ~ 5250 3900
Wire Wire Line
	5350 4600 5300 4600
Connection ~ 5300 4600
Wire Wire Line
	5300 4600 5300 4900
Wire Wire Line
	5300 5000 5300 4900
Connection ~ 5300 4900
$Comp
L Device:C C?
U 1 1 605C8201
P 4750 4300
AR Path="/605C8201" Ref="C?"  Part="1" 
AR Path="/605817AD/605C8201" Ref="C5"  Part="1" 
F 0 "C5" V 4700 4200 50  0000 C CNN
F 1 "47nF / 50V" V 4800 4000 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 4788 4150 50  0001 C CNN
F 3 "~" H 4750 4300 50  0001 C CNN
	1    4750 4300
	1    0    0    -1  
$EndComp
Wire Wire Line
	4750 4150 4750 3800
$Comp
L Device:R R?
U 1 1 605C8208
P 4650 4750
AR Path="/605C8208" Ref="R?"  Part="1" 
AR Path="/605817AD/605C8208" Ref="R1"  Part="1" 
F 0 "R1" V 4550 4750 50  0000 C CNN
F 1 "22R" V 4650 4750 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 4580 4750 50  0001 C CNN
F 3 "~" H 4650 4750 50  0001 C CNN
	1    4650 4750
	1    0    0    -1  
$EndComp
Wire Wire Line
	4650 4600 4650 3800
Wire Wire Line
	4650 4900 4650 5000
Wire Wire Line
	4650 5000 4750 5000
Wire Wire Line
	4950 5000 4950 4350
Wire Wire Line
	4950 4350 4900 4350
Wire Wire Line
	4900 4350 4900 4000
Wire Wire Line
	4900 4000 4950 4000
Wire Wire Line
	4950 4000 4950 3800
Wire Wire Line
	4750 4450 4750 5000
Connection ~ 4750 5000
Wire Wire Line
	4750 5000 4950 5000
Wire Wire Line
	5650 5300 5750 5300
Wire Wire Line
	5750 5300 5750 4900
Wire Wire Line
	5750 4900 5650 4900
Wire Wire Line
	5750 4900 5750 4600
Wire Wire Line
	5750 4600 5650 4600
Connection ~ 5750 4900
$Comp
L Device:D D?
U 1 1 605C8226
P 4650 5300
AR Path="/605C8226" Ref="D?"  Part="1" 
AR Path="/605817AD/605C8226" Ref="D2"  Part="1" 
F 0 "D2" H 4650 5083 50  0000 C CNN
F 1 "D1" H 4650 5174 50  0000 C CNN
F 2 "Diode_SMD:D_SMA_Handsoldering" H 4650 5300 50  0001 C CNN
F 3 "~" H 4650 5300 50  0001 C CNN
	1    4650 5300
	0    1    1    0   
$EndComp
Wire Wire Line
	4650 5150 4650 5000
Connection ~ 4650 5000
$Comp
L Device:D_TVS D?
U 1 1 605C822E
P 5150 5600
AR Path="/605C822E" Ref="D?"  Part="1" 
AR Path="/605817AD/605C822E" Ref="D1"  Part="1" 
F 0 "D1" H 5100 5700 50  0000 L CNN
F 1 "TVS SMA40" H 4950 5500 50  0000 L CNN
F 2 "Diode_SMD:D_SMA_Handsoldering" H 5150 5600 50  0001 C CNN
F 3 "~" H 5150 5600 50  0001 C CNN
	1    5150 5600
	1    0    0    -1  
$EndComp
Wire Wire Line
	5000 5600 4650 5600
Wire Wire Line
	4650 5600 4650 5450
Wire Wire Line
	5750 5300 5750 5600
Wire Wire Line
	5750 5600 5300 5600
Connection ~ 5750 5300
Connection ~ 4650 5600
NoConn ~ 4050 3300
NoConn ~ 4050 3100
NoConn ~ 4050 2600
NoConn ~ 5250 2100
NoConn ~ 5350 2100
Text Notes 2300 7874 2    74   ~ 0
NCN5120
$Comp
L Device:C C?
U 1 1 605C8243
P 6400 4550
AR Path="/605C8243" Ref="C?"  Part="1" 
AR Path="/605817AD/605C8243" Ref="C4"  Part="1" 
F 0 "C4" V 6350 4400 50  0000 L CNN
F 1 "10µF / 6.3V" V 6250 4300 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 6438 4400 50  0001 C CNN
F 3 "~" H 6400 4550 50  0001 C CNN
	1    6400 4550
	0    -1   -1   0   
$EndComp
Text Notes 5700 4380 2    30   Italic 0
1µF MLCC 1206
Text Notes 6650 2900 2    50   Italic 0
220µH inductors
Text Notes 4500 4950 1    30   Italic 0
22R resistor 2512
Text Notes 4500 5750 1    30   Italic 0
SS16 \nreverse polarity \nprotection diode
Text Notes 5400 5780 2    30   Italic 0
SMAJ40CA TVS diode
Text Notes 2250 6380 2    30   Italic 0
Values on the PCB may differ:\nC6: 4.7nF / 50V\nC9 + 10: 10pF / 50V\nC4 + 11: 10µF / 10V\nC5 + 12: 10µF / 50V\nC1 + 8 + 13: 100nF / 50V\nJP1: 0805 0R\n\nDeactivate / Change VCC2:\nIf you want to get rid of VCC2, L2, R3, R4, R5, C11 and C111 \nare not needed. \n\nClose J2 in that case.\n\nTo change VCC2, change R4 and R5 according the \nNCN5120 dataheet. When VCC2 is greater than 10V, \nC11 must be changed so that the voltage rating matches\nor exceeds VCC2 (e.g. when VCC2 = 12V, use a 16V MLCC)\n\nEnable High Bus current:\n\nFor bus current consumption up to 20mA, J3 must be closed \n(This is the default, closed by a 0R resistor).\n\nFor bus current consumption of 10mA or less, J3 can be open.\n\nGALVANISCHE TRENNUNG einbauen ???
Text Notes 4850 9000 1    74   ~ 0
KNX Klemme
$Comp
L Homekit_Baseplate:Wago_243-211 J?
U 1 1 605C8258
P 4200 8500
AR Path="/605C8258" Ref="J?"  Part="1" 
AR Path="/605817AD/605C8258" Ref="J10"  Part="1" 
F 0 "J10" H 4250 8150 50  0000 L CNN
F 1 "Wago_243-211" H 3950 8050 50  0000 L CNN
F 2 "Homekit_Baseplate:WAGO 243-211 KNX" H 4200 8500 50  0001 C CNN
F 3 "" H 4200 8500 50  0001 C CNN
	1    4200 8500
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4450 8650 4450 8500
Text HLabel 4650 2000 1    50   Input ~ 0
TX_1_KNX
Wire Wire Line
	4650 2000 4650 2100
Text HLabel 4750 2000 1    50   Input ~ 0
RX_1_KNX
Wire Wire Line
	4750 2000 4750 2100
Text HLabel 4150 4200 2    50   Input ~ 0
GND_KNX
Text HLabel 5300 1500 2    50   Input ~ 0
GND_KNX
Text HLabel 4050 2000 2    50   Input ~ 0
GND_KNX
Text HLabel 3950 8050 2    50   Input ~ 0
GND_KNX
Text HLabel 7150 3600 2    50   Input ~ 0
GND_KNX
Text HLabel 6650 4300 2    50   Input ~ 0
GND_KNX
Text HLabel 5900 4900 2    50   Input ~ 0
GND_KNX
Text HLabel 3250 4200 0    50   Input ~ 0
3V3_KNX
Text HLabel 3600 1500 0    50   Input ~ 0
3V3_KNX
Text HLabel 4350 8650 3    50   Input ~ 0
KNX+
$Comp
L SparkFun-Jumpers:JUMPER-SMT_3_1-NC JP8
U 1 1 60DD2D18
P 4350 6100
F 0 "JP8" V 4400 5800 45  0000 L CNN
F 1 "JUMPER-SMT_3_1-NC" V 4500 5850 45  0000 L CNN
F 2 "Jumper:SolderJumper-3_P1.3mm_Bridged12_RoundedPad1.0x1.5mm" H 4350 6350 20  0001 C CNN
F 3 "" V 4350 6045 60  0001 C CNN
F 4 "XXX-00000" H 4454 6016 60  0001 L CNN "PROD_ID"
	1    4350 6100
	0    -1   -1   0   
$EndComp
Text HLabel 4450 8650 3    50   Input ~ 0
KNX-
$Comp
L MicroBCU_Symbol:Konnekting_µBCU GN1
U 1 1 60EDC6FB
P 3500 5700
F 0 "GN1" H 3222 5654 50  0000 R CNN
F 1 "Konnekting_µBCU" H 3222 5745 50  0000 R CNN
F 2 "MicroBCU2:MicroBCU2_Solderd_Vertical" H 3400 5250 50  0001 C CNN
F 3 "" H 3400 5250 50  0001 C CNN
	1    3500 5700
	0    1    1    0   
$EndComp
NoConn ~ 3600 6050
Text HLabel 3200 6550 3    50   Input ~ 0
VCC2_KNX_2
Text HLabel 3300 6550 3    50   Input ~ 0
3V3_KNX_2
Text HLabel 3400 6550 3    50   Input ~ 0
TX_2_KNX_2
Text HLabel 3500 6550 3    50   Input ~ 0
RX_2_KNX_2
$Comp
L SparkFun-Jumpers:JUMPER-SMT_3_1-NC JP5
U 1 1 60DFD915
P 3950 7650
F 0 "JP5" V 3900 7400 45  0000 C CNN
F 1 "JUMPER-SMT_3_1-NC" V 4100 7400 45  0000 C CNN
F 2 "Jumper:SolderJumper-3_P1.3mm_Bridged12_RoundedPad1.0x1.5mm" H 3950 7900 20  0001 C CNN
F 3 "" V 3950 7595 60  0001 C CNN
F 4 "XXX-00000" V 4275 7650 60  0001 C CNN "PROD_ID"
	1    3950 7650
	-1   0    0    -1  
$EndComp
Text Notes 5250 7900 1    74   ~ 0
KNX Klemme 2
$Comp
L Homekit_Baseplate:Wago_243-211 J?
U 1 1 60F31D4C
P 4600 7400
AR Path="/60F31D4C" Ref="J?"  Part="1" 
AR Path="/605817AD/60F31D4C" Ref="J11"  Part="1" 
F 0 "J11" H 4650 7050 50  0000 L CNN
F 1 "Wago_243-211" H 4350 6950 50  0000 L CNN
F 2 "Homekit_Baseplate:WAGO 243-211 KNX" H 4600 7400 50  0001 C CNN
F 3 "" H 4600 7400 50  0001 C CNN
	1    4600 7400
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4850 7550 4850 7400
Text HLabel 4750 7550 3    50   Input ~ 0
KNX_2+
Text HLabel 4850 7550 3    50   Input ~ 0
KNX_2-
$Comp
L SparkFun-Jumpers:JUMPER-SMT_3_1-NC JP6
U 1 1 60F4FC57
P 3950 7150
F 0 "JP6" V 3900 6900 45  0000 C CNN
F 1 "JUMPER-SMT_3_1-NC" V 4100 7150 45  0000 C CNN
F 2 "Jumper:SolderJumper-3_P1.3mm_Bridged12_RoundedPad1.0x1.5mm" H 3950 7400 20  0001 C CNN
F 3 "" V 3950 7095 60  0001 C CNN
F 4 "XXX-00000" V 4275 7150 60  0001 C CNN "PROD_ID"
	1    3950 7150
	1    0    0    -1  
$EndComp
Wire Wire Line
	3800 7150 3700 7150
Wire Wire Line
	3950 7350 3950 7450
Wire Wire Line
	4100 7650 4450 7650
Wire Wire Line
	3950 8050 3950 7850
Wire Wire Line
	3950 6900 3950 6950
$Comp
L SparkFun-Jumpers:JUMPER-SMT_3_1-NC JP7
U 1 1 6109B20C
P 3950 6500
F 0 "JP7" V 3900 6250 45  0000 C CNN
F 1 "JUMPER-SMT_3_1-NC" V 4100 6500 45  0000 C CNN
F 2 "Jumper:SolderJumper-3_P1.3mm_Bridged12_RoundedPad1.0x1.5mm" H 3950 6750 20  0001 C CNN
F 3 "" V 3950 6445 60  0001 C CNN
F 4 "XXX-00000" V 4275 6500 60  0001 C CNN "PROD_ID"
	1    3950 6500
	1    0    0    1   
$EndComp
Wire Wire Line
	3700 7150 3700 6050
Wire Wire Line
	3500 6550 3500 6050
Wire Wire Line
	3400 6550 3400 6050
Wire Wire Line
	3300 6550 3300 6050
Wire Wire Line
	3200 6550 3200 6050
Wire Wire Line
	3800 6500 3800 6050
Wire Wire Line
	3950 6100 3950 6300
Wire Wire Line
	4150 6100 3950 6100
Wire Wire Line
	4650 6100 4550 6100
Wire Wire Line
	4650 6100 4650 5600
Wire Wire Line
	3950 6900 4850 6900
Wire Wire Line
	4350 8500 4350 8650
Wire Wire Line
	4350 8500 4350 6250
Connection ~ 4350 8500
Wire Wire Line
	3950 6700 4750 6700
Wire Wire Line
	4850 7400 4850 6900
Connection ~ 4850 7400
Wire Wire Line
	4750 7400 4750 7550
Wire Wire Line
	4750 7400 4750 6700
Connection ~ 4750 7400
Wire Wire Line
	4450 8500 4450 7650
Connection ~ 4450 8500
Text HLabel 6750 1750 2    50   Input ~ 0
GND_KNX
Wire Wire Line
	6750 1750 6650 1750
$Comp
L Connector_Generic:Conn_01x02 J13
U 1 1 60580A7F
P 8000 7050
F 0 "J13" H 8080 7042 50  0000 L CNN
F 1 "5V KNX Out" H 8080 6951 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 8000 7050 50  0001 C CNN
F 3 "~" H 8000 7050 50  0001 C CNN
	1    8000 7050
	1    0    0    -1  
$EndComp
Text Notes 8000 6800 0    50   ~ 0
VCC1: +3V3 KNX\nVCC2: +5V KNX
Wire Wire Line
	7800 7050 7500 7050
Wire Wire Line
	7500 7050 7500 6950
$Comp
L power:GND1 #PWR01
U 1 1 605A02CC
P 7500 7250
F 0 "#PWR01" H 7500 7000 50  0001 C CNN
F 1 "GND1" H 7500 7100 50  0000 C CNN
F 2 "" H 7500 7250 50  0001 C CNN
F 3 "" H 7500 7250 50  0001 C CNN
	1    7500 7250
	1    0    0    -1  
$EndComp
Wire Wire Line
	7500 7250 7500 7150
Wire Wire Line
	7500 7150 7800 7150
Wire Wire Line
	5900 4900 5850 4900
Text HLabel 7500 7150 0    50   Input ~ 0
GND_KNX
$Comp
L Connector_Generic:Conn_01x02 J14
U 1 1 605CCF73
P 6400 7050
F 0 "J14" H 6480 7042 50  0000 L CNN
F 1 "5V KNX Out" H 6480 6951 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 6400 7050 50  0001 C CNN
F 3 "~" H 6400 7050 50  0001 C CNN
	1    6400 7050
	1    0    0    -1  
$EndComp
$Comp
L power:GND1 #PWR02
U 1 1 605CE510
P 6050 7250
F 0 "#PWR02" H 6050 7000 50  0001 C CNN
F 1 "GND1" H 6050 7100 50  0000 C CNN
F 2 "" H 6050 7250 50  0001 C CNN
F 3 "" H 6050 7250 50  0001 C CNN
	1    6050 7250
	1    0    0    -1  
$EndComp
Text HLabel 6050 7150 0    50   Input ~ 0
GND_KNX
$Comp
L SparkFun-PowerSymbols:VCC_1 #SUPPLY02
U 1 1 605CF021
P 6050 6950
F 0 "#SUPPLY02" H 6100 6950 45  0001 L BNN
F 1 "VCC_1" H 6050 7226 45  0000 C CNN
F 2 "KNX+ 3V3" H 6050 7131 60  0000 C CNN
F 3 "" H 6050 6950 60  0001 C CNN
	1    6050 6950
	1    0    0    -1  
$EndComp
Wire Wire Line
	6050 7050 6200 7050
Wire Wire Line
	6050 6950 6050 7050
Wire Wire Line
	6050 7250 6050 7150
Wire Wire Line
	6050 7150 6200 7150
$Comp
L SparkFun-PowerSymbols:VCC_2 #SUPPLY01
U 1 1 605C493A
P 7500 6950
F 0 "#SUPPLY01" H 7550 6950 45  0001 L BNN
F 1 "VCC_2" H 7500 7226 45  0000 C CNN
F 2 "KNX+ 5V" H 7500 7131 60  0000 C CNN
F 3 "" H 7500 6950 60  0001 C CNN
	1    7500 6950
	1    0    0    -1  
$EndComp
$Comp
L SparkFun-PowerSymbols:VCC_2 #SUPPLY0101
U 1 1 605EEAFA
P 5950 4300
F 0 "#SUPPLY0101" H 6000 4300 45  0001 L BNN
F 1 "VCC_2" H 5950 4470 45  0000 C CNN
F 2 "KNX+ 5V" H 5950 4565 60  0000 C CNN
F 3 "" H 5950 4300 60  0001 C CNN
	1    5950 4300
	0    -1   -1   0   
$EndComp
$Comp
L power:GND1 #PWR0111
U 1 1 605FE152
P 7100 3600
F 0 "#PWR0111" H 7100 3350 50  0001 C CNN
F 1 "GND1" H 7100 3450 50  0000 C CNN
F 2 "" H 7100 3600 50  0001 C CNN
F 3 "" H 7100 3600 50  0001 C CNN
	1    7100 3600
	0    -1   -1   0   
$EndComp
Connection ~ 7100 3600
Wire Wire Line
	7100 3600 7050 3600
$Comp
L power:GND1 #PWR0112
U 1 1 605FE995
P 6650 1750
F 0 "#PWR0112" H 6650 1500 50  0001 C CNN
F 1 "GND1" H 6650 1600 50  0000 C CNN
F 2 "" H 6650 1750 50  0001 C CNN
F 3 "" H 6650 1750 50  0001 C CNN
	1    6650 1750
	0    -1   -1   0   
$EndComp
Connection ~ 6650 1750
Wire Wire Line
	6650 1750 6550 1750
$Comp
L power:GND1 #PWR0113
U 1 1 605FED5E
P 5250 1500
F 0 "#PWR0113" H 5250 1250 50  0001 C CNN
F 1 "GND1" H 5250 1350 50  0000 C CNN
F 2 "" H 5250 1500 50  0001 C CNN
F 3 "" H 5250 1500 50  0001 C CNN
	1    5250 1500
	0    -1   -1   0   
$EndComp
Connection ~ 5250 1500
Wire Wire Line
	5250 1500 5150 1500
$Comp
L power:GND1 #PWR0114
U 1 1 605FF36F
P 4050 4200
F 0 "#PWR0114" H 4050 3950 50  0001 C CNN
F 1 "GND1" H 4050 4050 50  0000 C CNN
F 2 "" H 4050 4200 50  0001 C CNN
F 3 "" H 4050 4200 50  0001 C CNN
	1    4050 4200
	0    -1   -1   0   
$EndComp
Connection ~ 4050 4200
Wire Wire Line
	4050 4200 4150 4200
Connection ~ 6600 4300
Wire Wire Line
	6600 4300 6550 4300
$Comp
L power:GND1 #PWR0115
U 1 1 60600214
P 6600 4300
F 0 "#PWR0115" H 6600 4050 50  0001 C CNN
F 1 "GND1" H 6600 4150 50  0000 C CNN
F 2 "" H 6600 4300 50  0001 C CNN
F 3 "" H 6600 4300 50  0001 C CNN
	1    6600 4300
	0    -1   -1   0   
$EndComp
$Comp
L power:GND1 #PWR0116
U 1 1 60600252
P 5850 4900
F 0 "#PWR0116" H 5850 4650 50  0001 C CNN
F 1 "GND1" H 5850 4750 50  0000 C CNN
F 2 "" H 5850 4900 50  0001 C CNN
F 3 "" H 5850 4900 50  0001 C CNN
	1    5850 4900
	0    -1   -1   0   
$EndComp
Connection ~ 5850 4900
Wire Wire Line
	5850 4900 5750 4900
$Comp
L power:GND1 #PWR0117
U 1 1 60608C0C
P 4100 8050
F 0 "#PWR0117" H 4100 7800 50  0001 C CNN
F 1 "GND1" H 4100 7900 50  0000 C CNN
F 2 "" H 4100 8050 50  0001 C CNN
F 3 "" H 4100 8050 50  0001 C CNN
	1    4100 8050
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3950 8050 4100 8050
$Comp
L SparkFun-PowerSymbols:VCC_1 #SUPPLY0102
U 1 1 60638D00
P 5950 1750
F 0 "#SUPPLY0102" H 6000 1750 45  0001 L BNN
F 1 "VCC_1" H 5950 1920 45  0000 C CNN
F 2 "KNX+ 3V3" H 5950 2015 60  0000 C CNN
F 3 "" H 5950 1750 60  0001 C CNN
	1    5950 1750
	0    -1   -1   0   
$EndComp
$Comp
L SparkFun-PowerSymbols:VCC_1 #SUPPLY0103
U 1 1 606421AB
P 3650 1500
F 0 "#SUPPLY0103" H 3700 1500 45  0001 L BNN
F 1 "VCC_1" H 3650 1670 45  0000 C CNN
F 2 "KNX+ 3V3" H 3650 1765 60  0000 C CNN
F 3 "" H 3650 1500 60  0001 C CNN
	1    3650 1500
	0    -1   -1   0   
$EndComp
$Comp
L SparkFun-PowerSymbols:VCC_1 #SUPPLY0104
U 1 1 60642DB9
P 3250 4200
F 0 "#SUPPLY0104" H 3300 4200 45  0001 L BNN
F 1 "VCC_1" H 3250 4370 45  0000 C CNN
F 2 "KNX+ 3V3" H 3250 4465 60  0000 C CNN
F 3 "" H 3250 4200 60  0001 C CNN
	1    3250 4200
	0    -1   -1   0   
$EndComp
$Comp
L SparkFun-PowerSymbols:VCC_2 #SUPPLY0105
U 1 1 6064DD56
P 5300 5000
F 0 "#SUPPLY0105" H 5350 5000 45  0001 L BNN
F 1 "VCC_2" V 5247 5128 45  0000 L CNN
F 2 "KNX+ 5V" V 5342 5128 60  0000 L CNN
F 3 "" H 5300 5000 60  0001 C CNN
	1    5300 5000
	-1   0    0    1   
$EndComp
$Comp
L power:GND1 #PWR03
U 1 1 6079E000
P 6000 2700
F 0 "#PWR03" H 6000 2450 50  0001 C CNN
F 1 "GND1" H 6000 2550 50  0000 C CNN
F 2 "" H 6000 2700 50  0001 C CNN
F 3 "" H 6000 2700 50  0001 C CNN
	1    6000 2700
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6000 2700 5750 2700
$Comp
L power:GND1 #PWR04
U 1 1 607D593C
P 6000 3100
F 0 "#PWR04" H 6000 2850 50  0001 C CNN
F 1 "GND1" H 6000 2950 50  0000 C CNN
F 2 "" H 6000 3100 50  0001 C CNN
F 3 "" H 6000 3100 50  0001 C CNN
	1    6000 3100
	0    -1   -1   0   
$EndComp
$EndSCHEMATC
