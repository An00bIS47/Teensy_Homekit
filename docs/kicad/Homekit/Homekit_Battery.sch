EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 5 5
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
L Homekit_Library:MCP73832 U6
U 1 1 60578978
P 2300 1850
F 0 "U6" H 2675 2415 50  0000 C CNN
F 1 "MCP73832" H 2675 2324 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:TSOT-23-5_HandSoldering" H 2650 2450 50  0001 C CNN
F 3 "https://ww1.microchip.com/downloads/en/DeviceDoc/MCP73831-Family-Data-Sheet-DS20001984H.pdf" H 2650 2450 50  0001 C CNN
	1    2300 1850
	1    0    0    -1  
$EndComp
Text HLabel 850  1550 0    50   Input ~ 0
VUSB
$Comp
L Device:C C25
U 1 1 60579EDF
P 1050 1950
F 0 "C25" V 900 1850 50  0000 L CNN
F 1 "4.7µF / 6.3 V" V 1150 1650 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 1088 1800 50  0001 C CNN
F 3 "~" H 1050 1950 50  0001 C CNN
	1    1050 1950
	1    0    0    -1  
$EndComp
$Comp
L Device:R R24
U 1 1 6057B54F
P 1950 2100
F 0 "R24" V 2050 2000 50  0000 L CNN
F 1 "470R" V 1950 2000 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 1880 2100 50  0001 C CNN
F 3 "~" H 1950 2100 50  0001 C CNN
	1    1950 2100
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR039
U 1 1 6057D033
P 1050 2250
F 0 "#PWR039" H 1050 2000 50  0001 C CNN
F 1 "GND" H 1055 2077 50  0000 C CNN
F 2 "" H 1050 2250 50  0001 C CNN
F 3 "" H 1050 2250 50  0001 C CNN
	1    1050 2250
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR040
U 1 1 6057D3E8
P 3150 2250
F 0 "#PWR040" H 3150 2000 50  0001 C CNN
F 1 "GND" H 3155 2077 50  0000 C CNN
F 2 "" H 3150 2250 50  0001 C CNN
F 3 "" H 3150 2250 50  0001 C CNN
	1    3150 2250
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR062
U 1 1 6057DE38
P 3800 2300
F 0 "#PWR062" H 3800 2050 50  0001 C CNN
F 1 "GND" H 3805 2127 50  0000 C CNN
F 2 "" H 3800 2300 50  0001 C CNN
F 3 "" H 3800 2300 50  0001 C CNN
	1    3800 2300
	1    0    0    -1  
$EndComp
$Comp
L Device:R R25
U 1 1 6057D935
P 3550 1700
F 0 "R25" V 3650 1600 50  0000 L CNN
F 1 "10k" V 3550 1600 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 3480 1700 50  0001 C CNN
F 3 "~" H 3550 1700 50  0001 C CNN
	1    3550 1700
	0    -1   -1   0   
$EndComp
Wire Wire Line
	850  1550 1050 1550
Wire Wire Line
	1050 1550 1050 1800
Connection ~ 1050 1550
Wire Wire Line
	1900 1550 2300 1550
Wire Wire Line
	1050 2250 1050 2100
$Comp
L Device:C C26
U 1 1 60581A1B
P 4050 1950
F 0 "C26" V 4165 1996 50  0000 L CNN
F 1 "4.7µF / 6.3 V" V 4250 1650 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 4088 1800 50  0001 C CNN
F 3 "~" H 4050 1950 50  0001 C CNN
	1    4050 1950
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR063
U 1 1 60582174
P 4050 2300
F 0 "#PWR063" H 4050 2050 50  0001 C CNN
F 1 "GND" H 4055 2127 50  0000 C CNN
F 2 "" H 4050 2300 50  0001 C CNN
F 3 "" H 4050 2300 50  0001 C CNN
	1    4050 2300
	1    0    0    -1  
$EndComp
Wire Wire Line
	4050 1550 4050 1800
