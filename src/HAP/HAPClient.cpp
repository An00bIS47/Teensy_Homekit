//
// HAPClient.cpp
// Homekit
//
//  Created on: 12.08.2017
//      Author: michael
//

#include "HAPClient.hpp"
#include "HAPHelper.hpp"
#include "HAPLogging.hpp"


HAPClient::HAPClient()
: state(HAPClientState::Disconnected)
, pairState(HAPPairingState::Reserved)
, verifyState(HAPVerifyState::Reserved)
, _isEncrypted(false)
, _isAdmin(false)

{
	memset(_idPtr,0,HAP_PAIRINGS_ID_LENGTH);
}

HAPClient::~HAPClient() {


	subscribtions.clear();
	clear();


}

bool HAPClient::operator==(const HAPClient &hap) const {
#if defined( ARDUINO_ARCH_ESP32 )
	return hap.client.fd() == client.fd();
#elif defined( CORE_TEENSY )
	return hap.client.getSocketNumber() == client.getSocketNumber();
#endif
}

void HAPClient::clear() {
	request.clear();
}

void HAPClient::subscribe(uint8_t aid, uint32_t iid, bool value){
	struct HAPSubscribtionItem item = HAPSubscribtionItem(aid, iid);

	if (value){
		subscribtions.insert(item);
	} else {
		subscribtions.erase(item);
	}

}

bool HAPClient::isSubscribed(uint8_t aid, uint32_t iid) const {
	struct HAPSubscribtionItem item = HAPSubscribtionItem(aid, iid);
	return subscribtions.find(item) != subscribtions.end();
}

