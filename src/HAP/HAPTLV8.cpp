//
// HAPTLV8.cpp
// Homekit
//
//  Created on: 18.06.2017
//      Author: michael
//
#include "HAPTLV8.hpp"
#include "HAPTypes.hpp"
#include <algorithm>

#if HAP_DEBUG_TLV8
#include "HAPHelper.hpp"
#include "HAPLogging.hpp"
#endif

TLV8::TLV8()
: _head(NULL)
, _tail(NULL)
, _ptr(NULL){
}


TLV8::~TLV8() {
	TLV8Entry* current;
	TLV8Entry* temp;

	current = _head;
	temp = _head;
	while(current != NULL) {
		current = current->next;
		delete temp;
		temp = current;
	}
	delete current;
}

bool TLV8::hasType(uint8_t type){
	return searchType(_head, type) != NULL;
}

TLV8Entry* TLV8::getType(uint8_t type){
	return searchType(_head, type);
}

void TLV8::decode(const uint8_t type, uint8_t* out, size_t *outSize){
	if (_head == NULL || !out) {
		if (outSize != nullptr) {
			*outSize = 0;
		}
		return;
	}


	if (outSize != nullptr) {
		*outSize = 0;
	}

	TLV8Entry *tmp = _head;

	// uint8_t* data;
	// size_t offset = 0;
	// size_t tlvSize = size(_head, type) - 2; // returns size with type and length values

	// data = (uint8_t*) malloc(sizeof(uint8_t) * tlvSize);
	// if (!data) return NULL;

	size_t offset = 0;


	while(_head) {

		if (_head->type == type) {
			//			data[offset++] = _head->type;
			//			data[offset++] = _head->length;
			memcpy(out + offset, _head->value, _head->length);
			offset += _head->length;

			if (outSize != nullptr) {
				*outSize = offset;
			}
		}


		if(_head->next == NULL) {
			_head = tmp;
			return;
		}

		_head = _head->next;
	}
	_head = tmp;
}


void TLV8::decode(uint8_t* out, size_t *outSize){

	if (_head == NULL || !out) {
		if (outSize != nullptr) {
			*outSize = 0;
		}
		return;
	}

	if (outSize != nullptr) {
		*outSize = 0;
	}

	TLV8Entry *tmp = _head;
	// outSize = size(_head);

	size_t offset = 0;

	while(_head) {
		out[offset++] = _head->type;
		out[offset++] = _head->length;

		if (_head->type != HAPTLVType::Seperator) {
			memcpy(out + offset, _head->value, _head->length);
			offset += _head->length;
		}

		if (outSize != nullptr) {
			*outSize = offset;
		}


		if(_head->next == NULL) {
			_head = tmp;
			return;
		}

		_head = _head->next;
	}

	_head = tmp;
}


void TLV8::addSeperator() {
	TLV8Entry *ptr = new TLV8Entry();

	// error? then just return
	if( ptr == NULL )
		return;
	// assign it
	// then return pointer to new node
	else {
		ptr->type = HAPTLVType::Seperator;
		ptr->length = 0x00;
		addNode( ptr );
	}
}

bool TLV8::encode(uint8_t type, size_t length, const uint8_t data) {

	if (length != 1) return false;

	uint8_t tmp[1];
	tmp[0] = data;

	TLV8Entry *ptr = initNode(type, 1, tmp);
	addNode(ptr);

	return true;
}

bool TLV8::encode(uint8_t* rawData, size_t dataLen){

	size_t encoded = 0;
	while (encoded < dataLen) {
		TLV8Entry *ptr = new TLV8Entry();
		// error? then just return
		if( ptr == NULL )
			return false;
		// assign it
		// then return pointer to new node
		else {
			ptr->type = rawData[encoded];
			ptr->length = rawData[encoded+1];

			ptr->value = new unsigned char[ptr->length];
			memcpy(ptr->value, rawData + (encoded+2), ptr->length);

//			Serial.printf("type:   %x \n", ptr->type);
//			Serial.printf("length: %x \n", ptr->length);
//			Serial.print("value: ");
//			Serial.println(SRPClass::toHex(ptr->value, ptr->length));

			addNode(ptr);

			encoded += ptr->length + 2;
		}
	}
	return true;
}