$Comp
L Device:Battery J13
U 1 1 605830E1
P 4450 1950
F 0 "J13" V 4550 1900 50  0000 L CNN
F 1 "JST Battery" V 4650 1750 50  0000 L CNN
F 2 "Connector_JST:JST_PH_B2B-PH-K_1x02_P2.00mm_Vertical" V 4450 2010 50  0001 C CNN
F 3 "~" V 4450 2010 50  0001 C CNN
	1    4450 1950
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR064
U 1 1 60584022
P 4450 2300
F 0 "#PWR064" H 4450 2050 50  0001 C CNN
F 1 "GND" H 4455 2127 50  0000 C CNN
F 2 "" H 4450 2300 50  0001 C CNN
F 3 "" H 4450 2300 50  0001 C CNN
	1    4450 2300
	1    0    0    -1  
$EndComp
Wire Wire Line
	4450 1750 4450 1550
$Comp
L Device:Q_PMOS_GSD Q1
U 1 1 6058AE2E
P 4800 1450
F 0 "Q1" V 4700 1500 50  0000 C CNN
F 1 "DMP1045U" V 4600 1650 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23_Handsoldering" H 5000 1550 50  0001 C CNN
F 3 "~" H 4800 1450 50  0001 C CNN
	1    4800 1450
	0    -1   1    0   
$EndComp
$Comp
L Device:C C27
U 1 1 6058CA24
P 5100 1900
F 0 "C27" V 5215 1946 50  0000 L CNN
F 1 "1µF / 6.3 V" V 5300 1600 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 5138 1750 50  0001 C CNN
F 3 "~" H 5100 1900 50  0001 C CNN
	1    5100 1900
	1    0    0    -1  
$EndComp
Wire Wire Line
	1900 1550 1900 1050
Wire Wire Line
	4800 1250 4800 1050
$Comp
L Diode:B130-E3 D13
U 1 1 6058E998
P 5100 1300
F 0 "D13" V 5146 1220 50  0000 R CNN
F 1 "B130-E3" V 5055 1220 50  0000 R CNN
F 2 "Diode_SMD:D_SMA" H 5100 1125 50  0001 C CNN
F 3 "http://www.vishay.com/docs/88946/b120.pdf" H 5100 1300 50  0001 C CNN
	1    5100 1300
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5100 1150 5100 1050
Wire Wire Line
	5750 1050 5750 1150
$Comp
L power:GND #PWR066
U 1 1 605908D3
P 5750 2300
F 0 "#PWR066" H 5750 2050 50  0001 C CNN
F 1 "GND" H 5755 2127 50  0000 C CNN
F 2 "" H 5750 2300 50  0001 C CNN
F 3 "" H 5750 2300 50  0001 C CNN
	1    5750 2300
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR065
U 1 1 60590C5A
P 5100 2300
F 0 "#PWR065" H 5100 2050 50  0001 C CNN
F 1 "GND" H 5105 2127 50  0000 C CNN
F 2 "" H 5100 2300 50  0001 C CNN
F 3 "" H 5100 2300 50  0001 C CNN
	1    5100 2300
	1    0    0    -1  
$EndComp
$Comp
L Device:R R26
U 1 1 605916A0
P 5750 1300
F 0 "R26" H 5820 1346 50  0000 L CNN
F 1 "100k" V 5750 1200 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 5680 1300 50  0001 C CNN
F 3 "~" H 5750 1300 50  0001 C CNN
	1    5750 1300
	1    0    0    -1  
$EndComp
Wire Wire Line
	5750 1450 5750 2300
