EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 5
Title "Homekit Teensy 4.1"
Date "2021-03-19"
Rev "0.1"
Comp "michael"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L SparkFun-Connectors:I2C_STANDARDQWIIC J11
U 1 1 60B9EDA6
P 1200 2350
F 0 "J11" H 1100 2750 45  0000 C CNN
F 1 "J I2C/Serial QWIIC" V 950 2500 45  0000 C CNN
F 2 "Connector_JST:JST_SH_SM04B-SRSS-TB_1x04-1MP_P1.00mm_Horizontal" H 1200 2850 20  0001 C CNN
F 3 "" H 1200 2350 50  0001 C CNN
F 4 "CONN-13729" H 1100 2100 60  0001 C CNN "Field4"
	1    1200 2350
	1    0    0    -1  
$EndComp
$Comp
L SparkFun-Jumpers:JUMPER-SMT_3_NO JP10
U 1 1 60BA1F2D
P 2050 2250
F 0 "JP10" V 1900 2350 45  0000 C CNN
F 1 "SJ QWIIC PULLUPS" H 1700 2450 45  0000 C CNN
F 2 "Jumper:SolderJumper-3_P1.3mm_Open_Pad1.0x1.5mm" H 2050 2500 20  0001 C CNN
F 3 "" H 2050 2250 60  0001 C CNN
F 4 "XXX-00000" V 2375 2250 60  0001 C CNN "PROD_ID"
	1    2050 2250
	0    1    1    0   
$EndComp
$Comp
L Device:R R18
U 1 1 60BA4FF4
P 1850 2500
F 0 "R18" V 1950 2450 50  0000 L CNN
F 1 "4k7" V 1850 2400 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 1780 2500 50  0001 C CNN
F 3 "~" H 1850 2500 50  0001 C CNN
	1    1850 2500
	1    0    0    -1  
$EndComp
$Comp
L Device:R R19
U 1 1 60BA54DE
P 2250 2500
F 0 "R19" V 2350 2450 50  0000 L CNN
F 1 "4k7" V 2250 2400 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 2180 2500 50  0001 C CNN
F 3 "~" H 2250 2500 50  0001 C CNN
	1    2250 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	1850 2700 1850 2650
Wire Wire Line
	2250 2800 2250 2650
Wire Wire Line
	2250 2350 2250 2250
Wire Wire Line
	1850 2350 1850 2250
$Comp
L power:+3.3V #PWR041
U 1 1 60BF00C6
P 2050 1900
F 0 "#PWR041" H 2050 1750 50  0001 C CNN
F 1 "+3.3V" H 2050 2050 50  0000 C CNN
F 2 "" H 2050 1900 50  0001 C CNN
F 3 "" H 2050 1900 50  0001 C CNN
	1    2050 1900
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR037
U 1 1 60BF1761
P 1400 1900
F 0 "#PWR037" H 1400 1750 50  0001 C CNN
F 1 "+3.3V" H 1400 2050 50  0000 C CNN
F 2 "" H 1400 1900 50  0001 C CNN
F 3 "" H 1400 1900 50  0001 C CNN
	1    1400 1900
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x12 J15
U 1 1 60FC9013
P 8650 2250
F 0 "J15" H 8650 1550 50  0000 C CNN
F 1 "J Faceplate Connector" V 8750 2200 50  0000 C CNN
F 2 "Connector_FFC-FPC:Molex_200528-0120_1x12-1MP_P1.00mm_Horizontal" H 8650 2250 50  0001 C CNN
F 3 "~" H 8650 2250 50  0001 C CNN
	1    8650 2250
	-1   0    0    1   
$EndComp
$Comp
L power:+3.3V #PWR054
U 1 1 60FD4BB6
P 8950 1550
F 0 "#PWR054" H 8950 1400 50  0001 C CNN
F 1 "+3.3V" H 8900 1700 50  0000 C CNN
F 2 "" H 8950 1550 50  0001 C CNN
F 3 "" H 8950 1550 50  0001 C CNN
	1    8950 1550
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR055
U 1 1 60FD5428
P 8950 2850
F 0 "#PWR055" H 8950 2600 50  0001 C CNN
F 1 "GND" H 8950 2700 50  0000 C CNN
F 2 "" H 8950 2850 50  0001 C CNN
F 3 "" H 8950 2850 50  0001 C CNN
	1    8950 2850
	1    0    0    -1  
$EndComp
Wire Wire Line
	8950 2850 8950 2750
Wire Wire Line
	8950 2750 8850 2750
Wire Wire Line
	8850 1650 8950 1650
Wire Wire Line
	8950 1650 8950 1550
Text Notes 8750 650  0    74   ~ 0
Faceplate Connector
$Comp
L Connector_Generic:Conn_01x04 J9
U 1 1 612A1752
P 1100 2900
F 0 "J9" H 1100 2600 50  0000 C CNN
F 1 "J I2C/Serial" V 1200 2850 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x04_P2.54mm_Vertical" H 1100 2900 50  0001 C CNN
F 3 "~" H 1100 2900 50  0001 C CNN
	1    1100 2900
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR038
U 1 1 612A3E27
P 1500 3100
F 0 "#PWR038" H 1500 2850 50  0001 C CNN
F 1 "GND" H 1500 2950 50  0000 C CNN
F 2 "" H 1500 3100 50  0001 C CNN
F 3 "" H 1500 3100 50  0001 C CNN
	1    1500 3100
	1    0    0    -1  
