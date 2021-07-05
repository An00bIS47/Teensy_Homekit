//
// HAPAccessory.cpp
// Homekit
//
//  Created on: 18.08.2017
//      Author: michael
//

#include "HAPAccessorySet.hpp"
#include "HAPHelper.hpp"
#include "HAPLogging.hpp"
#include "HAPGlobals.hpp"
#include "HAPServer.hpp"

#include <mbedtls/sha512.h>
#if defined ( ARDUINO_ARCH_ESP32)
#include <mbedtls/base64.h>
#elif defined (CORE_TEENSY)
#include <Base64.h>
#endif

#include <mbedtls/bignum.h>

#include "base36.h"

#include "HAPDeviceID.hpp"

uint32_t HAPAccessorySet::configurationNumber = HOMEKIT_CONFIGURATION_NUMBER;

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
HAPAccessorySet::HAPAccessorySet()
: _accessoryType(HAP_ACCESSORY_TYPE_OTHER)
{
	// memset(_xhm, 0, 21);
	_configuration = nullptr;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
HAPAccessorySet::~HAPAccessorySet(){

}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPAccessorySet::begin(){
	addAccessoryInfo();
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPAccessorySet::addAccessoryInfo(){
	HAPAccessory *accessory = new HAPAccessory();
	accessory->addInfoService(modelName(), HAP_MANUFACTURER, HAP_MODELL_NAME, "44-22-777", NULL, hap.versionString());

	addAccessory(accessory);
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
uint8_t HAPAccessorySet::accessoryType(){
	return _accessoryType;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPAccessorySet::setAccessoryType(enum HAP_ACCESSORY_TYPE accessoryType){
	_accessoryType = (uint8_t)accessoryType;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
const char* HAPAccessorySet::setupID(){
	return _configuration->setupId;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPAccessorySet::generateSetupID(){
	char setupID[5];

#if HAP_DEBUG
	strcpy(setupID, HAP_SETUP_ID);
#else
	const char* letters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	randomSeed(analogRead(0));

	int i = 0;
  	while(i < 4) {
    	setupID[i] = ( randomChar((char*)letters) );
    	i++;
  	}
  	setupID[4] = '\0';

#endif

	_configuration->setSetupId(setupID);
  	computeSetupHash();

  	generateXMI();
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
char HAPAccessorySet::randomChar(char* letters) {
   return letters[random(0, strlen(letters)-1)];
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPAccessorySet::setModelName(const char* name){
	_configuration->setModelName(name);
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
const char* HAPAccessorySet::modelName(){
	return _configuration->modelname;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
const char* HAPAccessorySet::setupHash(){
	return _setupHash;
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPAccessorySet::computeSetupHash(){

	uint8_t *baseMac = HAPDeviceID::generateID();

	char ci[18];
	sprintf(ci, "%02X:%02X:%02X:%02X:%02X:%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);

	uint8_t setupHashMaterial[(4 + 17)];

	memcpy(setupHashMaterial, _configuration->setupId, 4);
	memcpy(setupHashMaterial + 4, ci, 17);

	uint8_t output[64];
	mbedtls_sha512(setupHashMaterial, 21, output, 0);

	uint8_t sliced[4];
	memcpy(sliced, output, 4);

	// Doesn't work :(
	//_setupHash = base64::encode(sliced);


#if defined ( ARDUINO_ARCH_ESP32)
	size_t olen;
	if (mbedtls_base64_encode((uint8_t*)_setupHash, 9, &olen, sliced, 4)) {
		LogE("[computeSetupHash] ERROR: mbedtls_base64_encode failed!", true);
	}
#elif defined (CORE_TEENSY)

	base64_encode(_setupHash, (char*)sliced, 4);
#endif

	//setupHash[9] = '\0';
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPAccessorySet::setPinCode(const char* pinCode){
	_configuration->setPincode(pinCode);
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
const char* HAPAccessorySet::pinCode(){
	return _configuration->pincode;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPAccessorySet::generateXMI(){

	// String tmp1 = _configuration->pincode;
	// tmp1.replace("-", "");
	// int lowValue = atoi(tmp1.c_str());

	// XXX-XX-XXX
	// X-HM://0023ISZCGUPFT
	char tmp[9] = {'\0',};
	memcpy(tmp, _configuration->pincode, 3);
	memcpy(tmp + 3, _configuration->pincode + 4, 2);
	memcpy(tmp + 5, _configuration->pincode + 7, 3);
	// int lowValue = atoi(tmp);
	uint32_t lowValue = atol(tmp);

	// LOG_E("tmp1: %s\n", tmp1.c_str());
	// LOG_E("tmp: %s\n", tmp);

	lowValue |= 1 << 28;

	mbedtls_mpi bignumLow;
	mbedtls_mpi bignumHigh;

	mbedtls_mpi_init(&bignumLow);
	mbedtls_mpi_init(&bignumHigh);


	int error_code = mbedtls_mpi_lset(&bignumLow, lowValue);
	if (error_code != 0){
		LOG_E("ERROR: Failed to mbedtls_mpi_lset\n");
	}


	size_t olenLow = 0;
	mbedtls_mpi_write_string(&bignumLow, 16, NULL, 0, &olenLow);

	char dstLow[olenLow];
	int dstLowLen = olenLow;
	error_code = mbedtls_mpi_write_string(&bignumLow, 16, dstLow, dstLowLen, &olenLow);
	if (error_code != 0){
		LOG_E("ERROR: Failed to mbedtls_mpi_write_string\n");
	}


	int valueHigh = _accessoryType >> 1;

	char dst2[olenLow + 1];
	sprintf(dst2, "%d%s", valueHigh, dstLow);

	error_code = mbedtls_mpi_read_string(&bignumHigh, 16, dst2);
	if (error_code != 0){
		LOG_E("ERROR: Failed to mbedtls_mpi_read_string\n");
	}


	size_t olen2 = 0;
	mbedtls_mpi_write_string(&bignumHigh, 16, NULL, 0, &olen2);

	char dst3[olen2];
	int dstlen3 = olen2;
	error_code = mbedtls_mpi_write_string(&bignumHigh, 16, dst3, dstlen3, &olen2);
	if (error_code != 0){
		LOG_E("ERROR: Failed to mbedtls_mpi_write_string\n");
	}

	char dest[9];
	str16_to_str36(dest, dst3);

	char finalDest[9];
	HAPHelper::prependZeros(finalDest, dest, 9);

	memcpy(_xhm, "X-HM://", 7);
	memcpy(_xhm + 7, finalDest, 9);
	memcpy(_xhm + 7 + 9, _configuration->setupId, 4);
	_xhm[20] = '\0';

	mbedtls_mpi_free(&bignumLow);
	mbedtls_mpi_free(&bignumHigh);
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
const char* HAPAccessorySet::xhm(){
	return _xhm;
}



uint8_t HAPAccessorySet::numberOfAccessories() {
	return _accessories.size();
}


HAPAccessory* HAPAccessorySet::accessoryWithAID(uint8_t aid) {
	for (uint8_t i=0; i < _accessories.size(); i++){
		if (_accessories[i]->aid() == aid){
			return _accessories.at(i).get();
		}
	}
	return nullptr;
}


HAPAccessory* HAPAccessorySet::accessoryAtIndex(uint8_t index){
	return _accessories.at(index).get();
}


void HAPAccessorySet::addAccessory(HAPAccessory *accessory) {
	accessory->setAID(++_aid);
	_accessories.emplace_back(std::move(accessory));
}


bool HAPAccessorySet::removeAccessory(HAPAccessory *accessory) {
	for (int i=0; i < _accessories.size(); i++){
		if (_accessories[i]->aid() == accessory->aid()){
			_accessories.erase(_accessories.begin() + i);
			return true;
		}
	}
	return false;
}

HAPCharacteristicBase* HAPAccessorySet::getCharacteristic(uint8_t aid, uint32_t iid){
	HAPAccessory* accessory = accessoryWithAID(aid);

	if (accessory == nullptr) {
		LOG_E("ERROR: Accessory with aid: %d not found - Error Code %d\n", aid, HAP_STATUS_RESOURCE_NOT_FOUND);
		//error_code = HAP_STATUS_RESOURCE_NOT_FOUND;
		//errorOccured = true;
		return nullptr;
	} else {

		HAPCharacteristicBase* chr = accessory->characteristicWithIID(iid);

		if (chr == nullptr) {
			LOG_E("ERROR: Characteristics %d.%d not found - Error Code %d\n", aid, iid, HAP_STATUS_RESOURCE_NOT_FOUND);
			return nullptr;
		} else {
			return chr;
		}

	}

	return nullptr;
}


void HAPAccessorySet::printTo(Print& print){
	print.print(F("{\"accessories\":["));
	for (int i = 0; i < numberOfAccessories(); i++) {
        _accessories[i]->printTo(print);

		if (i+1 < numberOfAccessories()){
			print.print(F(","));
		}
    }
	print.print(F("]}"));
}

HAPCharacteristicBase* HAPAccessorySet::getCharacteristicOfType(uint8_t aid, uint8_t type){
	HAPAccessory* accessory = accessoryWithAID(aid);
	if (accessory){
		return accessory->characteristicsOfType(type);
	}
	return nullptr;
}

HAPCharacteristicBase* HAPAccessorySet::getCharacteristicOfType(uint8_t aid, const char* type){
	HAPAccessory* accessory = accessoryWithAID(aid);
	if (accessory){
		return accessory->characteristicsOfType(type);
	}
	return nullptr;
}