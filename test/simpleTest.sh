#!/bin/bash

function characteristics(){	
	python3 -m homekit.get_characteristic -f ${PAIRINGDATAFILE} -a ${ALIAS} -c ${CHARACTERISTICS} -c 2.9 
}

function characteristic(){	
	python3 -m homekit.get_characteristic -f ${PAIRINGDATAFILE} -a ${ALIAS} -c ${CHARACTERISTICS} -m -p -e -t 	
}


function accessory(){	   	
   	python3 -m homekit.get_accessories -f ${PAIRINGDATAFILE} -a ${ALIAS} -o json | python3 ~/Development/Homekit/utils/accessory_validate/accval.py
}


function identify(){
	echo "Call paired /identify a $ITERATIONS times"
	python3 -m homekit.identify -f ${PAIRINGDATAFILE} -a ${ALIAS}		
}

function pair() {
	echo "Pair a device"
	echo "==========================================="
	python3 -m homekit.pair -d ${DEVICEID} -p ${SETUPCODE} -f ${PAIRINGDATAFILE} -a ${ALIAS}
	echo "==========================================="
}

function identifyUnpaired() {
	echo "Call unpaired /identify a $ITERATIONS times"
	echo "==========================================="
	for i in $(seq 1 $ITERATIONS);
	do
	   	echo "Loading $i times"
		python3 -m homekit.identify -d ${DEVICEID}
		
	done
	echo "==========================================="
}

function init() {
	echo "Init homekit storage file" 
	echo "==========================================="
	python3 -m homekit.init_controller_storage -f ${PAIRINGDATAFILE}
	echo "==========================================="
}


function printSep(){
	echo ""
 	echo ""
 	echo ""
}


function main () {
 echo running ...
 init
 printSep

 #identifyUnpaired
 #printSep
 
 time pair
 printSep

 identify
 printSep
 
 ITERATIONS=50
 echo "Call /accessory a $ITERATIONS times"
 echo "====================================="
 for i in $(seq 1 $ITERATIONS);
	do
		echo "   - $i / $ITERATIONS"
		time accessory
		printSep
	done

 
 echo "Get /characteristic a $ITERATIONS times"
 echo "====================================="
 for i in $(seq 1 $ITERATIONS);
	do
		echo "   - $i / $ITERATIONS"
		time characteristic
		printSep
	done
 
 echo "Get /characteristics a $ITERATIONS times"
 echo "====================================="
 for i in $(seq 1 $ITERATIONS);
	do
		echo "   - $i / $ITERATIONS"
		time characteristics
		printSep
	done

 
 printSep

}

PAIRINGDATAFILE="./homekitStorage.json"

DEVICEID="98:F4:AB:13:99:4C"
ALIAS="knx"

ALIAS_ADD_PAIRING="knx_user"

SETUPCODE="031-45-712"
CHARACTERISTICS="2.10"

ITERATIONS=1

main