bool TLV8::encode(uint8_t type, size_t length, const uint8_t* rawData) {

	if (length == 0) return false;


	size_t bdone = 0;

	if (length > 255) {
		while (length > 0) {
			unsigned char tmp[255];
			size_t l =  (length > 255) ? 255 : length;
			memcpy(tmp, rawData + bdone, l);
			bdone += l;
			length -= l;

			TLV8Entry *ptr = initNode(type, l, tmp);
			addNode(ptr);
		}

	} else {
		bdone = length;
		TLV8Entry *ptr = initNode(type, length, rawData);
		addNode(ptr);
	}

	return (bdone == length) ? true : false;
}

bool TLV8::encode(uint8_t type, const std::initializer_list<uint8_t> data){

	size_t length = data.size();
	uint8_t rawData[length];

	std::copy(data.begin(), data.end(), rawData);

	return encode(type, length, rawData);
}

TLV8Entry* TLV8::initNode(const uint8_t* rawData) {
	TLV8Entry *ptr = new TLV8Entry();

	// error? then just return
	if( ptr == NULL )
		return static_cast<TLV8Entry *>(NULL);
	// assign it
	// then return pointer to new node
	else {

		ptr->type = rawData[0];
		ptr->length = rawData[1];


		ptr->value = new unsigned char[ptr->length];
		memcpy(ptr->value, rawData + 2, ptr->length);
		return ptr;
	}
}

TLV8Entry* TLV8::initNode(const uint8_t type, const uint8_t length, const uint8_t* value) {
	TLV8Entry *ptr = new TLV8Entry();


	// error? then just return
	if( ptr == NULL )
		return static_cast<struct TLV8Entry *>(NULL);
	// assign it
	// then return pointer to new node
	else {
		ptr->type = type;
		ptr->length = length;


		ptr->value = new unsigned char[length];
		memcpy(ptr->value, value, length);
		return ptr;
	}
}

// adding to the end of list
void TLV8::addNode( TLV8Entry *ptr )  {
	// if there is no node, put it to head
	if( _head == NULL ) {
		_head = ptr;
		_tail = ptr;
	}

	// link in the new_node to the tail of the list
	// then mark the next field as the end of the list
	// adjust tail to point to the last node

	_tail->next = ptr;
	ptr->next = NULL;
	_tail = ptr;
}


/**
 * Add entry only if the same type doesn't exist
 */
void TLV8::insertNode( TLV8Entry *ptr ) {
	TLV8Entry *temp, *prev;

	if( _head == NULL ) {                    // if an empty list,
		_head = ptr;                      // set 'head' to it
		_tail = ptr;
		_head->next = NULL;                  // set end of list to NULL
		return;
	}

	temp = _head;                            // start at beginning of list
	// while currentname < newname
	while( temp->type < ptr->type) {	    // to be inserted then
		temp = temp->next;                // goto the next node in list
		if( temp == NULL )                // don't go past end of list
			break;
	}
	// set previous node before we insert
	// first check to see if it's inserting
	if( temp == _head ) {		    	    // before the first node
		ptr->next = _head;                 // link next field to original list
		_head = ptr;                       // head adjusted to new node
	}
	else {				    // it's not the first node
		prev = _head;		    	    // start of the list,
		while( prev->next != temp ) {
			prev = prev->next;	    	    // will cycle to node before temp
		}
		prev->next = ptr;                 // insert node between prev and next
		ptr->next = temp;
		if( _tail == prev )		    // if the new node is inserted at the
			_tail = ptr;		    // end of the list the adjust 'end'
	}
}


TLV8Entry* TLV8::searchType(TLV8Entry* ptr, uint8_t type) {
	while( type != ptr->type ) {
		ptr = ptr->next;
		if( ptr == NULL )
			break;
	}
	return ptr;
}


TLV8Entry* TLV8::searchId(TLV8Entry* ptr, uint8_t id) {
	while( id != ptr->id ) {
		ptr = ptr->next;
		if( ptr == NULL )
			break;
	}
	return ptr;
}

void TLV8::clear() {
	if (_head == NULL) return;
	deleteList(_head);
}


