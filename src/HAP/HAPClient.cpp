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
#include "HAPEncryption.hpp"
#include "m_chacha20_poly1305.h"
#include "mbedtls/chachapoly.h"


HAPClient::HAPClient()
: state(HAP_CLIENT_STATE_DISCONNECTED)
, pairState(HAP_PAIR_STATE_RESERVED)
, verifyState(HAP_VERIFY_STATE_RESERVED)
, _isEncrypted(false)
, _headerSent(false)
, _isAdmin(false)
, _chunkedMode(true)
// , shouldNotify(false)
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
	_headerSent = false;
	
	_headers.clear();	
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

int HAPClient::available(){
	return client.available();
}

int HAPClient::peek(){
	return client.peek();
}

int HAPClient::read(){
	return client.read();
}

void HAPClient::flush(){
	client.flush();
}


String HAPClient::buildHeaderAndStatus(int statusCode, size_t size){
	
	String response = "";
	if (_headerSent == true) return response;

	// Status code and Status message
	response += "HTTP/1.1 ";
	response += String(statusCode) + " " + statusMessage(statusCode);
	response += "\r\n";		

	// Headers
	for (auto & elem : _headers){
		response += elem.describe();
		response += "\r\n";
	}	

	// Transfer-Encoding or Content length
	if (_chunkedMode) {
		response += "Transfer-Encoding: chunked\r\n";
	} else {
		response += "Content-Length: ";
		response += String(size);
		response += "\r\n";
	}
		
	// end of headers
	response += "\r\n";

	return response;
}



