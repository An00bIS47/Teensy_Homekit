// 
// HAPFakegato.cpp
// Homekit
//
//  Created on: 31.03.2021
//      Author: michael
//

#include "HAPFakegato.hpp"
#include "HAPLogger.hpp"
#include "HAPTime.hpp"

#if defined ( ARDUINO_ARCH_ESP32 )
#if ESP_IDF_VERSION_MAJOR == 4
#include "mbedtls/base64.h"
#else
extern "C" {
    #include "crypto/base64.h"    
}

#endif
#elif defined(CORE_TEENSY)
#include <Base64.h>
#include "mbedtls/base64.h"
#endif

HAPFakegato::HAPFakegato(){

}


HAPFakegato::~HAPFakegato(){
    if (_historyInfo != nullptr) delete _historyInfo;
    if (_historyEntries != nullptr) delete _historyEntries;
    if (_historyRequest != nullptr) delete _historyRequest;
    if (_historySetTime != nullptr) delete _historySetTime;

    if (_configRead != nullptr) delete _configRead;
    if (_configWrite != nullptr) delete _configWrite;
   
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
void HAPFakegato::registerFakeGatoService(HAPAccessory* accessory, const String& name, bool withSchedule){
    _name = name;

    HAPService* fgService = new HAPService(HAP_SERVICE_FAKEGATO_HISTORY);    

    HAPCharacteristicT<String>* accNameCha = new HAPCharacteristicT<String>(HAP_CHARACTERISTIC_NAME, HAP_PERMISSION_READ, HAP_HOMEKIT_DEFAULT_STRING_LENGTH);
    accNameCha->setValue(name + " History");
    accessory->addCharacteristicToService(fgService, accNameCha);

    
    // History Info
    _historyInfo = new HAPCharacteristicT<String>(HAP_CHARACTERISTIC_FAKEGATO_HISTORY_STATUS, HAP_PERMISSION_READ|HAP_PERMISSION_NOTIFY, 128);    
    _historyInfo->setDescription("EVE History Info");
    _historyInfo->setValue((char*)NULL);    
    // auto callbackS2R1 = std::bind(&HAPFakeGato::setS2R1Characteristics, this, std::placeholders::_1, std::placeholders::_2);            
    // _historyInfo->setValueChangeCallback(callbackS2R1);
    auto callbackGetHistoryInfo = std::bind(&HAPFakegato::callbackGetHistoryInfo, this);            
    _historyInfo->setValueGetCallback(callbackGetHistoryInfo);    
    accessory->addCharacteristicToService(fgService, _historyInfo);
    
    // History Entries
    _historyEntries = new HAPCharacteristicT<String>(HAP_CHARACTERISTIC_FAKEGATO_HISTORY_ENTRIES, HAP_PERMISSION_READ|HAP_PERMISSION_NOTIFY, HAP_FAKEGATO_CHUNK_BUFFER_SIZE);    
    _historyEntries->setDescription("EVE History Entries");
    _historyEntries->setValue((char*)NULL);  
    // auto callbackS2R2 = std::bind(&HAPFakeGato::setS2R2Characteristics, this, std::placeholders::_1, std::placeholders::_2);            
    // _historyEntries->setValueChangeCallback(callbackS2R2);
    auto callbackGetHistoryEntries = std::bind(&HAPFakegato::callbackGetHistoryEntries, this);            
    _historyEntries->setValueGetCallback(callbackGetHistoryEntries);

    accessory->addCharacteristicToService(fgService, _historyEntries);

    // History Request
    _historyRequest = new HAPCharacteristicT<String>(HAP_CHARACTERISTIC_FAKEGATO_HISTORY_REQUEST, HAP_PERMISSION_WRITE, 128);
    _historyRequest->setDescription("EVE History Request");   
    _historyInfo->setValue((char*)NULL);         
    auto callbackHistoryRequest = std::bind(&HAPFakegato::callbackHistoryRequest, this, std::placeholders::_1, std::placeholders::_2);        
    _historyRequest->setValueChangeCallback(callbackHistoryRequest);
    accessory->addCharacteristicToService(fgService, _historyRequest);
    
    // Set Time
    _historySetTime = new HAPCharacteristicT<String>(HAP_CHARACTERISTIC_FAKEGATO_SET_TIME, HAP_PERMISSION_WRITE, 128);
    _historySetTime->setDescription("EVE SetTime");
    _historyInfo->setValue((char*)NULL);    
    auto callbackSetTime = std::bind(&HAPFakegato::callbackHistorySetTime, this, std::placeholders::_1, std::placeholders::_2);        
    _historySetTime->setValueChangeCallback(callbackSetTime);
    accessory->addCharacteristicToService(fgService, _historySetTime);
        

    if (withSchedule){        
        // Config Read
        _configRead = new HAPCharacteristicT<String>(HAP_CHARACTERISTIC_FAKEGATO_CONFIG_READ, HAP_PERMISSION_READ|HAP_PERMISSION_NOTIFY, HAP_FAKEGATO_CHUNK_BUFFER_SIZE);
        _configRead->setDescription("EVE Schedule Read");
        _historyInfo->setValue((char*)NULL);    
        auto callbackConfigRead = std::bind(&HAPFakegato::scheduleRead, this, std::placeholders::_1, std::placeholders::_2);        
        _configRead->setValueChangeCallback(callbackConfigRead);
        accessory->addCharacteristicToService(fgService, _configRead);

    
        // Config Write
        _configWrite = new HAPCharacteristicT<String>(HAP_CHARACTERISTIC_FAKEGATO_CONFIG_WRITE, HAP_PERMISSION_WRITE, 256);
        _configWrite->setDescription("EVE Schedule Write");
        _historyInfo->setValue((char*)NULL);    
        auto callbackConfigWrite = std::bind(&HAPFakegato::scheduleWrite, this, std::placeholders::_1, std::placeholders::_2);        
        _configWrite->setValueChangeCallback(callbackConfigWrite);
        accessory->addCharacteristicToService(fgService, _configWrite);
    }

    accessory->addService(fgService);
    
    begin();   

    enable(true);
}


void HAPFakegato::handle(bool forced){
    if ( shouldHandle() || forced ){       
        // This line could cause a crash 
        // LogD(HAPTime::timeString() + " " + String(__CLASS_NAME__) + "->" + String(__FUNCTION__) + " [   ] " + "Handle fakegato ", true);         
        
        if (_periodicUpdates) {
            if (_callbackAddEntry != NULL){
                bool overwritten = !_callbackAddEntry();  
                
                // ToDo: Persist history ??
                if (overwritten) {
                    LogW("A fakegato history entry was overwritten!", true);
                }                      
            }   
        }
                                         
    }
}

bool HAPFakegato::shouldHandle(){

    if (_isEnabled && _periodicUpdates) {
        unsigned long currentMillis = millis(); // grab current time

        if ((unsigned long)(currentMillis - _previousMillis) >= _interval) {

            // save the last time you blinked the LED
            _previousMillis = currentMillis;

            //LogD("Handle plugin: " + String(_name), true);			
            return true;			
        }
    }

    return false;
}


// // 
// // t == temperature - float     >> calculate value * 100
// // h == humidity    - float     >> calculate value * 100
// // p == airpressure - uint16_t  >> calculate value * 10
// // ...
// #if defined(ARDUINO_TEENSY41)
// FLASHMEM 
// #endif
// void HAPFakegato::addEntry(uint8_t bitmask, const char* fmt, ...){
//     va_list args;
//     va_start(args, fmt);

//     // calculating needed buffer size   
//     uint8_t bufferSize = 0;

//     const char* fmt2 = fmt;
//     while (*fmt2 != '\0') {
//         if ((*fmt2 == 't') || (*fmt2 == 'h') || (*fmt2 == 'p')) {
//             // All values with length of 2
//             //    + Temperature 
//             //    + Humidity    
//             //    + Air Pressure                            
//             bufferSize += 2;
//         }
//         ++fmt2;
//     }
    
//     uint8_t buffer[bufferSize];
//     uint8_t offset = 0;

//     while (*fmt != '\0') {
//         if ( (*fmt == 't') || (*fmt == 'h') ) {
//             // Calculate FLOAT value * 100:
//             //    + Temperature 
//             //    + Humidity
//             double value = va_arg(args, double);
//             value = value * 100;
//             ui16_to_ui8 bytesUnion;
//             bytesUnion.ui16 = value;                
//             memcpy(buffer + offset, bytesUnion.ui8, 2);                
//             offset += 2;
//         } else if (*fmt == 'p') {
//             // Calculate UINT16_T value * 100:
//             //    + Air Pressure
//             int value = va_arg(args, int);
//             value = value * 10;
//             ui16_to_ui8 bytesUnion;
//             bytesUnion.ui16 = (uint16_t)value;                
//             memcpy(buffer + offset, bytesUnion.ui8, 2);                
//             offset += 2;                
//         }
//         ++fmt;
//     }

//     va_end(args);

//     addDataToBuffer(bitmask, buffer, offset);
// }


#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
void HAPFakegato::addEntry(uint8_t bitmask){
    uint8_t data[valueLength()];
    uint8_t offset = 0;
    for (uint8_t i=0; i < _signatures.size(); i++){
        if (bitmask && (1 << i)){                                   
            uint8_t length = 0;
            _signatures[i]->getBytes(data + offset, &length);
            offset += length;
        }
    }
    addDataToBuffer(bitmask, data, offset);
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
void HAPFakegato::addDataToBuffer(uint8_t bitmask, uint8_t* data, uint8_t length){

    LogD(HAPTime::timeString() + " HAPFakegato->addDataToBuffer [   ] " + "Adding entry for " + _name + " [size=" + String(_entries.size()) + " bitmask=" + String(bitmask) + "]", true);


    // Entry will be overwritten ...
    if (_entries.capacity - _entries.size() < 1) {
        // ToDo: Add a dispatcher for overwritten objects? 
        //       Probably first just a LOG message ?
        //       Therefore a reference to the aid and iid would be needed ...
        LogW("WARNING: Fakegato entry will be overwritten!", true);
        delete _entries.shift();
    }

#if HAP_DEBUG_FAKEGATO   
    Serial.print("bitmask: "); Serial.println(bitmask);
    HAPHelper::array_print("FAKEGATO ENTRY DATA", data, length);
#endif

    size_t indexLast = _entries.size() - 1;        
    bool overwritten = !_entries.push(std::move(new HAPFakegatoDataEntry(bitmask, HAPTime::timestamp(), data, length)));
    if (overwritten == true) {
        // ToDo: Add overwritten handling..
        _rolledOver = true;        
    }

    // ToDo: Update History Info Characteristic with new size
    //       Required ?? should be getting the update via the read callback ?!
}

/**
 * @brief 
 *  E863F117-079E-48FF-8F27-9C2605A29F52
 *
 *  This read-only characteristics is used to send the actual log entry to Eve.app It is an array of logs with each entry having x bytes as determined by Byte 1. The first portion up to byte 10 included is common, the last portion contain the data specific for each accessory.
 *
 *  Byte 1: Length (i.e. 14 for 20 Bytes)
 *  Bytes 2-5: entry counter
 *  Bytes 6-9: Seconds since reference time set with type 0x81 entry. In order to account for multiple iOS devices, the actual reference time and offset are apparently always reported also in E863F116
 *  Byte 10: Entry type
 *  0x81 Entry to set the reference timestamp, to be written on bytes 11-14. A negative offset can be written on bytes 6-9, and it's probably neeeded when the clock is updated (21 bytes in total).
 *  0x07 Entry for Eve Weather log.
 *  0x1f Entry for Eve Energy log (20 bytes in total)
 *  0x1e Entry for Eve Energy log (18 bytes in total - not working)
 *  0x0f Entry for Eve Room log.
 *  0x02 Entry for Eve Motion log.
 *  0x01 Entry for Eve Door log.
 *  0x1f Entry for Eve Thermo log.
 *  0x05 Entry for Eve Aqua, valve on, 13 bytes in total
 *  0x07 Entry for Eve Aqua, valve off + water usage, 21 bytes in total
 *
 *
 * // ToDo: Rewrite as READ callback for Characteristic
 *       Be aware that you have to add the reftime entry if requestedIndex was ?? 01 oder 00 ?  idk
 *       and that you have to base64 encode if you use a string value
 *       basically have a look at the old implementation
 **/
String HAPFakegato::callbackGetHistoryEntries(){
    
    if (!_transfer) return F("AA==");
    
    uint16_t offset = 0;

    // 
    // buffersize
    // calculation: (16 * (10 + 6)) + 21
    //                |     |   |      +-> 21 is the size of a reftime entry
    //                |     |   +-> 6 is max length for all values in bitmask (in this case)
    //                |     +-> 10 is the static data size
    //                +-> batch size (defined as 16 in this case)


    // ToDo: fix getMaxEntryValueLength()!!
    // uint16_t buffersize = (HAP_FAKEGATO_BATCH_SIZE * (10 + getMaxEntryValueLength())) + 21;    
    // Serial.print(">>>> bufferSize: ");
    // Serial.println(buffersize); Serial.send_now();
    // uint8_t data[buffersize];

    // uint16_t buffersize = 512;
    uint8_t data[512];
    
    uint16_t entryCounter = 0;
    uint8_t usedBatch = 0;

#if HAP_DEBUG_FAKEGATO          
        Serial.print(">>>> _requestedIndex: ");
        Serial.println(_requestedIndex); Serial.send_now();    
#endif 


    if (_requestedIndex == 0){
        // ToDo: 

    } else if (_requestedIndex == 1){
        getRefTime(data, &offset); 
        entryCounter = _requestedIndex + 1;
        _requestedIndex--;   
        usedBatch++;        
    } else {
        _requestedIndex--; 
        entryCounter = _requestedIndex + 1;
    }    


#if HAP_DEBUG_FAKEGATO          
        Serial.print(">>>> _requestedIndex: ");
        Serial.println(_requestedIndex); Serial.send_now();
#endif 

    for (uint8_t i=0; i < (HAP_FAKEGATO_BATCH_SIZE - usedBatch); i++) {    
      
        if (_requestedIndex > _entries.size() - 1 ) { 
            //|| _entries[_requestedIndex]->length == 0
            // _requestedIndex is greater than _entries.size()!
            Serial.println("_requestedIndex is greater than _entries.size() or length = 0! BREAK");

            _transfer = false;
            break;
        }
        
       
#if HAP_DEBUG_FAKEGATO          
        Serial.print(">>>> _requestedIndex: ");
        Serial.println(_requestedIndex); Serial.send_now();
#endif 
        
        uint8_t currentOffset = 0;

        uint8_t size = 10;  // length of static data is 10
        size += getEntryValueLength(_entries[_requestedIndex]->bitmask);
        

#if HAP_DEBUG_FAKEGATO        
        Serial.print(">>>> size: ");
        Serial.println(size);

        Serial.print(">>>> timestamp: ");
        Serial.println(_entries[_requestedIndex]->timestamp);

        Serial.print(">>>> bitmask: ");
        Serial.println(_entries[_requestedIndex]->bitmask);
#endif   

        // size
        data[offset + currentOffset] = size;        
        currentOffset += 1;

        // entry count
        // requestedEntry == index
        ui32_to_ui8 entryCount;
        entryCount.ui32 = entryCounter;
        memcpy(data + offset + currentOffset, entryCount.ui8, 4);
        currentOffset += 4;

        // timestamp
        ui32_to_ui8 secs;
        secs.ui32 = (_entries[_requestedIndex]->timestamp - HAPTime::refTime());
        memcpy(data + offset + currentOffset, secs.ui8, 4);
        currentOffset += 4;

        // bitmask
        data[offset + currentOffset] = _entries[_requestedIndex]->bitmask; 
        currentOffset += 1;

        // data includes all values
        memcpy(data + offset + currentOffset, _entries[_requestedIndex]->data, _entries[_requestedIndex]->length);
        currentOffset += _entries[_requestedIndex]->length;  // + 1 for bitmask !

#if HAP_DEBUG_FAKEGATO
        String t = "History Entry " + String(entryCounter);
        HAPHelper::array_print(t.c_str(), data + offset, currentOffset);
#endif   
        offset += currentOffset;
        _requestedIndex++;
        entryCounter++;
    }

#if HAP_DEBUG_FAKEGATO
    HAPHelper::array_print("History Entries", data, offset);
#endif        

#if defined(ARDUINO_ARCH_ESP32)
    // _s2r2Characteristics->setValue(base64::encode(data, offset));  
    return base64::encode(data, offset)
#elif defined(CORE_TEENSY)
    int encodedLen = base64_enc_len(offset);
    char encodedChr[encodedLen];
    base64_encode(encodedChr,(char*)data, offset);

    // _s2r2Characteristics->valueFromString(encodedChr);  
#endif

    _transfer = false;
    
    return String(encodedChr);    
}

void HAPFakegato::getRefTime(uint8_t* data, uint16_t* length){

#if HAP_DEBUG_FAKEGATO
    LogD(HAPTime::timeString() + " " + "HAPFakeGato" + "->" + String(__FUNCTION__) + " [   ] " + "Get ref time entry", true);    
#endif
    
    uint8_t offset = 0;
    uint8_t size = 21;
    uint8_t typ = HAP_FAKEGATO_TYPE_REFTIME;

    data[offset] = size;
    offset += 1;

    ui32_to_ui8 eC;
    eC.ui32 = _requestedIndex;
    memcpy(data + offset, eC.ui8, 4);
    offset += 4;

    ui32_to_ui8 secs;
    secs.ui32 = 0;
    memcpy(data + offset, secs.ui8, 4);
    offset += 4;

    data[offset] = typ;
    offset += 1;

    ui32_to_ui8 refTime;
    refTime.ui32 = HAPTime::refTime() - HAP_FAKEGATO_EPOCH;
    memcpy(data + offset, refTime.ui8, 4);
    offset += 4;

#if HAP_DEBUG_FAKEGATO
    LogD(">>>>> Fakegato RefTime: " + String( refTime.ui32 ), true);
#endif
    memset(data + offset, 0x00, 7); 
    offset += 7;               
    *length = offset;    
}


/**
 * @brief 
 *  In this write-only characteristics a time stamp is written by Eve.app every second if the accessory is selected in the app. 
 *  Format from https://gist.github.com/gomfunkel/b1a046d729757120907c#gistcomment-1841206:
 *  
 *  The current timestamp is in seconds since 1.1.2001 e.g.: 
 *  written value: cf1b521d -> reverse 1d521bcf -> hex2dec 491920335 
 *  Epoch Timestamp 1.1.2001 = 978307200; 978307200 + 491920335 = 1470227535 
 *  = Wed, 03 Aug 2016 12:32:15 GMT = 3.8.2016, 14:32:15 GMT+2:00 DST (MEST)
 *
 *  It is probably used to set time/date of the accessory.
 *
 * @param oldValue 
 * @param newValue 
 **/
void HAPFakegato::callbackHistorySetTime(String oldValue, String newValue){
    
    if (_isTimeSource){
            
        // "SPMZIw==" == 588903240
        size_t outputLength = 0;        
        mbedtls_base64_decode(NULL, 0, &outputLength, (uint8_t*)newValue.c_str(), newValue.length());
        uint8_t decoded[outputLength];


        mbedtls_base64_decode(decoded, sizeof(decoded), &outputLength,(uint8_t*)newValue.c_str(), newValue.length());

#if HAP_DEBUG_FAKEGATO 
        HAPHelper::array_print("History Set Time", decoded, outputLength);
#endif

        uint32_t timestamp = HAPHelper::u8_to_u32(decoded) + HAP_FAKEGATO_EPOCH;
        
        LogD(HAPTime::timeString() + " " + "HAPFakeGato" + "->" + String(__FUNCTION__) + " [   ] " + "Setting Time to: " + String(timestamp), true);    

        // 978307200 + 588903240 = 1567210440
        HAPTime::setTimeFromTimestamp(timestamp);        
    }      
}

/**
 * @brief 
 *  This write-only characteristic seem to control data flux from accessory to Eve.app. 
 *
 *  A typical value when reading from a fake Eve Weather accessory is 
 *    01140100 000000
 *   
 *   byte 1: ??
 *   byte 2: ??
 *   byte 3-6: Requested memory entry, based on the last entry that Eve.app downloaded. 
 *           If set to 0000, asks the accessory the start restart from the beginning of the memory
 *   byte 7-8: ??
 *
 * @param oldValue 
 * @param newValue 
 **/
#if defined(ARDUINO_TEENSY41)
FLASHMEM 
#endif
void HAPFakegato::callbackHistoryRequest(String oldValue, String newValue){
    LogD(HAPTime::timeString() + " " + "HAPFakeGato" + "->" + String(__FUNCTION__) + " [   ] " + "History Request for iid " + String(_historyRequest->iid()) +  " oldValue: " + oldValue + " -> newValue: " + newValue, true);    
    
    size_t outputLength = 0;   
    // Serial.println(newValue);

    mbedtls_base64_decode(NULL, 0, &outputLength, (uint8_t*)newValue.c_str(), newValue.length());
    uint8_t decoded[outputLength];
    mbedtls_base64_decode(decoded, sizeof(decoded), &outputLength, (uint8_t*)newValue.c_str(), newValue.length());    

#if HAP_DEBUG_FAKEGATO    
    HAPHelper::array_print("History Request", decoded, outputLength);
#endif

    ui32_to_ui8 requestedIndex;
    int n = 0;
    for (unsigned idx = 2; idx < 6; idx++) {
        requestedIndex.ui8[n++] = decoded[idx];
    }    

#if HAP_DEBUG_FAKEGATO    
    ui32_to_ui8 address;
    address.ui32 = __builtin_bswap32(requestedIndex.ui32);
    HAPHelper::array_print("History Request address",  address.ui8, 4);
#endif

    _requestedIndex = requestedIndex.ui32;
    _transfer = true;

#if HAP_DEBUG_FAKEGATO
    Serial.print("_requestedIndex: ");
    Serial.print(_requestedIndex);
#endif 
}



/**
 * @brief 
 *  E863F116-079E-48FF-8F27-9C2605A29F52 (tentative)
 *
 *  This read-only characteristic is used by the accessory to signal how many entries are in the log
 *  (and other infos). 
 *  Comparing this characteristics over different type of accessory, it was possible to obtain the 
 *  following partial decoding. 
 * 
 *  Data is composed by a fixed size portion (12 bytes) with info about time, 
 *  1 byte indicating the length of the following variable length portion with 
 *  accessory "signature" and finally a fixed length portion with info about memory status.
 *
 *   4 bytes: Actual time, in seconds from last time update
 *   4 bytes: negative offset of reference time
 *   4 bytes: reference time/last Accessory time update (taken from E863F117-079E-48FF-8F27-9C2605A29F52)
 *   1 byte: number of 16 bits word of the following "signature" portion
 *   2-12 bytes: variable length "signature"
 *   2 bytes: last physical memory position occupied (used by Eve.app to understand how many transfers are needed). If set to an address lower than the last successfully uploaded entry, forces Eve.app to start from the beginning of the memory, asking address 00 in E863F11C. Accessory answers with entry 01. Once the memory is fully written and memory overwriting is necessary this field remains equal to history size.
 *   2 bytes: history size
 *   4 bytes: once memory rolling occurred it indicates the address of the oldest entry present in memory (if memory rolling did not occur yet, these bytes are at 0)
 *   4 bytes:??
 *   2 bytes:?? always 01ff or 0101
 *
 *  Energy (working, tweeking from Room): 
 *   4 16bits word, "0102 0202 0702 0f03", 
 *
 *  example of full data 
 *   "58020000 00000000 cd8f0220 04 0102 0202 0702 0f03 0300 c00f 00000000 00000000 0101"
 * 
 *  // ToDo: Rewrite as READ callback for characteristic
 *           Check for base64 encoding - better use mbedtls if possible 
 **/     
String HAPFakegato::callbackGetHistoryInfo(){

    
    // ToDo: Signature for calculating buffersize and length !!!              
    uint8_t sigLength = signatureLength();
    uint8_t buffersize = (13 + sigLength + 14);

    uint8_t data[buffersize];
    uint8_t offset = 0;

    // evetime
    // Time from last update in seconds
    ui32_to_ui8 evetime;
    evetime.ui32 = (HAPTime::timestamp() - HAPTime::refTime());
    memcpy(data + offset, evetime.ui8, 4);
    offset += 4;    // == 4

    // negativOffset
    // Negativ offset of reference time
    ui32_to_ui8 negativOffset;
    negativOffset.ui32 = 0x00;
    memcpy(data + offset, negativOffset.ui8, 4);
    offset += 4;    // == 8

    // refTimeLastUpdate
    // reference time/last Accessory time update (taken from E863F117-079E-48FF-8F27-9C2605A29F52)
    ui32_to_ui8 refTimeLastUpdate;
    refTimeLastUpdate.ui32 = (HAPTime::refTime() - HAP_FAKEGATO_EPOCH);
    memcpy(data + offset, refTimeLastUpdate.ui8, 4);
    offset += 4;    // == 12

    // signature Length as word ( x / 2 )
    data[offset++] = (sigLength / 2);  // == 13

    // signature
    uint8_t signature[sigLength];
    getSignature(signature);

#if HAP_DEBUG_FAKEGATO   
    HAPHelper::array_print("signature", signature, sigLength);
#endif

    memcpy(data + offset, signature, sigLength);
    offset += sigLength;     // == 19

    // used Memory
    // last physical memory position occupied
    ui16_to_ui8 usedMemory;
    usedMemory.ui16 = _entries.size();
    memcpy(data + offset, usedMemory.ui8, 2);
    offset += 2;            // == 21

    // size
    // capacity of circular buffer
    ui16_to_ui8 memorySize;
    memorySize.ui16 = _entries.capacity;
    memcpy(data + offset, memorySize.ui8, 2);
    offset += 2;            // == 23


    // rolled over
    // once memory rolling occurred it indicates the address of the oldest entry present in memory
    // (if memory rolling did not occur yet, these bytes are at 0)
    ui32_to_ui8 rolledOver;
    if (_rolledOver){ 
        // ToDo: Check if that works ??
        rolledOver.ui32 = 0x01;
    } else {
        rolledOver.ui32 = 0x00;
    }
    memcpy(data + offset, rolledOver.ui8, 4);
    offset += 4;            // == 27

    ui32_to_ui8 spacer;        
    spacer.ui32 = 0x00;    
    memcpy(data + offset, spacer.ui8, 4);
    offset += 4;            // == 31

    ui16_to_ui8 endBit;
    endBit.ui16 = 0x0101;
    memcpy(data + offset, endBit.ui8, 2);
    offset += 2;            // == 33
    
#if HAP_DEBUG_FAKEGATO   
    HAPHelper::array_print("SET History Info", data, offset);
#endif

#if defined(ARDUINO_ARCH_ESP32)
    String encoded = base64::encode(data, 13 + _sigLength + 14);
#elif defined(CORE_TEENSY)
    
    int encodedLen = base64_enc_len(offset);
    char encodedChr[encodedLen];

     base64_encode(encodedChr, (char*)data, offset);     
#endif    
    //_historyInfo->setValue(encodedChr);  
    return String(encodedChr);
}