void TLV8::deleteNode( TLV8Entry *ptr )
{
	TLV8Entry *temp, *prev;
	temp = ptr;    // node to be deleted
	prev = _head;   // start of the list, will cycle to node before temp

	if( temp == prev ) {                    // deleting first node?
		_head = _head->next;                // moves head to next node
		if( _tail == temp )                 // is it end, only one node?
			_tail = _tail->next;            // adjust end as well
		delete temp ;                       // free up space
	}
	else {                                  // if not the first node, then
		while( prev->next != temp ) {       // move prev to the node before
			prev = prev->next;              // the one to be deleted
		}
		prev->next = temp->next;            // link previous node to next
		if( _tail == temp )                 // if this was the end node,
			_tail = prev;                   // then reset the end pointer
		delete temp;                        // free up space
	}
}

void TLV8::deleteList( TLV8Entry *ptr )
{
	TLV8Entry *temp;

	if ( _head == NULL ) return;   			// don't try to delete an empty list

	if ( ptr == _head ) {					// if we are deleting the entire list
		_head = NULL;						// then reset head and
		_tail = NULL;						// end to empty
	} else {
		temp = _head;						// if it's not the entire list, readjust end
		while( temp->next != ptr )       	// locate previous node to ptr
			temp = temp->next;
		_tail = temp;                     	// set end to node before ptr
	}

	while( ptr != NULL ) {					// whilst there are still nodes to delete
		temp = ptr->next;					// record address of next node
		delete ptr;							// free this node
		ptr = temp;							// point to next node to be deleted
	}
}

uint8_t TLV8::count(){
	return count(_head);
}

uint8_t TLV8::count( TLV8Entry *ptr ) {
	size_t count = 0;
	while(ptr) {
		count++;
		if(ptr->next == 0) {
			return count;
		}
		ptr = ptr->next;
	}
	return count;
}

size_t TLV8::size() {
	return size(_head);
}

size_t TLV8::size(uint8_t type) {
	return size(_head, type);
}

size_t TLV8::size( TLV8Entry *ptr ){
	long size = 0;
	while(ptr) {

		size += ptr->length + 2;	// + 2 for TYPE and LENGTH

		if(ptr->next == NULL) {
			return size;
		}

		ptr = ptr->next;
	}
	return size;
}

size_t TLV8::size(TLV8Entry *ptr, uint8_t type ){
	long size = 0;

	while(ptr) {

		if (type == ptr->type) {
			size += ptr->length;
		}

		if (ptr->next == NULL) {
			return size;
		}
		ptr = ptr->next;
	}
	return size;
}

bool TLV8::isValidTLVType(uint8_t type) {
	switch (type) {
		case HAPTLVType::Method:
			return true;
		case HAPTLVType::Identifier:
			return true;
		case HAPTLVType::Salt:
			return true;
		case HAPTLVType::PublicKey:
			return true;
		case HAPTLVType::Proof:
			return true;
		case HAPTLVType::EncryptedData:
			return true;
		case HAPTLVType::State:
			return true;
		case HAPTLVType::Error:
			return true;
		case HAPTLVType::RetryDelay:
			return true;
		case HAPTLVType::Certificate:
			return true;
		case HAPTLVType::Signature:
			return true;
		case HAPTLVType::Permissions:
			return true;
		case HAPTLVType::FragmentData:
			return true;
		case HAPTLVType::LastFragment:
			return true;
		case HAPTLVType::Seperator:
			return true;
		default:
			return false;
	}
}


#if HAP_DEBUG_TLV8
void TLV8::printNode( TLV8Entry *ptr )
{
	LOG_D("T: 0x%X\n", ptr->type);
	LOG_D("L: %d\n", ptr->length);
	 HAPTime::timestring_D("V:", ptr->value, ptr->length);
}

void TLV8::print( ) {
	printList(_head);
}

void TLV8::printList( TLV8Entry *ptr ){
	LOG_D( "================= TLV8 =================\n");
	if(!ptr) {
		LOG_D( "Nothing to display!\n");
		return;
	}

	while(ptr) {
		printNode(ptr);
		ptr = ptr->next;
	}
}
#endif