$EndComp
Text Notes 800  2000 3    74   ~ 0
I2C / Serial
Text Notes 800  6350 3    74   ~ 0
SPI+
Text Notes 6300 6450 0    50   ~ 0
ADUM Pin 1 (VDD1)    <->    +5V BCU\nADUM Pin 2 (TX 1)    <->    RX BCU\nADUM Pin 3 (RX 1)    <->    TX BCU\nADUM Pin 4 (GND1)    <->    GND BCU\n\nADUM Pin 8 (VDD2)    <->    VCC Arduino\nADUM Pin 7 (RX 2)    <->    TX Arduino\nADUM Pin 6 (TX 2)    <->    RX Arduino\nADUM Pin 5 (GND1)    <->    GND Arduin
$Comp
L dk_USB-DVI-HDMI-Connectors:0475890001 J10
U 1 1 6054C956
P 1150 4150
F 0 "J10" H 1050 4750 50  0000 C CNN
F 1 "Micro USB" V 900 4150 50  0000 C CNN
F 2 "digikey-footprints:USB_Micro_AB_Female_0475890001" H 1350 4350 60  0001 L CNN
F 3 "https://www.molex.com/pdm_docs/sd/475890001_sd.pdf" H 1350 4450 60  0001 L CNN
F 4 "WM17143CT-ND" H 1350 4550 60  0001 L CNN "Digi-Key_PN"
F 5 "0475890001" H 1350 4650 60  0001 L CNN "MPN"
F 6 "Connectors, Interconnects" H 1350 4750 60  0001 L CNN "Category"
F 7 "USB, DVI, HDMI Connectors" H 1350 4850 60  0001 L CNN "Family"
F 8 "https://www.molex.com/pdm_docs/sd/475890001_sd.pdf" H 1350 4950 60  0001 L CNN "DK_Datasheet_Link"
F 9 "/product-detail/en/molex/0475890001/WM17143CT-ND/1832255" H 1350 5050 60  0001 L CNN "DK_Detail_Page"
F 10 "CONN RCPT MICRO USB AB 5P SMD RA" H 1350 5150 60  0001 L CNN "Description"
F 11 "Molex" H 1350 5250 60  0001 L CNN "Manufacturer"
F 12 "Active" H 1350 5350 60  0001 L CNN "Status"
	1    1150 4150
	1    0    0    -1  
$EndComp
Connection ~ 8600 5950
Wire Wire Line
	8600 5950 8250 5950
$Comp
L Device:C C21
U 1 1 60542C34
P 8400 5350
F 0 "C21" V 8148 5350 50  0000 C CNN
F 1 "100nF / 6.3V" V 8250 5450 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 8438 5200 50  0001 C CNN
F 3 "~" H 8400 5350 50  0001 C CNN
	1    8400 5350
	0    -1   1    0   
$EndComp
Wire Wire Line
	10150 5350 10150 5550
Connection ~ 10150 5350
$Comp
L Device:C C24
U 1 1 60503D86
P 10300 5350
F 0 "C24" V 10048 5350 50  0000 C CNN
F 1 "100nF / 6.3V" V 10139 5350 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 10338 5200 50  0001 C CNN
F 3 "~" H 10300 5350 50  0001 C CNN
	1    10300 5350
	0    -1   1    0   
$EndComp
Text Notes 8800 4850 0    74   ~ 0
Galv. Trennung µBCU
Wire Wire Line
	8600 5650 8850 5650
Wire Wire Line
	8600 5850 8850 5850
Wire Wire Line
	8850 5550 8600 5550
Wire Wire Line
	8600 5950 8600 6050
Wire Wire Line
	8850 5950 8600 5950
$Comp
L power:GND #PWR051
U 1 1 604FC99E
P 8600 6050
F 0 "#PWR051" H 8600 5800 50  0001 C CNN
F 1 "GND" H 8600 5900 50  0000 C CNN
F 2 "" H 8600 6050 50  0001 C CNN
F 3 "" H 8600 6050 50  0001 C CNN
	1    8600 6050
	-1   0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR050
U 1 1 604FC060
P 8600 5150
F 0 "#PWR050" H 8600 5000 50  0001 C CNN
F 1 "+3V3" H 8600 5300 50  0000 C CNN
F 2 "" H 8600 5150 50  0001 C CNN
F 3 "" H 8600 5150 50  0001 C CNN
	1    8600 5150
	-1   0    0    -1  
$EndComp
$Comp
L Isolator:ADuM1201AR U?
U 1 1 604F9AE0
P 9350 5750
AR Path="/605817AD/604F9AE0" Ref="U?"  Part="1" 
AR Path="/604F9AE0" Ref="U?"  Part="1" 
AR Path="/605467E9/604F9AE0" Ref="U4"  Part="1" 
F 0 "U4" H 9350 6217 50  0000 C CNN
F 1 "ADuM1201AR" H 9350 6126 50  0000 C CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 9350 5350 50  0001 C CIN
F 3 "https://www.analog.com/media/en/technical-documentation/data-sheets/ADuM1200_1201.pdf" H 9350 5650 50  0001 C CNN
	1    9350 5750
	-1   0    0    -1  
$EndComp
Wire Wire Line
	10150 4950 10150 5350
$Comp
L Device:CP C19
U 1 1 607028F2
P 6300 4000
F 0 "C19" V 6350 3800 50  0000 L CNN
F 1 "0.1µF / 50V" V 6150 3750 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 6338 3850 50  0001 C CNN
F 3 "~" H 6300 4000 50  0001 C CNN
	1    6300 4000
	1    0    0    -1  
$EndComp
Text Notes 7400 4750 1    74   ~ 0
Teensy 4.1 Ethernet
$Comp
L power:GND #PWR047
U 1 1 6055753D
P 6450 5150
F 0 "#PWR047" H 6450 4900 50  0001 C CNN
F 1 "GND" H 6450 5000 50  0000 C CNN
F 2 "" H 6450 5150 50  0001 C CNN
F 3 "" H 6450 5150 50  0001 C CNN
	1    6450 5150
	1    0    0    -1  
$EndComp
Text Notes 7300 1100 2    74   ~ 0
ATECC608A
$Comp
L power:+3.3V #PWR046
U 1 1 60B4AC80
P 6350 1350
F 0 "#PWR046" H 6350 1200 50  0001 C CNN
F 1 "+3.3V" H 6350 1500 50  0000 C CNN
F 2 "" H 6350 1350 50  0001 C CNN
F 3 "" H 6350 1350 50  0001 C CNN
	1    6350 1350
	-1   0    0    -1  
