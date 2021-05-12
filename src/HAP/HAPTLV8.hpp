//
// HAPTLV8.hpp
// Homekit
//
//  Created on: 18.06.2017
//      Author: michael
//

#ifndef HAPTLV8_HPP_
#define HAPTLV8_HPP_

#include <Arduino.h>

#ifndef HAP_DEBUG_TLV8
#define HAP_DEBUG_TLV8 0
#endif

struct TLV8Entry {
	uint8_t type;
	uint8_t length;
	uint8_t* value = nullptr;

	uint8_t id;

	TLV8Entry* next = nullptr;

	~TLV8Entry(){
		if (value) delete[] value;
	};
};


class TLV8 {
public:
	TLV8();
	~TLV8();

	void addSeperator();

	TLV8Entry* searchType(uint8_t type){
		return searchType(_head, type);
	};

	TLV8Entry* searchType(TLV8Entry* ptr, uint8_t type);

	TLV8Entry* searchId(TLV8Entry* ptr, uint8_t id);

	TLV8Entry* getType(uint8_t type);
	bool hasType(uint8_t type);

	bool encode(uint8_t type, size_t length, const uint8_t data);
	bool encode(uint8_t type, size_t length, const uint8_t* rawData);
	bool encode(uint8_t* rawData, size_t dataLen);

	bool encode(uint8_t type, const std::initializer_list<uint8_t> data);

	void decode(uint8_t* out, size_t *outSize = nullptr);
	void decode(const uint8_t type, uint8_t* out, size_t *outSize = nullptr);


	void clear();


#if HAP_DEBUG_TLV8
	void print();
	static void printList( TLV8Entry* ptr);
	static void printNode( TLV8Entry* ptr);
#endif


	// Size of data values + TYPE + LENGTH
	size_t size();
	size_t size( uint8_t type );
	static size_t size( TLV8Entry *ptr );
	static size_t size( TLV8Entry *ptr, uint8_t type );

	// Number of entries
	uint8_t count();
	static uint8_t count(TLV8Entry* ptr);

	static bool isValidTLVType(uint8_t type);


	TLV8Entry *_head;
protected:

	TLV8Entry* initNode(const uint8_t type, const uint8_t length, const uint8_t* value);
	TLV8Entry* initNode(const uint8_t* rawData);

	void addNode( TLV8Entry* ptr);
	void insertNode( TLV8Entry* ptr);
	void deleteNode( TLV8Entry* ptr);

	void deleteList( TLV8Entry* ptr);

	TLV8Entry *_tail;
	TLV8Entry *_ptr;
};

#endif /* HAPTLV8_HPP_ */
