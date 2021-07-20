//
// HAPTypes.hpp
// Homekit
//
//  Created on: 18.06.2017
//      Author: michael
//

#ifndef HAPTYPES_HPP_
#define HAPTYPES_HPP_

#include <Arduino.h>

struct HAPTLVType {
	enum Type : uint8_t {
		Method			= 0x00,
		Identifier		= 0x01,
		Salt			= 0x02,
		PublicKey		= 0x03,
		Proof			= 0x04,
		EncryptedData 	= 0x05,
		State			= 0x06,
		Error			= 0x07,
		RetryDelay		= 0x08,
		Certificate		= 0x09,
		Signature		= 0x0A,
		Permissions		= 0x0B,
		FragmentData	= 0x0C,
		LastFragment	= 0x0D,
		
		Seperator		= 0xFF
	};
};


struct HAPPairingType {
	enum Type : uint8_t {
		Setup			= 0x00,
		SetupAuth		= 0x01,
		Verify			= 0x02,
		Add				= 0x03,
		Remove			= 0x04,
		List			= 0x05
	};
};


struct HAPPairingState {
	enum Type : uint8_t {
		Reserved		= 0x00,
		M1				= 0x01,
		M2				= 0x02,
		M3				= 0x03,
		M4				= 0x04,
		M5				= 0x05,
		M6				= 0x06
	};
};

struct HAPVerifyState {
	enum Type : uint8_t {
		Reserved		= 0x00,
		M1				= 0x01,
		M2				= 0x02,
		M3				= 0x03,
		M4				= 0x04
	};
};

struct HAPErrorType {
	enum Type : uint8_t {
		None			= 0x00,
		Unknown			= 0x01,
		Authentication 	= 0x02,
		Backoff			= 0x03,
		MaxPeers		= 0x04,
		MaxTries		= 0x05,
		Unavailable		= 0x06,
		Busy			= 0x07
	};
};

struct HAPErrorCode {
	enum Type : int {
		Success 					= 0,
		InsufficientPriviliges		= -70401,
		InternalCommunication		= -70402,
		Busy						= -70403,
		ReadOnlyWrite				= -70404,
		WriteOnlyRead				= -70405,
		NoNotification				= -70406,
		OutOfResources				= -70407,
		TimedOut					= -70408,
		ResourceNotFound			= -70409,
		InvalidValue				= -70410,
		AuthFailed					= -70411
	};
};

#endif /* HAPTYPES_HPP_ */