$EndComp
$Comp
L Device:R R20
U 1 1 60B14A4A
P 6150 1850
F 0 "R20" V 6250 1800 50  0000 L CNN
F 1 "4k7" V 6150 1750 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 6080 1850 50  0001 C CNN
F 3 "~" H 6150 1850 50  0001 C CNN
	1    6150 1850
	-1   0    0    -1  
$EndComp
$Comp
L SparkFun-Jumpers:JUMPER-SMT_3_2-NC JP11
U 1 1 60B1148A
P 6350 1600
F 0 "JP11" V 6500 1650 45  0000 C CNN
F 1 "SJ ATECC608A PULLUPS" H 6400 1300 45  0000 C CNN
F 2 "Homekit:SolderJumper-3_P1.3mm_Bridged2Bar123_Pad1.0x1.5mm" H 6350 1850 20  0001 C CNN
F 3 "" H 6350 1600 60  0001 C CNN
F 4 "XXX-00000" V 6675 1600 60  0001 C CNN "PROD_ID"
	1    6350 1600
	0    -1   1    0   
$EndComp
$Comp
L power:+3.3V #PWR049
U 1 1 60ADE336
P 7500 1650
F 0 "#PWR049" H 7500 1500 50  0001 C CNN
F 1 "+3.3V" H 7500 1800 50  0000 C CNN
F 2 "" H 7500 1650 50  0001 C CNN
F 3 "" H 7500 1650 50  0001 C CNN
	1    7500 1650
	-1   0    0    -1  
$EndComp
$Comp
L power:GND #PWR048
U 1 1 60AD1A48
P 7100 2650
F 0 "#PWR048" H 7100 2400 50  0001 C CNN
F 1 "GND" H 7100 2500 50  0000 C CNN
F 2 "" H 7100 2650 50  0001 C CNN
F 3 "" H 7100 2650 50  0001 C CNN
	1    7100 2650
	-1   0    0    -1  
$EndComp
$Comp
L Security:ATECC608A-SSHDA U3
U 1 1 60AD0543
P 7100 2150
F 0 "U3" H 7250 1900 50  0000 R CNN
F 1 "ATECC608A" H 7550 1800 50  0000 R CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 7100 2150 50  0001 C CNN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/ATECC608A-CryptoAuthentication-Device-Summary-Data-Sheet-DS40001977B.pdf" H 7250 2400 50  0001 C CNN
	1    7100 2150
	-1   0    0    -1  
$EndComp
NoConn ~ 6550 4650
NoConn ~ 6550 4550
NoConn ~ 6550 3950
NoConn ~ 6550 3750
$Comp
L J1B1211CCD:J1B1211CCD P1
U 1 1 6068F96C
P 6950 4150
F 0 "P1" H 6650 5100 50  0000 L CNN
F 1 "J1B1211CCD" H 6650 5200 50  0000 L CNN
F 2 "J1B1211CCD:J1B1211CCD" H 6950 4150 50  0001 L BNN
F 3 "" H 6950 4150 50  0001 L BNN
	1    6950 4150
	1    0    0    -1  
$EndComp
Wire Wire Line
	6550 4850 6450 4850
Wire Wire Line
	6550 4050 6450 4050
Wire Wire Line
	6550 3450 6300 3450
$Comp
L Device:C C20
U 1 1 60ADDDD7
P 7500 2150
F 0 "C20" V 7450 1950 50  0000 L CNN
F 1 "0.1µF / 6.3V" V 7350 1900 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 7538 2000 50  0001 C CNN
F 3 "~" H 7500 2150 50  0001 C CNN
	1    7500 2150
	-1   0    0    -1  
$EndComp
Wire Wire Line
	7100 2450 7100 2550
Wire Wire Line
	7100 2550 7500 2550
Wire Wire Line
	7500 2550 7500 2300
Connection ~ 7100 2550
Wire Wire Line
	7100 2550 7100 2650
Wire Wire Line
	7100 1850 7100 1750
Wire Wire Line
	7100 1750 7500 1750
Wire Wire Line
	7500 1650 7500 1750
Connection ~ 7500 1750
Wire Wire Line
	7500 1750 7500 2000
Wire Wire Line
	6700 2200 6700 2250
Wire Wire Line
	6700 2250 6800 2250
Wire Wire Line
	6800 2050 6700 2050
Wire Wire Line
	6700 2050 6700 2100
Wire Wire Line
	6700 2100 6150 2100
Wire Wire Line
	6150 2100 6150 2000
Wire Wire Line
	6150 1700 6150 1600
$Comp
L Device:R R21
U 1 1 60CF299B
P 6550 1850
F 0 "R21" V 6650 1800 50  0000 L CNN
F 1 "4k7" V 6550 1750 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 6480 1850 50  0001 C CNN
F 3 "~" H 6550 1850 50  0001 C CNN
	1    6550 1850
	-1   0    0    -1  
$EndComp
Wire Wire Line
	6550 1700 6550 1600
Wire Wire Line
	6550 2000 6550 2200
Wire Wire Line
	6550 2200 6700 2200
Wire Wire Line
	6350 1450 6350 1350
Wire Wire Line
	1450 3950 3300 3950
Wire Wire Line
	1450 4050 3300 4050
Wire Wire Line
	1450 4150 3300 4150
Wire Wire Line
	1450 4250 3200 4250
Wire Wire Line
	1450 4350 1500 4350
Text Notes 800  3900 3    74   ~ 0
USB Host
Text Label 8600 5850 2    50   ~ 0
RX8
Text Label 8600 5650 2    50   ~ 0
TX8
Text HLabel 10700 5850 2    50   Input ~ 0
KNX2_TX
Text HLabel 10700 5750 2    50   Output ~ 0
KNX2_RX
Text HLabel 10700 5650 2    50   UnSpc ~ 0
KNX2_3V3
Text HLabel 10700 5950 2    50   UnSpc ~ 0
KNX2_GND
Wire Wire Line
	8600 5150 8600 5350