Text HLabel 5350 1550 2    50   Output ~ 0
VIN
$Comp
L dk_Battery-Holders-Clips-Contacts:BS-7 BAT1
U 1 1 60718DB8
P 1950 3350
F 0 "BAT1" H 2078 3403 60  0000 L CNN
F 1 "RTC Battery" H 2078 3297 60  0000 L CNN
F 2 "digikey-footprints:Battery_Holder_Coin_2032_BS-7" H 2150 3550 60  0001 L CNN
F 3 "http://www.memoryprotectiondevices.com/datasheets/BS-7-datasheet.pdf" V 2150 3650 60  0001 L CNN
F 4 "BS-7-ND" H 2150 3750 60  0001 L CNN "Digi-Key_PN"
F 5 "BS-7" H 2150 3850 60  0001 L CNN "MPN"
F 6 "Battery Products" H 2150 3950 60  0001 L CNN "Category"
F 7 "Battery Holders, Clips, Contacts" H 2150 4050 60  0001 L CNN "Family"
F 8 "http://www.memoryprotectiondevices.com/datasheets/BS-7-datasheet.pdf" H 2150 4150 60  0001 L CNN "DK_Datasheet_Link"
F 9 "/product-detail/en/mpd-memory-protection-devices/BS-7/BS-7-ND/389447" H 2150 4250 60  0001 L CNN "DK_Detail_Page"
F 10 "BATTERY HOLDER COIN 20MM PC PIN" H 2150 4350 60  0001 L CNN "Description"
F 11 "MPD (Memory Protection Devices)" H 2150 4450 60  0001 L CNN "Manufacturer"
F 12 "Active" H 2150 4550 60  0001 L CNN "Status"
	1    1950 3350
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J12
U 1 1 6071BC78
P 1150 3400
F 0 "J12" H 1150 3200 50  0000 C CNN
F 1 "J RTC BatteryPack" V 1250 3450 50  0000 C CNN
F 2 "Connector_JST:JST_PH_B2B-PH-K_1x02_P2.00mm_Vertical" H 1150 3400 50  0001 C CNN
F 3 "~" H 1150 3400 50  0001 C CNN
	1    1150 3400
	-1   0    0    1   
$EndComp
Wire Wire Line
	1950 3550 1950 3700
Wire Wire Line
	1950 3700 1550 3700
Wire Wire Line
	1550 3700 1550 3400
Wire Wire Line
	1550 3400 1450 3400
Wire Wire Line
	1350 3300 1400 3300
Wire Wire Line
	1550 3300 1550 3000
Wire Wire Line
	1550 3000 1950 3000
Wire Wire Line
	1950 3000 1950 3150
Wire Wire Line
	1550 3000 1250 3000
Connection ~ 1550 3000
Text HLabel 1250 3000 0    50   Output ~ 0
VBAT
Text Notes 1000 900  0    50   ~ 0
USB Charging with load sharing\nhttps://blog.zakkemble.net/wp-content/uploads/2013/05/MCP73831_loadshare.png\n
Text Notes 850  2900 0    50   ~ 0
RTC Battery
$Comp
L Connector_Generic:Conn_01x02 J23
U 1 1 60756BF5
P 1150 4150
F 0 "J23" H 1150 3950 50  0000 C CNN
F 1 "J RTC Breakout" V 1250 4200 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 1150 4150 50  0001 C CNN
F 3 "~" H 1150 4150 50  0001 C CNN
	1    1150 4150
	-1   0    0    1   
$EndComp
Wire Wire Line
	1350 4050 1400 4050
Wire Wire Line
	1400 4050 1400 3300
Connection ~ 1400 3300
Wire Wire Line
	1400 3300 1550 3300
Wire Wire Line
	1350 4150 1450 4150
Wire Wire Line
	1450 4150 1450 3400
Connection ~ 1450 3400
Wire Wire Line
	1450 3400 1350 3400
$Comp
L Device:R R27
U 1 1 607C8EE1
P 1950 1850
F 0 "R27" V 2050 1750 50  0000 L CNN
F 1 "470R" V 1950 1750 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 1880 1850 50  0001 C CNN
F 3 "~" H 1950 1850 50  0001 C CNN
	1    1950 1850
	0    1    1    0   
$EndComp
$Comp
L Device:LED D12
U 1 1 607CCAA7
P 1550 1850
F 0 "D12" H 1550 1750 50  0000 C CNN
F 1 "LED Red" H 1350 1700 50  0000 C CNN
F 2 "LED_SMD:LED_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 1550 1850 50  0001 C CNN
F 3 "~" H 1550 1850 50  0001 C CNN
	1    1550 1850
	-1   0    0    1   
