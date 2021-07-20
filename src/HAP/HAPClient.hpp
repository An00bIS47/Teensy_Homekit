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
#include "HAPTypes.hpp"

#if HAP_API_ADMIN_MODE
#include <ArduinoJson.h>
#endif

struct HAPClientState {
	enum Type : uint8_t {
		Disconnected 		= 0x00,
		Connected			= 0x01,
		Available			= 0x02,
		Idle				= 0x03,
		AllPairingsRemoved 	= 0x04
	};
};


struct HAPSubscribtionItem {
	int aid;
	int iid;

	HAPSubscribtionItem(int aid_, int iid_) : aid(aid_), iid(iid_) {};
	bool operator<(const HAPSubscribtionItem& rhs) const {
		return rhs.aid < this->aid || (rhs.aid == this->aid && rhs.iid < this->iid);
  	};

	std::string describe() const {
		char tmp[10] = {'\0',};
		sprintf(tmp, "%d.%d", aid, iid);
		return std::string(tmp);
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

	HAPClientState::Type 	state;
	HAPPairingState::Type	pairState;
	HAPVerifyState::Type	verifyState;


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