Wire Wire Line
	8550 5350 8600 5350
Connection ~ 8600 5350
Wire Wire Line
	8600 5350 8600 5550
Wire Wire Line
	8250 5350 8250 5950
Wire Wire Line
	10450 5350 10450 5950
Wire Wire Line
	10600 4950 10600 5650
Wire Wire Line
	10600 4950 10150 4950
Wire Wire Line
	10700 5650 10600 5650
Wire Wire Line
	10700 5750 10150 5750
Wire Wire Line
	10700 5950 10450 5950
Connection ~ 10450 5950
Connection ~ 8650 4350
Wire Wire Line
	8650 4350 8300 4350
$Comp
L Device:C C22
U 1 1 6119E40D
P 8450 3750
F 0 "C22" V 8198 3750 50  0000 C CNN
F 1 "100nF / 6.3V" V 8300 3850 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 8488 3600 50  0001 C CNN
F 3 "~" H 8450 3750 50  0001 C CNN
	1    8450 3750
	0    -1   1    0   
$EndComp
Wire Wire Line
	10100 3750 10100 3950
Connection ~ 10100 3750
$Comp
L Device:C C23
U 1 1 6119E419
P 10250 3750
F 0 "C23" V 9998 3750 50  0000 C CNN
F 1 "100nF / 6.3V" V 10089 3750 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 10288 3600 50  0001 C CNN
F 3 "~" H 10250 3750 50  0001 C CNN
	1    10250 3750
	0    -1   1    0   
$EndComp
Text Notes 8750 3250 0    74   ~ 0
Galv. Trennung NCN5120
Wire Wire Line
	10100 3950 9900 3950
Wire Wire Line
	8650 4050 8900 4050
Wire Wire Line
	8650 4250 8900 4250
Wire Wire Line
	8900 3950 8650 3950
Wire Wire Line
	8650 4350 8650 4450
Wire Wire Line
	8900 4350 8650 4350
$Comp
L power:GND #PWR053
U 1 1 6119E42A
P 8650 4450
F 0 "#PWR053" H 8650 4200 50  0001 C CNN
F 1 "GND" H 8650 4300 50  0000 C CNN
F 2 "" H 8650 4450 50  0001 C CNN
F 3 "" H 8650 4450 50  0001 C CNN
	1    8650 4450
	-1   0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR052
U 1 1 6119E434
P 8650 3550
F 0 "#PWR052" H 8650 3400 50  0001 C CNN
F 1 "+3V3" H 8650 3700 50  0000 C CNN
F 2 "" H 8650 3550 50  0001 C CNN
F 3 "" H 8650 3550 50  0001 C CNN
	1    8650 3550
	-1   0    0    -1  
$EndComp
$Comp
L Isolator:ADuM1201AR U?
U 1 1 6119E43E
P 9400 4150
AR Path="/605817AD/6119E43E" Ref="U?"  Part="1" 
AR Path="/6119E43E" Ref="U?"  Part="1" 
AR Path="/605467E9/6119E43E" Ref="U5"  Part="1" 
F 0 "U5" H 9400 4617 50  0000 C CNN
F 1 "ADuM1201AR" H 9400 4526 50  0000 C CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 9400 3750 50  0001 C CIN
F 3 "https://www.analog.com/media/en/technical-documentation/data-sheets/ADuM1200_1201.pdf" H 9400 4050 50  0001 C CNN
	1    9400 4150
	-1   0    0    -1  
$EndComp
Wire Wire Line
	9900 4050 10000 4050
Wire Wire Line
	10100 3350 10100 3750
Wire Wire Line
	9900 4350 10400 4350
Wire Wire Line
	10000 4150 10000 4050
Text Label 8650 4250 2    50   ~ 0
RX1
Text Label 8650 4050 2    50   ~ 0
TX1
Text HLabel 10650 4250 2    50   Input ~ 0
KNX1_TX
Text HLabel 10650 4150 2    50   Output ~ 0
KNX1_RX
Text HLabel 10650 4050 2    50   UnSpc ~ 0
KNX1_3V3
Text HLabel 10650 4350 2    50   UnSpc ~ 0
KNX1_GND
Wire Wire Line
	8650 3550 8650 3750
Wire Wire Line
	8600 3750 8650 3750
Connection ~ 8650 3750
Wire Wire Line
	8650 3750 8650 3950
Wire Wire Line
	8300 3750 8300 4350
Wire Wire Line
	10400 3750 10400 4350
Wire Wire Line
	10550 3350 10550 4050
Wire Wire Line
	10550 3350 10100 3350
Wire Wire Line
	10650 4050 10550 4050
Wire Wire Line
	10650 4150 10000 4150
Wire Wire Line
	10650 4250 9900 4250
Wire Wire Line
	10650 4350 10400 4350
Connection ~ 10400 4350
Text Label 3100 1300 2    50   ~ 0
RX1
Wire Wire Line
	3100 1300 3300 1300
Text Label 3100 1400 2    50   ~ 0
TX1
Wire Wire Line
	3100 1400 3300 1400
Text HLabel 10700 6050 2    50   UnSpc ~ 0
KNX2_5V
Text HLabel 10650 4450 2    50   UnSpc ~ 0
KNX1_5V
NoConn ~ 10550 4450
Wire Wire Line
	10550 4450 10650 4450
Wire Wire Line
	10450 6050 10700 6050
Wire Wire Line
	10150 5750 10150 5650
Wire Wire Line
	9850 5650 10150 5650
Wire Wire Line
	8850 1750 9100 1750
Text Label 9200 1350 1    50   ~ 0
LED_ON
Wire Wire Line
	9100 1350 9100 1750
