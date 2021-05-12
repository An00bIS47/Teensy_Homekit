//
// HAPClient.hpp
// Homekit
//
//  Created on: 12.08.2017
//      Author: michael
//

#ifndef HAPCLIENT_HPP_
#define HAPCLIENT_HPP_

#include <Arduino.h>

#if defined( ARDUINO_ARCH_ESP32 )
#include <WiFiClient.h>
#elif defined( CORE_TEENSY )
#include <NativeEthernet.h>
#endif

// #include <StreamUtils.h>

#include <vector>
#include <set>

#include "HAPGlobals.hpp"
#include "HAPRequest.hpp"
#include "HAPVerifyContext.hpp"
#include "HAPTLV8Types.hpp"

#if HAP_API_ADMIN_MODE
#include <ArduinoJson.h>
#endif



enum HAP_CLIENT_STATE {
	HAP_CLIENT_STATE_DISCONNECTED = 0,
	HAP_CLIENT_STATE_CONNECTED,
	HAP_CLIENT_STATE_AVAILABLE,
	HAP_CLIENT_STATE_SENT,
	HAP_CLIENT_STATE_RECEIVED,
	HAP_CLIENT_STATE_IDLE,
	HAP_CLIENT_STATE_ALL_PAIRINGS_REMOVED
};



struct HAPSubscribtionItem {
	int aid;
	int iid;

	HAPSubscribtionItem(int aid_, int iid_) : aid(aid_), iid(iid_) {};
	bool operator<(const HAPSubscribtionItem& rhs) const {
		return rhs.aid < this->aid || (rhs.aid == this->aid && rhs.iid < this->iid);
  	};

	String describe() const {
		return String(aid) + "." + String(iid);
	}
};

class HAPClient {
public:
	HAPClient();
	~HAPClient();

	HAPRequest		request;

#if defined(ARDUINO_ARCH_ESP32)
	WiFiClient 		client;
#elif defined(CORE_TEENSY)
	EthernetClient	client;
#endif

	HAP_CLIENT_STATE 	state;
	HAP_PAIR_STATE		pairState;
	HAP_VERIFY_STATE	verifyState;


	struct HAPVerifyContext 		verifyContext;
	struct HAPEncryptionContext 	encryptionContext;

	void setEncryped(bool mode) {
		_isEncrypted = mode;
	}

	bool isEncrypted() {
		return _isEncrypted;
	}

	void clear();

	bool operator==(const HAPClient &hap) const;

	bool isAdmin(){
		return _isAdmin;
	}

	void setAdmin(bool mode){
		_isAdmin = mode;
	}

	void subscribe(int aid, int iid, bool value = true);
	bool isSubscribed(int aid, int iid) const;

	std::set<HAPSubscribtionItem> subscribtions;

#if HAP_API_ADMIN_MODE
	String describe() const;
#endif

	const uint8_t* getId(){
		return _idPtr;
	}

	void setId(const uint8_t *id){
		memcpy(_idPtr, id, HAP_PAIRINGS_ID_LENGTH);
	}

protected:
	bool			_isEncrypted;
	bool			_isAdmin;

	uint8_t	_idPtr[HAP_PAIRINGS_ID_LENGTH];
};

#endif /* HAPCLIENT_HPP_ */