size_t HAPClient::write(const uint8_t* buffer, size_t size) {	

	size_t bytesSend = 0;
	
	uint8_t writeBuffer[1360];
	size_t writeBufferUsed = 0;
	
	String headerStr = buildHeaderAndStatus(200, size);

	// WriteBufferingStream bufferedWifiClient{client, 1360};

#if HAP_DEBUG_REQUESTS
	LogD(headerStr, true);
	HAPHelper::array_print("Response:", buffer, size);
#endif


	if (!_isEncrypted) {

		LogV("\nSending unencrypted response!", true);

		size_t bytesChunk = 0;	

		if (headerStr != ""){
			memcpy(writeBuffer, headerStr.c_str(), headerStr.length());
			writeBufferUsed += headerStr.length();
			_headerSent = true;
		}	
		
		int remainingSize = size;
		size_t written = 0;

		while (remainingSize > 0){
			size_t toWrite = (remainingSize > 1360 - writeBufferUsed) ? 1360 - writeBufferUsed : remainingSize;

			// chunk size for payload
			if (_chunkedMode) {
				char chunkSize[12];
				sprintf(chunkSize, "%x\r\n", toWrite);

				memcpy(writeBuffer + writeBufferUsed, chunkSize, strlen(chunkSize));
				writeBufferUsed	+= strlen(chunkSize);
			}


			memcpy(writeBuffer + writeBufferUsed, buffer + written, toWrite);
			writeBufferUsed += toWrite;

#if HAP_DEBUG_REQUESTS_DETAILED	
			HAPHelper::array_print("Response:", writeBuffer, writeBufferUsed);
#endif

			bytesSend += client.write(writeBuffer, writeBufferUsed);
			writeBufferUsed = 0;
			
			written += toWrite;
			remainingSize -= written;

			bytesChunk += client.write((uint8_t*) "\r\n", 2);			

#if HAP_DEBUG_REQUESTS_DETAILED	
			HAPHelper::array_print("endline:", (uint8_t*)"\r\n", 2);
#endif			
			bytesSend += bytesChunk;
		}


		// End of request
		// send end chunk
		if (_chunkedMode) {
			char chunkSize[8];
			sprintf(chunkSize, "%x\r\n", 0);

#if HAP_DEBUG_REQUESTS_DETAILED	
			HAPHelper::array_print("chunksize 0:", (uint8_t*) chunkSize, strlen(chunkSize));
#endif			
			bytesSend += client.write((uint8_t*) chunkSize, strlen(chunkSize));				
		}

#if HAP_DEBUG_REQUESTS_DETAILED	
		HAPHelper::array_print("endline:", (uint8_t*)"\r\n", 2);
#endif
		// send end of request
		bytesSend += client.write((uint8_t*) "\r\n", 2);			

		
	} else {

		LogV("\nSending encrypted response!", true);

		size_t bytesHeader = 0;
		size_t bytesChunk = 0;

		int ret;

		mbedtls_chachapoly_context chachapoly_ctx;
		mbedtls_chachapoly_init(&chachapoly_ctx);
		mbedtls_chachapoly_setkey(&chachapoly_ctx, encryptionContext.encryptKey);


		if (headerStr != ""){
			memcpy(writeBuffer, headerStr.c_str(), headerStr.length());
			writeBufferUsed += headerStr.length();
			bytesHeader = headerStr.length();			
		}	

		// chunk size for payload
		if (_chunkedMode) {
			char chunkSize[8];
			sprintf(chunkSize, "%x\r\n", size);

			memcpy(writeBuffer + writeBufferUsed, chunkSize, strlen(chunkSize));
			writeBufferUsed	+= strlen(chunkSize);
			bytesChunk += strlen(chunkSize);		
		}

		uint8_t nonce[12] = {0,};
		nonce[4] = encryptionContext.encryptCount % 256;
		nonce[5] = encryptionContext.encryptCount++ / 256;


		ret = mbedtls_chachapoly_starts( &chachapoly_ctx, nonce, MBEDTLS_CHACHAPOLY_ENCRYPT );
		if( ret != 0 ) {
			LogE("Error: mbedtls_chachapoly_starts failed", true);
		}

		
		uint8_t aad[HAP_ENCRYPTION_AAD_SIZE];
		aad[0] = size % 256;
		aad[1] = size / 256;			

		ret = mbedtls_chachapoly_update_aad( &chachapoly_ctx, aad, HAP_ENCRYPTION_AAD_SIZE );
		if( ret != 0 ) {
			LogE("Error: mbedtls_chachapoly_update_aad failed", true);
		}		

		int remainingSize = size;
		size_t written = 0;


		while (remainingSize > 0){
			int toWrite = (remainingSize > 1360 - writeBufferUsed) ? 1360 - writeBufferUsed : remainingSize;
			
			uint8_t encryptedBuffer[toWrite + bytesHeader + bytesChunk];

			if (!_headerSent){
				bytesHeader = 0;
				_headerSent = true;
				bytesChunk = 0;
			}


			memcpy(writeBuffer + writeBufferUsed, buffer + written, toWrite);

			ret = mbedtls_chachapoly_update( &chachapoly_ctx, toWrite, writeBuffer, encryptedBuffer);
			if( ret != 0 ) {
				LogE("Error: mbedtls_chachapoly_update failed", true);
			}				
			writeBufferUsed += toWrite + bytesHeader + bytesChunk;
			
			// bytesSend += bufferedWifiClient.write(encryptedBuffer, sizeof(encryptedBuffer));
			bytesSend += client.write(encryptedBuffer, sizeof(encryptedBuffer));

			writeBufferUsed = 0;
			
			written += toWrite;
			remainingSize -= toWrite + bytesHeader + bytesChunk;
		}

		memset(writeBuffer, 0, 1360);

		{
			const char* chunkSize = "\r\n";
			memcpy(writeBuffer + writeBufferUsed, chunkSize, strlen(chunkSize));
			writeBufferUsed	+= strlen(chunkSize);
		}


		// End of request
		// send end chunk
		if (_chunkedMode) {
			char chunkSize[8];
			sprintf(chunkSize, "%x\r\n", 0);
			memcpy(writeBuffer + writeBufferUsed, chunkSize, strlen(chunkSize));
			writeBufferUsed	+= strlen(chunkSize);						
		}

		{
			const char* chunkSize = "\r\n";
			memcpy(writeBuffer + writeBufferUsed, chunkSize, strlen(chunkSize));
			writeBufferUsed	+= strlen(chunkSize);
		}

		
		uint8_t encryptedBuffer[writeBufferUsed];
		ret = mbedtls_chachapoly_update( &chachapoly_ctx, writeBufferUsed, writeBuffer, encryptedBuffer);
		if( ret != 0 ) {
			LogE("Error: mbedtls_chachapoly_update failed", true);
		}

		// bytesSend += bufferedWifiClient.write(encryptedBuffer, writeBufferUsed);
		bytesSend += client.write(encryptedBuffer, writeBufferUsed);
		
		uint8_t tag[16];
		ret = mbedtls_chachapoly_finish( &chachapoly_ctx, tag );
		if( ret != 0 ) {
			LogE("Error: mbedtls_chachapoly_finish failed", true);
		}
		
		// bytesSend += bufferedWifiClient.write(tag, 16);		
		bytesSend += client.write(tag, 16);
	}	

	// bufferedWifiClient.flush();
	return bytesSend;
}


size_t HAPClient::write(uint8_t b){
	return client.write(b);
}

void HAPClient::setHeader(HAPClientHeader header){
	setHeader(header.name, header.value);
}

void HAPClient::setHeader(const String& name, const String& value) {
	bool found = false;
	for (auto & elem : _headers)
	{
		if (elem.name == name){
			elem.value = value;
			found = true;
			break;
		}
	}

	if (!found) {
		_headers.push_back(HAPClientHeader(name, value));
	}
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

String HAPClient::statusMessage(int statusCode){

	switch(statusCode) {
        case 200:                        
            return F("OK");
		case 201:                        
            return F("Created");	
		case 202:                        
            return F("Accepted");			
		case 204:                        
            return F("No Content");	
		case 400:
			return F("Bad Request");
		case 401:
			return F("Unauthorized");	
		case 403:
			return F("Forbidden");			
		case 404:
			return F("Not Found");		
		case 405:
			return F("Method not allowed");	
		case 409:
			return F("Conflict");		
		case 413:
			return F("Payload too large");
		case 420:
			return F("Enhance your calm");	
		default:
			return F("");
	}
}