Wire Wire Line
	8850 1850 9200 1850
Text Label 9100 1350 1    50   ~ 0
LED_PROG
Wire Wire Line
	9200 1350 9200 1850
Wire Wire Line
	8850 1950 9300 1950
Text Label 9300 1350 1    50   ~ 0
LED_CONN
Wire Wire Line
	9300 1350 9300 1950
Wire Wire Line
	8850 2050 9400 2050
Text Label 9400 1350 1    50   ~ 0
LED_ERROR
Wire Wire Line
	9400 1350 9400 2050
Wire Wire Line
	8850 2150 9500 2150
Text Label 9500 1350 1    50   ~ 0
LED_T41_PROG
Wire Wire Line
	8850 2250 9600 2250
Wire Wire Line
	8850 2350 9700 2350
Text Label 9600 1350 1    50   ~ 0
SCL0
Text Label 9700 1350 1    50   ~ 0
SDA0
Wire Wire Line
	9700 2350 9700 1350
Wire Wire Line
	9600 2250 9600 1350
Wire Wire Line
	9500 1350 9500 2150
Text HLabel 10150 2150 2    50   Output ~ 0
LED_T41_PROG
Text HLabel 10150 2050 2    50   Output ~ 0
LED_ERROR
Text HLabel 10150 1950 2    50   Output ~ 0
LED_CONN
Text HLabel 10150 1750 2    50   Output ~ 0
LED_PROG
Text HLabel 10150 1850 2    50   Output ~ 0
LED_ON
Text HLabel 10150 2350 2    50   BiDi ~ 0
SDA0
Text HLabel 10150 2250 2    50   BiDi ~ 0
SCL0
Text HLabel 10150 2450 2    50   Input ~ 0
PUSH_PROG
Text HLabel 10150 2650 2    50   Input ~ 0
PUSH_T41_PROG
Text HLabel 10150 2550 2    50   Input ~ 0
PUSH_RESET
Text Notes 10950 2450 3    50   ~ 0
PUSH
Text Notes 10950 2200 3    50   ~ 0
I2C
Text Notes 10950 1650 3    50   ~ 0
STATUS LEDS
Text Label 9800 1350 1    50   ~ 0
PUSH_PROG
Text Label 9900 1350 1    50   ~ 0
PUSH_RESET
Wire Wire Line
	8850 2650 10000 2650
Wire Wire Line
	10150 2650 10000 2650
Connection ~ 10000 2650
Wire Wire Line
	8850 2550 9900 2550
Wire Wire Line
	9900 2550 9900 1350
Connection ~ 9900 2550
Wire Wire Line
	9900 2550 10150 2550
Wire Wire Line
	8850 2450 9800 2450
Wire Wire Line
	9800 2450 9800 1350
Connection ~ 9800 2450
Wire Wire Line
	9800 2450 10150 2450
Wire Wire Line
	9700 2350 10150 2350
Connection ~ 9700 2350
Wire Wire Line
	9600 2250 10150 2250
Connection ~ 9600 2250
Wire Wire Line
	9500 2150 10150 2150
Connection ~ 9500 2150
Wire Wire Line
	9400 2050 10150 2050
Connection ~ 9400 2050
Wire Wire Line
	9300 1950 10150 1950
Connection ~ 9300 1950
Wire Wire Line
	9200 1850 10150 1850
Connection ~ 9200 1850
Wire Wire Line
	9100 1750 10150 1750
Connection ~ 9100 1750
Text Label 4550 4750 3    50   ~ 0
PUSH_T41_PROG
Text Label 3100 3500 2    50   ~ 0
PUSH_RESET
Wire Wire Line
	3100 3500 3300 3500
Text Label 3100 2200 2    50   ~ 0
PUSH_PROG
Wire Wire Line
	3100 2200 3300 2200
Text Label 3100 1900 2    50   ~ 0
LED_T41_PROG
Wire Wire Line
	3100 1900 3300 1900
Text Label 3100 1800 2    50   ~ 0
LED_ERROR
Wire Wire Line
	3100 1800 3300 1800
Text Label 3100 1700 2    50   ~ 0
LED_CONN
Wire Wire Line
	3100 1700 3300 1700
Text Label 3100 1500 2    50   ~ 0
LED_PROG
Wire Wire Line
	3100 1600 3300 1600
Text Label 3100 1600 2    50   ~ 0
LED_ON
Wire Wire Line
	3100 1500 3300 1500
Text Label 5700 1900 0    50   ~ 0
SCL0
Text Label 5700 2000 0    50   ~ 0
SDA0
Wire Wire Line
	5700 2000 5500 2000
Wire Wire Line
	5700 1900 5500 1900
Wire Wire Line
	4550 4750 4550 4650
Wire Wire Line
	4400 4750 4200 4750
Wire Wire Line
	4400 4750 4400 4650
Text Notes 4650 4100 0    50   ~ 0
65 - 6\n64 - GND\n63 - 1\n62 - 3\n61 - 10\n60 - 4
Wire Wire Line
	6450 5150 6450 4950
Connection ~ 6450 4950
Wire Wire Line
	6450 4950 6550 4950
Wire Wire Line
	6450 4950 6450 4850
Connection ~ 6450 4850
Wire Wire Line
	6450 4850 6450 4250
Connection ~ 6450 4250
Wire Wire Line
	6450 4250 6550 4250
Wire Wire Line
	6450 4250 6450 4050
Wire Wire Line
	6300 4250 6300 4150
Wire Wire Line
	6300 4250 6450 4250
Wire Wire Line
	6300 3850 6300 3450
Wire Notes Line rgb(194, 194, 194)
	7950 6250 7950 200 
Wire Notes Line rgb(194, 194, 194)
	7950 4400 11200 4400
Wire Notes Line rgb(194, 194, 194)
	7950 2800 11200 2800