$EndComp
$Comp
L Device:LED D14
U 1 1 607CF7CA
P 1550 2100
F 0 "D14" H 1550 2200 50  0000 C CNN
F 1 "LED Green" H 1600 1950 50  0000 C CNN
F 2 "LED_SMD:LED_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 1550 2100 50  0001 C CNN
F 3 "~" H 1550 2100 50  0001 C CNN
	1    1550 2100
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0101
U 1 1 607D07DB
P 1300 2250
F 0 "#PWR0101" H 1300 2000 50  0001 C CNN
F 1 "GND" H 1305 2077 50  0000 C CNN
F 2 "" H 1300 2250 50  0001 C CNN
F 3 "" H 1300 2250 50  0001 C CNN
	1    1300 2250
	1    0    0    -1  
$EndComp
Wire Wire Line
	1300 2250 1300 2100
Wire Wire Line
	1300 2100 1400 2100
Wire Wire Line
	1400 1850 1300 1850
Wire Wire Line
	1700 1850 1800 1850
Wire Wire Line
	1700 2100 1800 2100
Wire Wire Line
	2100 1850 2200 1850
Wire Wire Line
	2100 2100 2200 2100
Wire Wire Line
	2200 2100 2200 1850
Connection ~ 2200 1850
Wire Wire Line
	2200 1850 2300 1850
Wire Wire Line
	3150 2250 3150 1850
Wire Wire Line
	3150 1850 3050 1850
Wire Wire Line
	1300 1850 1300 1550
Wire Wire Line
	1050 1550 1300 1550
Wire Wire Line
	1300 1550 1900 1550
Connection ~ 1300 1550
Connection ~ 1900 1550
Wire Wire Line
	3050 1700 3300 1700
$Comp
L Device:R R28
U 1 1 607EFBAD
P 3550 2200
F 0 "R28" V 3650 2100 50  0000 L CNN
F 1 "2k5" V 3550 2100 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 3480 2200 50  0001 C CNN
F 3 "~" H 3550 2200 50  0001 C CNN
	1    3550 2200
	0    -1   -1   0   
$EndComp
$Comp
L Jumper:SolderJumper_2_Open SJ6
U 1 1 607F18FB
P 3300 1950
F 0 "SJ6" V 3254 2018 50  0000 L CNN
F 1 "SolderJumper_2_Open" V 3345 2018 50  0000 L CNN
F 2 "Jumper:SolderJumper-2_P1.3mm_Open_Pad1.0x1.5mm" H 3300 1950 50  0001 C CNN
F 3 "~" H 3300 1950 50  0001 C CNN
	1    3300 1950
	0    1    1    0   
$EndComp
Wire Wire Line
	3300 2200 3300 2100
Wire Wire Line
	3300 1800 3300 1700
Wire Wire Line
	3800 2200 3800 2300
Wire Wire Line
	3700 1700 3800 1700
Wire Wire Line
	3700 2200 3800 2200
Wire Wire Line
	3800 2200 3800 1700
Connection ~ 3800 2200
Wire Wire Line
	3400 1700 3300 1700
Connection ~ 3300 1700
Wire Wire Line
	3300 2200 3400 2200
Wire Wire Line
	3050 1550 4050 1550
Wire Wire Line
	4050 2100 4050 2300
Wire Wire Line
	4450 2150 4450 2300
Wire Wire Line
	5100 2050 5100 2300
Wire Wire Line
	4800 1050 5100 1050
Connection ~ 5100 1050
Wire Wire Line
	5100 1050 5750 1050
Wire Wire Line
	5000 1550 5100 1550
Wire Wire Line
	5100 1450 5100 1550
Connection ~ 5100 1550
Wire Wire Line
	5100 1550 5350 1550
Wire Wire Line
	5100 1550 5100 1750
Wire Wire Line
	4600 1550 4450 1550
Connection ~ 4050 1550
Connection ~ 4450 1550
Wire Wire Line
	4450 1550 4050 1550
Wire Wire Line
	1900 1050 4800 1050
Connection ~ 4800 1050
$EndSCHEMATC
