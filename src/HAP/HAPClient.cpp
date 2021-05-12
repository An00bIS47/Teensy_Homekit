//
// HAPClient.cpp
// Homekit
//
//  Created on: 12.08.2017
//      Author: michael
//

#include "HAPClient.hpp"
#include "HAPHelper.hpp"
#include "HAPLogger.hpp"


HAPClient::HAPClient()
: state(HAP_CLIENT_STATE_DISCONNECTED)
, pairState(HAP_PAIR_STATE_RESERVED)
, verifyState(HAP_VERIFY_STATE_RESERVED)
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

void HAPClient::subscribe(int aid, int iid, bool value){
	struct HAPSubscribtionItem item = HAPSubscribtionItem(aid, iid);

	if (value){
		subscribtions.insert(item);
	} else {
		subscribtions.erase(item);
	}

}

bool HAPClient::isSubscribed(int aid, int iid) const {
	struct HAPSubscribtionItem item = HAPSubscribtionItem(aid, iid);
	return subscribtions.find(item) != subscribtions.end();
}

#if HAP_API_ADMIN_MODE
// ToDo: Replace with streaming method
String HAPClient::describe() const {

	String keys[4];
    String values[4];
    int i=0;
    {
        keys[i] = "isEncrypted";
        values[i++] = String(_isEncrypted);
    }
	{
        keys[i] = "state";
        values[i++] = String(state);
    }

	{
        keys[i] = "ip";
        values[i++] = HAPHelper::wrap(client.remoteIP().toString());
    }

	{
        //Form subscribtions list
        int j=0;
		int noOfSubscribtions = subscribtions.size();
        String *subs = new String[noOfSubscribtions];
		for (auto &sub : subscribtions ) {
			subs[j++] = sub.describe();
		}
        keys[i] = "subscribtions";
        values[i++] = HAPHelper::arrayWrap(subs, noOfSubscribtions);
        delete [] subs;
    }

    return HAPHelper::dictionaryWrap(keys, values, i);
}


#endif