Text Label 3100 2500 2    50   ~ 0
MISO0
Text Label 3100 2400 2    50   ~ 0
MOSI0
Text Label 3100 2300 2    50   ~ 0
CS0
Wire Wire Line
	3100 2300 3300 2300
Wire Wire Line
	3100 2400 3300 2400
Wire Wire Line
	3100 2500 3300 2500
Text Label 5700 2500 0    50   ~ 0
SCK0
Wire Wire Line
	5700 2500 5500 2500
$Comp
L power:+3.3V #PWR043
U 1 1 61DFD894
P 3250 950
F 0 "#PWR043" H 3250 800 50  0001 C CNN
F 1 "+3.3V" H 3250 1100 50  0000 C CNN
F 2 "" H 3250 950 50  0001 C CNN
F 3 "" H 3250 950 50  0001 C CNN
	1    3250 950 
	1    0    0    -1  
$EndComp
Wire Wire Line
	3250 950  3250 2600
Wire Wire Line
	3250 2600 3300 2600
$Comp
L power:GND #PWR042
U 1 1 61E071D3
P 3200 4500
F 0 "#PWR042" H 3200 4250 50  0001 C CNN
F 1 "GND" H 3200 4350 50  0000 C CNN
F 2 "" H 3200 4500 50  0001 C CNN
F 3 "" H 3200 4500 50  0001 C CNN
	1    3200 4500
	-1   0    0    -1  
$EndComp
Wire Wire Line
	3200 4500 3200 4350
Connection ~ 3200 4350
Wire Wire Line
	3200 4350 3300 4350
Wire Wire Line
	3200 4350 3200 4250
Connection ~ 3200 4250
Wire Wire Line
	3200 4250 3300 4250
Wire Wire Line
	3200 4250 3200 1200
Wire Wire Line
	3200 1200 3300 1200
$Comp
L power:+3.3V #PWR044
U 1 1 61E23F01
P 5550 950
F 0 "#PWR044" H 5550 800 50  0001 C CNN
F 1 "+3.3V" H 5550 1100 50  0000 C CNN
F 2 "" H 5550 950 50  0001 C CNN
F 3 "" H 5550 950 50  0001 C CNN
	1    5550 950 
	1    0    0    -1  
$EndComp
Wire Wire Line
	5500 1400 5550 1400
Wire Wire Line
	5550 1400 5550 950 
Wire Wire Line
	5500 1300 5600 1300
Wire Wire Line
	5600 1300 5600 2600
Wire Wire Line
	5600 2600 5500 2600
$Comp
L power:GND #PWR045
U 1 1 61E39207
P 5600 4500
F 0 "#PWR045" H 5600 4250 50  0001 C CNN
F 1 "GND" H 5600 4350 50  0000 C CNN
F 2 "" H 5600 4500 50  0001 C CNN
F 3 "" H 5600 4500 50  0001 C CNN
	1    5600 4500
	-1   0    0    -1  
$EndComp
Connection ~ 5600 2600
Wire Wire Line
	1850 2700 3300 2700
Connection ~ 2250 2800
Wire Wire Line
	2250 2800 3300 2800
Connection ~ 1850 2700
Wire Wire Line
	2050 1900 2050 2100
Wire Wire Line
	1300 2700 1700 2700
Wire Wire Line
	1300 2800 1600 2800
Wire Wire Line
	1300 2050 1700 2050
Wire Wire Line
	1700 2050 1700 2700
Connection ~ 1700 2700
Wire Wire Line
	1700 2700 1850 2700
Wire Wire Line
	1300 2150 1600 2150
Wire Wire Line
	1600 2150 1600 2800
Connection ~ 1600 2800
Wire Wire Line
	1600 2800 2250 2800
Wire Wire Line
	1500 2350 1300 2350
Wire Wire Line
	1500 2900 1300 2900
Wire Wire Line
	1500 2900 1500 2350
Wire Wire Line
	1400 1900 1400 2250
Wire Wire Line
	1300 2250 1400 2250
Wire Wire Line
	1400 3000 1400 2250
Wire Wire Line
	1300 3000 1400 3000
Connection ~ 1400 2250
Wire Wire Line
	5500 2200 6550 2200
Connection ~ 6550 2200
Wire Wire Line
	6150 2100 5500 2100
Connection ~ 6150 2100
Wire Wire Line
	6450 3750 6450 3850
Wire Wire Line
	6450 3850 6550 3850
Connection ~ 6300 4250
Wire Wire Line
	5500 3750 6450 3750
Wire Wire Line
	5600 2600 5600 3850
Wire Wire Line
	5500 3850 5600 3850
Connection ~ 5600 3850
Wire Wire Line
	5600 3850 5600 4500
Wire Wire Line
	6100 3850 6100 4250
Wire Wire Line
	6100 4250 6300 4250
Wire Wire Line
	5600 3850 6100 3850
Wire Wire Line
	6000 4150 6000 4350
Wire Wire Line
	6000 4350 6550 4350
Wire Wire Line
	5500 4150 6000 4150
Wire Wire Line
	6000 3950 6000 3350
Wire Wire Line
	6000 3350 6550 3350
Wire Wire Line
	5500 3950 6000 3950
Wire Wire Line
	5900 4050 5900 3550
Wire Wire Line
	5900 3550 6550 3550
Wire Wire Line
	5500 4050 5900 4050
Wire Wire Line
	5800 4250 5800 3650
Wire Wire Line
	5800 3650 6550 3650
Wire Wire Line
	5500 4250 5800 4250
Wire Wire Line
	1150 4850 1500 4850
Wire Wire Line
	1500 4850 1500 4350
Connection ~ 1500 4350
Wire Wire Line
	1500 4350 3200 4350
Wire Wire Line
	1500 3100 1500 2900
Connection ~ 1500 2900
Text Label 5700 3300 0    50   ~ 0
TX8
Text Label 5700 3400 0    50   ~ 0
RX8
Wire Wire Line
	5700 3400 5500 3400
Wire Wire Line
	5700 3300 5500 3300
Text Notes 4100 650  0    74   ~ 0
Teensy 4.1
Text Notes 3750 5500 0    74   ~ 0
RTC Battery
$Comp
L Connector_Generic:Conn_01x02 J14
U 1 1 61762081
P 4100 5250
F 0 "J14" V 4100 5350 50  0000 L CNN
F 1 "J RTC Battery Socket" V 4200 4950 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x02_P2.54mm_Vertical" H 4100 5250 50  0001 C CNN
F 3 "~" H 4100 5250 50  0001 C CNN
	1    4100 5250
	0    -1   1    0   
$EndComp
Text Label 5700 2700 0    50   ~ 0
A17_RST
Text Label 5700 2800 0    50   ~ 0
A16_LDR
Wire Wire Line
	5700 2800 5500 2800
Wire Wire Line
	5700 2700 5500 2700
Text HLabel 1100 6800 0    50   Output ~ 0
RST
Text HLabel 1100 6900 0    50   Input ~ 0
DC
Text HLabel 1100 6700 0    50   Input ~ 0
LDR
Text HLabel 1100 7000 0    50   Output ~ 0
CS
Text HLabel 1100 7200 0    50   Output ~ 0
MOSI
Text HLabel 1100 7100 0    50   Output ~ 0
SCK
$Comp
L Connector_Generic:Conn_01x06 J?
U 1 1 62AF5A48
P 1550 6350
AR Path="/62569875/62AF5A48" Ref="J?"  Part="1" 
AR Path="/605467E9/62AF5A48" Ref="J21"  Part="1" 
F 0 "J21" V 1550 6750 50  0000 R CNN
F 1 "J Connector SPI+" V 1650 6600 50  0000 R CNN
F 2 "Connector_FFC-FPC:Molex_200528-0060_1x06-1MP_P1.00mm_Horizontal" H 1550 6350 50  0001 C CNN
F 3 "~" H 1550 6350 50  0001 C CNN
	1    1550 6350
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1350 6700 1350 6550
Wire Wire Line
	1350 6700 1100 6700
Wire Wire Line
	1450 6800 1450 6550
Wire Wire Line
	1550 6900 1550 6550
Wire Wire Line
	1650 7000 1650 6550
Wire Wire Line
	1750 7100 1750 6550
Wire Wire Line
	1850 7200 1850 6550
Wire Wire Line
	1100 6800 1450 6800
Wire Wire Line
	1100 6900 1550 6900
Wire Wire Line
	1100 7000 1650 7000
Wire Wire Line
	1100 7100 1750 7100
Wire Wire Line
	1100 7200 1850 7200
Text Label 3700 6700 0    50   ~ 0
A16_LDR
Text Label 3700 6800 0    50   ~ 0
A17_RST
Text Label 3700 6900 0    50   ~ 0
MISO0
Text Label 4550 6850 0    50   ~ 0
CS0
Text Label 3700 7100 0    50   ~ 0
SCK0
Text Label 3700 7200 0    50   ~ 0
MOSI0
Wire Wire Line
	1350 6700 3050 6700
Connection ~ 1350 6700
Connection ~ 1450 6800
Connection ~ 1550 6900
Connection ~ 1650 7000
Connection ~ 1750 7100
Connection ~ 1850 7200
$Comp
L Connector_Generic:Conn_01x09 J22
U 1 1 62EA45B6
P 2650 5950
F 0 "J22" V 2650 5450 50  0000 C CNN
F 1 "J SPI Display Breakout" V 2750 5950 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x09_P2.54mm_Vertical" H 2650 5950 50  0001 C CNN
F 3 "~" H 2650 5950 50  0001 C CNN
	1    2650 5950
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2250 6650 2100 6650
$Comp
L power:+3.3V #PWR058
U 1 1 62EBA5B6
P 2100 6250
F 0 "#PWR058" H 2100 6100 50  0001 C CNN
F 1 "+3.3V" H 2100 6400 50  0000 C CNN
F 2 "" H 2100 6250 50  0001 C CNN
F 3 "" H 2100 6250 50  0001 C CNN
	1    2100 6250
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR059
U 1 1 62EBB19A
P 2350 7300
F 0 "#PWR059" H 2350 7050 50  0001 C CNN
F 1 "GND" H 2350 7150 50  0000 C CNN
F 2 "" H 2350 7300 50  0001 C CNN
F 3 "" H 2350 7300 50  0001 C CNN
	1    2350 7300
	1    0    0    -1  
$EndComp
Wire Wire Line
	1450 6800 2950 6800
Wire Wire Line
	1550 6900 2850 6900
Wire Wire Line
	1650 7000 2750 7000
Wire Wire Line
	1750 7100 2650 7100
Wire Wire Line
	1850 7200 2550 7200
NoConn ~ 2450 6650
Wire Wire Line
	2100 6250 2100 6650
Wire Wire Line
	6100 1050 6100 850 
Wire Wire Line
	5500 1050 6100 1050
Wire Wire Line
	6000 1200 6000 850 
Wire Wire Line
	5500 1200 6000 1200
Text HLabel 6000 850  1    50   Output ~ 0
VUSB
Text HLabel 6100 850  1    50   Input ~ 0
VIN
Wire Wire Line
	4100 4650 4100 4850
Wire Wire Line
	4100 4850 3500 4850
Connection ~ 4100 4850
Wire Wire Line
	4100 4850 4100 5050
Wire Wire Line
	4200 4750 4200 4950
Text HLabel 3500 4850 0    50   Input ~ 0
VBAT
$Comp
L power:GND #PWR0102
U 1 1 608A0A17
P 3500 5050
F 0 "#PWR0102" H 3500 4800 50  0001 C CNN
F 1 "GND" H 3500 4900 50  0000 C CNN
F 2 "" H 3500 5050 50  0001 C CNN
F 3 "" H 3500 5050 50  0001 C CNN
	1    3500 5050
	-1   0    0    -1  
$EndComp
Wire Wire Line
	3500 5050 3500 4950
Wire Wire Line
	3500 4950 4200 4950
Connection ~ 4200 4950
Wire Wire Line
	4200 4950 4200 5050
Wire Wire Line
	2250 6150 2250 6650
Wire Wire Line
	2350 6150 2350 7300
Wire Wire Line
	2450 6150 2450 6650
Wire Wire Line
	2550 6150 2550 7200
Wire Wire Line
	2650 6150 2650 7100
Wire Wire Line
	2850 6150 2850 6900
Wire Wire Line
	2950 6150 2950 6800
Wire Wire Line
	3050 6150 3050 6700
$Comp
L Homekit_Faceplate_v1.1-rescue:SolderJumper_3_Bridged12-Jumper SJ7
U 1 1 60A83E1D
P 4250 7000
F 0 "SJ7" H 4250 6800 50  0000 C CNN
F 1 "SJ SPI CS Select" H 4300 7100 50  0000 C CNN
F 2 "Jumper:SolderJumper-3_P1.3mm_Bridged12_Pad1.0x1.5mm" H 4250 7000 50  0001 C CNN
F 3 "" H 4250 7000 50  0001 C CNN
	1    4250 7000
	1    0    0    1   
$EndComp
Text Label 4550 7000 0    50   ~ 0
CS1
Wire Wire Line
	3050 6700 3700 6700
Connection ~ 3050 6700
Wire Wire Line
	2950 6800 3700 6800
Connection ~ 2950 6800
Wire Wire Line
	2850 6900 3700 6900
Connection ~ 2850 6900
Connection ~ 2750 7000
Wire Wire Line
	2650 7100 3700 7100
Connection ~ 2650 7100
Wire Wire Line
	2550 7200 3700 7200
Connection ~ 2550 7200
Text Label 3100 3400 2    50   ~ 0
CS1
Wire Wire Line
	3100 3400 3300 3400
Text Label 10200 1350 1    50   ~ 0
PUSH_T41_PROG
$Comp
L teensy:Teensy4.1 U2
U 1 1 605A3F1A
P 4400 3350
F 0 "U2" H 4400 5915 50  0000 C CNN
F 1 "Teensy4.1" H 4400 5824 50  0000 C CNN
F 2 "teensy:Teensy41" H 4000 3750 50  0001 C CNN
F 3 "" H 4000 3750 50  0001 C CNN
	1    4400 3350
	1    0    0    -1  
$EndComp
Text Label 3100 3300 2    50   ~ 0
PUSH_RESERVED
Wire Wire Line
	3100 3300 3300 3300
Text Label 10400 1350 1    50   ~ 0
PUSH_RESERVED
$Comp
L Homekit_Faceplate_v1.1-rescue:SolderJumper_3_Bridged12-Jumper SJ8
U 1 1 60D67CD0
P 10200 1550
F 0 "SJ8" H 10100 1450 50  0000 C CNN
F 1 "SJ PUSH RES" V 10500 1100 50  0000 C CNN
F 2 "Jumper:SolderJumper-3_P1.3mm_Bridged12_Pad1.0x1.5mm" H 10200 1550 50  0001 C CNN
F 3 "" H 10200 1550 50  0001 C CNN
	1    10200 1550
	1    0    0    1   
$EndComp
Wire Wire Line
	10000 1550 10000 2650
Wire Wire Line
	10200 1400 10200 1350
Wire Wire Line
	10400 1550 10400 1350
Wire Wire Line
	6950 1650 7150 1650
Wire Wire Line
	7150 1650 7150 3750
$Comp
L Device:R R?
U 1 1 60E4A791
P 1650 850
AR Path="/62569875/60E4A791" Ref="R?"  Part="1" 
AR Path="/605467E9/60E4A791" Ref="R29"  Part="1" 
F 0 "R29" V 1600 650 50  0000 C CNN
F 1 "220R" V 1650 850 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 1580 850 50  0001 C CNN
F 3 "~" H 1650 850 50  0001 C CNN
	1    1650 850 
	0    1    1    0   
$EndComp
$Comp
L Device:LED D?
U 1 1 60E4A797
P 2050 850
AR Path="/62569875/60E4A797" Ref="D?"  Part="1" 
AR Path="/605467E9/60E4A797" Ref="D15"  Part="1" 
F 0 "D15" H 2150 800 50  0000 C CNN
F 1 "LED RED" H 2000 950 28  0000 C CNN
F 2 "LED_SMD:LED_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 2050 850 50  0001 C CNN
F 3 "~" H 2050 850 50  0001 C CNN
	1    2050 850 
	-1   0    0    1   
$EndComp
Text Label 1300 850  2    50   ~ 0
LED_PROG
$Comp
L power:GND #PWR0103
U 1 1 60F0200F
P 2300 950
F 0 "#PWR0103" H 2300 700 50  0001 C CNN
F 1 "GND" H 2300 800 50  0000 C CNN
F 2 "" H 2300 950 50  0001 C CNN
F 3 "" H 2300 950 50  0001 C CNN
	1    2300 950 
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2300 950  2300 850 
Wire Wire Line
	2300 850  2200 850 
Wire Wire Line
	1900 850  1800 850 
Wire Wire Line
	1500 850  1300 850 
Text Notes 800  700  3    74   ~ 0
Status LEDs
Wire Wire Line
	2750 6150 2750 7000
Wire Wire Line
	4550 6850 4250 6850
Wire Wire Line
	4550 7000 4450 7000
Wire Wire Line
	2750 7000 4050 7000
Wire Wire Line
	9850 5550 10150 5550
Wire Wire Line
	9850 5850 10700 5850
Wire Wire Line
	9850 5950 10450 5950
NoConn ~ 10450 6050
NoConn ~ 9400 6350
$EndSCHEMATC