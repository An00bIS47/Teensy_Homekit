// 
// HAPCharacteristicBase.hpp
// Teensy_Homekit
//
//  Created on: 20.02.2021
//      Author: michael
//

#ifndef HAPCHARACTERISTICBASE_HPP_
#define HAPCHARACTERISTICBASE_HPP_

#include <Arduino.h>
#include <functional>
#include <ArduinoJson.h>
#include "HAPGlobals.hpp"
#include "HAPHelper.hpp"


#ifndef CHAR_TYPE_NULL
#define CHAR_TYPE_NULL  0x00
#endif


enum {
	HAP_PERMISSION_READ             = 1,        // Paired read
    HAP_PERMISSION_WRITE            = 1 << 1,   // Paired write
    HAP_PERMISSION_NOTIFY           = 1 << 2,   // Notify/Events = Accessory will notice the controller
    HAP_PERMISSION_HIDDEN           = 1 << 3,   // Hidden 
    HAP_PERMISSION_ADDITIONAL_AUTH  = 1 << 4,   // This characteristic supports Additional Authorization  
    HAP_PERMISSION_TIMED_WRITE      = 1 << 5,   // This characteristic allows only timed write procedure
    HAP_PERMISSION_WRITE_RESPONSE   = 1 << 6    // This characteristic supports write response
};

typedef enum {
	HAP_UNIT_NONE = 0,          // none
	HAP_UNIT_CELSIUS,           // °C
	HAP_UNIT_PERCENTAGE,        // %
	HAP_UNIT_ARC_DEGREE,         // °
	HAP_UNIT_LUX,               // lux
	HAP_UNIT_SECONDS,           // sec

	HAP_UNIT_HPA,               // hPa

	HAP_UNIT_WATT,              // W
	HAP_UNIT_VOLTAGE,           // V	
    HAP_UNIT_KWH,               // kWh

	HAP_UNIT_KMH,               // km/h
	
    HAP_UNIT_KM,                // km
	HAP_UNIT_M,                 // m
	HAP_UNIT_MM,                // mm    
	
    HAP_UNIT_KELVIN,            // K
	
    HAP_UNIT_DU,                // DU
	
    HAP_UNIT_MIRED,             // Mired
	
    HAP_UNIT_PPM,               // ppm    
} HAP_UNIT;


// 
//  Base Characteristic
// 
// 
//  T == Available DataTypes
// 
//  Base Class:
// 
//  Numeric Base Class:
//       * float
//       * uint16_t
//       * uint32_t
//       * uint64_t
// 

class HAPCharacteristicBase {
public:

    HAPCharacteristicBase(uint8_t type, uint8_t permissions): _iid(0), _type(type), _permissions(permissions), _typeString(nullptr) {
#if HAP_ADD_DESC_TO_JSON		
        _desc = nullptr;
#endif 		
	}

	HAPCharacteristicBase(const char* typeString, uint8_t permissions): _iid(0), _type(CHAR_TYPE_NULL), _permissions(permissions), _typeString(typeString) {        
		       
		// typeString = new char[HAP_UUID_LENGTH + 1];
		// strncpy(typeString, _typeString, HAP_UUID_LENGTH + 1);
		// typeString[HAP_UUID_LENGTH] = '\0';
#if HAP_ADD_DESC_TO_JSON		
        _desc = nullptr;
#endif         

	} 

    virtual ~HAPCharacteristicBase() {
        //if (_typeString) delete[] _typeString;

#if HAP_ADD_DESC_TO_JSON		
        if (_desc) delete[] _desc;
#endif        

    }

    void setIID(uint32_t iid){ _iid = iid; }
    uint32_t iid() { return _iid; }


#if HAP_ADD_DESC_TO_JSON
	void setDescription(const char* str){
		if (_desc) delete[] _desc;

		_desc = new char[HAP_HOMEKIT_DEFAULT_STRING_LENGTH + 1];
		strncpy(_desc, str, HAP_HOMEKIT_DEFAULT_STRING_LENGTH + 1);
		_desc[strlen(str)] = '\0';
	}  

    void descriptionToJson(JsonObject& root){

        if ( (_desc) && (strcmp(_desc, "") != 0) ) {            
            root[F("description")] = _desc;            
        }
    }  
#endif

    bool readable()       { return _permissions & HAP_PERMISSION_READ;            }
	bool writable()       { return _permissions & HAP_PERMISSION_WRITE;           }
	bool notifiable()     { return _permissions & HAP_PERMISSION_NOTIFY;          }	
	bool hidden()         { return _permissions & HAP_PERMISSION_HIDDEN;          }
	bool timedWrite()     { return _permissions & HAP_PERMISSION_TIMED_WRITE;     }
	bool writeResponse()  { return _permissions & HAP_PERMISSION_WRITE_RESPONSE;  }
	bool additionalAuth() { return _permissions & HAP_PERMISSION_ADDITIONAL_AUTH; }

    void permissionsToJson(JsonObject& root){
        JsonArray permsArray = root.createNestedArray(F("perms"));
        if (writable())
            permsArray.add(F("pw"));
        if (readable())
            permsArray.add(F("pr"));
        if (notifiable())
            permsArray.add(F("ev"));  
        if (hidden())
            permsArray.add(F("hd"));
        if (timedWrite())
            permsArray.add(F("tw"));
        if (writeResponse())
            permsArray.add(F("wr"));
        if (additionalAuth())
            permsArray.add(F("aa"));
    }
    

    void addToJsonArray(JsonArray& array) {
        JsonObject root = array.createNestedObject();
	    toJson(root, true, true, false, true, true);
    }


	void toJson(JsonObject& root, bool type_ = false, bool perms = false, bool event = false, bool meta = false, bool addDesc = false) {
        
        root[F("iid")] = _iid;	       
	    
        valueToJson(root);

        if (meta){
            metaToJson(root);
        }
	
        if (perms){
            permissionsToJson(root);      
        }

        if (event) {
            root[F("ev")] = (uint8_t)notifiable();
        }
		
        if (type_){
            if (_type == CHAR_TYPE_NULL) {
                root[F("type")] = _typeString;
            } else {
                char hexType[5];
                sprintf(hexType, "%X", _type);
                root[F("type")] = hexType;
            }            
        }

#if HAP_ADD_DESC_TO_JSON
        if (addDesc) {      
            descriptionToJson(root);
        }  
#endif	
    }


	void printTo(Print& print) {
        DynamicJsonDocument doc(1024);
        JsonObject root = doc.to<JsonObject>();
        toJson(root, true, true, false, true, true);
        serializeJson(doc, print);
    }


    virtual void valueToJson(JsonObject& root) = 0;    
    virtual void metaToJson(JsonObject& root) = 0;

    static void unitToJson(JsonObject& root, HAP_UNIT unit) {
        switch (unit) {
            case HAP_UNIT_ARC_DEGREE:                
                root[F("unit")] = F("arcdegrees");          
            case HAP_UNIT_CELSIUS:                
                root[F("unit")] = F("celsius");                
            case HAP_UNIT_PERCENTAGE:
                root[F("unit")] = F("percentage");
            case HAP_UNIT_LUX:
                root[F("unit")] = F("lux");     
            case HAP_UNIT_SECONDS:
                root[F("unit")] = F("seconds");
            case HAP_UNIT_HPA:
                root[F("unit")] = F("hPa");
            case HAP_UNIT_WATT:
                root[F("unit")] = F("W");
            case HAP_UNIT_VOLTAGE:
                root[F("unit")] = F("V");
            case HAP_UNIT_KWH:
                root[F("unit")] = F("kWh");
            case HAP_UNIT_KMH:
                root[F("unit")] = F("km/h");
            case HAP_UNIT_KM:
                root[F("unit")] = F("km");
            case HAP_UNIT_M:
                root[F("unit")] = F("m");
            case HAP_UNIT_MM:
                root[F("unit")] = F("mm");
            case HAP_UNIT_KELVIN:
                root[F("unit")] = F("K");
            case HAP_UNIT_DU:
                root[F("unit")] = F("DU");            
            case HAP_UNIT_MIRED:
                root[F("unit")] = F("Mired");
            case HAP_UNIT_PPM:
                root[F("unit")] = F("ppm");
            case HAP_UNIT_NONE:
                root[F("unit")] = "";            
            default:
                root[F("unit")] = "";
        }
    }

    static const char* unitString(HAP_UNIT unit) {
        switch (unit) {
            case HAP_UNIT_ARC_DEGREE:                
                return "°";                
            case HAP_UNIT_CELSIUS:                
                return "°C";                    
            case HAP_UNIT_PERCENTAGE:
                return "%";
            case HAP_UNIT_LUX:
                return "lux";             
            case HAP_UNIT_SECONDS:
                return "seconds";
                
            case HAP_UNIT_HPA:
                return "hPa";
            case HAP_UNIT_WATT:
                return "W";
            case HAP_UNIT_VOLTAGE:
                return "V";
            case HAP_UNIT_KWH:
                return "kWh";
            case HAP_UNIT_KMH:
                return "km/h";
            case HAP_UNIT_KM:
                return "km";
            case HAP_UNIT_M:
                return "m";
            case HAP_UNIT_MM:
                return "mm";
            case HAP_UNIT_KELVIN:
                return "K";
            case HAP_UNIT_DU:
                return "DU";            
            case HAP_UNIT_MIRED:
                return "Mired";
            case HAP_UNIT_PPM:
                return "ppm";

            case HAP_UNIT_NONE:
                return "";                
            default:
                return "";
        }
    }


protected:

    uint32_t  _iid;
	const uint8_t   _type;
	const uint8_t   _permissions;	
	const char*     _typeString;    

#if HAP_ADD_DESC_TO_JSON
	char*           _desc;
#endif 

};




// 
//  Base Value Characteristic
// 
template <class T>
class HAPCharacteristicBaseValue : public HAPCharacteristicBase {

public:
    
    HAPCharacteristicBaseValue(uint8_t type, uint8_t permissions) : HAPCharacteristicBase(type, permissions) {
        
    }
    HAPCharacteristicBaseValue(const char* type, uint8_t permissions) : HAPCharacteristicBase(type, permissions) {
        
    }

    ~HAPCharacteristicBaseValue(){

    }

    virtual T value(bool withCallback = true){
        if (_valueGetFunctionCall && withCallback) {

            T callbackValue = _valueGetFunctionCall();

            if (callbackValue != _value){
                _valueChangeFunctionCall(_value, callbackValue);
                _value = callbackValue;
            }            
        }

        return _value;
    }

    virtual void setValue(const T& value, bool withCallback = true){
        if (_valueChangeFunctionCall && withCallback) {
            _valueChangeFunctionCall(_value, value);
        }

        _value = value;        
    } 

    virtual void valueToJson(JsonObject& root) = 0;
    virtual void metaToJson(JsonObject& root) = 0;

#if HAP_USE_STD_STRING
    virtual std::string valueString() = 0;

    virtual void valueFromString(const std::string& value){
        valueFromString(value.c_str());
    }    
#else
    virtual String valueString() = 0;
    
    virtual void valueFromString(const String& value){
        valueFromString(value.c_str());
    }
#endif

    virtual void valueFromString(const char* value) = 0; 


    void setValueGetCallback(std::function<T(void)> callback){
        _valueGetFunctionCall = callback;
    }

    void setValueChangeCallback(std::function<void(T, T)> callback){
        _valueChangeFunctionCall = callback;
    }

	void* operator new(size_t size)
    {
        Serial.printf(PSTR("Overloading new operator with size: %d\n"), size);
        //void * p = ::operator new(size);

#if defined(ARDUINO_TEENSY41)
		void* ptr = extmem_malloc(size);		
#else		
        void* ptr = malloc(size); // will also work fine
#endif     
        return ptr;
    }
 
    void operator delete(void* ptr)
    {
        Serial.println(F("Overloading delete operator"));
        
#if defined(ARDUINO_TEENSY41)
		extmem_free(ptr);
#else		
        free(ptr);
#endif 		
    }

protected:
    T _value;    
    std::function<T(void)>      _valueGetFunctionCall       = nullptr;
    std::function<void(T, T)>   _valueChangeFunctionCall    = nullptr;  
};



// 
//  Numeric Base Characteristic
// 
template <class T>
class HAPCharacteristicNumericBase : public HAPCharacteristicBaseValue<T> {

public:
    HAPCharacteristicNumericBase(uint8_t type, uint8_t permissions, T minVal, T maxVal, T step, HAP_UNIT unit) : HAPCharacteristicBaseValue<T>(type, permissions), _minVal(minVal), _maxVal(maxVal), _step(step), _unit(unit) {
        this->_value = minVal;
    }

    HAPCharacteristicNumericBase(const char* type, uint8_t permissions, T minVal, T maxVal, T step, HAP_UNIT unit) : HAPCharacteristicBaseValue<T>(type, permissions), _minVal(minVal), _maxVal(maxVal), _step(step), _unit(unit) {
        this->_value = minVal;
    }

    ~HAPCharacteristicNumericBase(){

    }

    virtual T value(bool withCallback = true) override {
        if (this->_valueGetFunctionCall && withCallback) {

            T callbackValue = this->_valueGetFunctionCall();

            if ((callbackValue != this->_value) && (callbackValue >= _minVal) && (callbackValue <= _maxVal)){
                this->_valueChangeFunctionCall(this->_value, callbackValue);
                this->_value = callbackValue;
            }
        }

            
        return this->_value;
    }

    virtual void setValue(const T& value, bool withCallback = true) override {

        if ((value != this->_value) && (value >= _minVal) && (value <= _maxVal)){
            if (this->_valueChangeFunctionCall && withCallback) {
                this->_valueChangeFunctionCall(this->_value, value);
            }

            this->_value = value;
        } 

    } 

    virtual void valueToJson(JsonObject& root) = 0;
    virtual void metaToJson(JsonObject& root)  = 0;

#if HAP_USE_STD_STRING
    virtual std::string valueString() = 0;
#else
    virtual String valueString() = 0;    
#endif



    virtual void valueFromString(const char* value) = 0;     
   
protected:        
    const T _minVal, _maxVal, _step;
    const HAP_UNIT _unit;
};




// 
//  Data Base Characteristic
// 
template <class T>
class HAPCharacteristicDataBase : public HAPCharacteristicBase {

public:
    HAPCharacteristicDataBase(uint8_t type, uint8_t permissions, size_t maxDataLen, const char* desc = "") : HAPCharacteristicBase(type, permissions), _value(nullptr), _maxDataLen(maxDataLen), _valueLen(0) {
        if (strcmp(desc, "") == 0){
            setDescription(desc);
        }

        _value = nullptr;
    }

    HAPCharacteristicDataBase(const char* type, uint8_t permissions, size_t maxDataLen, const char* desc = "") : HAPCharacteristicBase(type, permissions), _value(nullptr), _maxDataLen(maxDataLen), _valueLen(0) {
        if (strcmp(desc, "") == 0){
            setDescription(desc);
        }

        _value = nullptr;
    }

    ~HAPCharacteristicDataBase(){
        if (_value) delete[] _value;
    }

    virtual size_t setValueRaw(const T* data, const size_t dataLen) {

        if (dataLen > _maxDataLen) return 0;        
        
        if (memcmp(_value, data, dataLen) == 0) return dataLen;

        if (_value != nullptr) {
            delete[] _value;
        }

        _value = new T[dataLen + 1];
        _valueLen = dataLen;
        memcpy((T*)_value, data, dataLen);
        _value[_valueLen] = '\0'; 

        return dataLen;
    }


    virtual void setValue(const T* value, const size_t dataLen, bool withCallback = true){
        
        if (_valueChangeFunctionCall && withCallback) {
            _valueChangeFunctionCall(value, dataLen);                 
        } 

        setValueRaw(value, dataLen);               
    } 

    virtual void value(T* output, size_t* dataLen, bool withCallback = true){  

        if (_valueGetFunctionCall && withCallback){

            T bufferCallback[_maxDataLen];
            size_t valueLenCallback = 0;
            valueGetFunctionCall(bufferCallback, &valueLenCallback); 
            
            setValueRaw(bufferCallback, valueLenCallback);            
        }
        
        if (output){
            memcpy(output, _value, _valueLen);            
        }

        *dataLen = _valueLen;
    }

    virtual void valueToJson(JsonObject& root) = 0;
    virtual void metaToJson(JsonObject& root) = 0;

protected:        
    std::function<void(T*, size_t*)>  _valueGetFunctionCall       = nullptr;
    std::function<void(T*, size_t)>   _valueChangeFunctionCall    = nullptr;    

    
    const size_t _maxDataLen;
    size_t _valueLen;
    T* _value;
};




// 
//  Generic Characteristic
// 
template <class T>
class HAPCharacteristicT : public HAPCharacteristicBase {
public:



    virtual void valueToJson(JsonObject& root) = 0;
    virtual void metaToJson(JsonObject& root)  = 0;

#if HAP_USE_STD_STRING
    virtual std::string valueString() = 0;
#else
    virtual String valueString() = 0;
#endif
    virtual void valueFromString(const char* value) = 0;
};




// =========================================================================================================
// 
//  Specific Characteristics
// 
// =========================================================================================================

// 
// bool
// 
template <>
class HAPCharacteristicT<bool> : public HAPCharacteristicBaseValue<bool> {    
public:

    HAPCharacteristicT(uint8_t type, uint8_t permissions) : HAPCharacteristicBaseValue<bool>(type, permissions) {
        _value = false;    
    }
    HAPCharacteristicT(const char* type, uint8_t permissions) : HAPCharacteristicBaseValue<bool>(type, permissions) {
        _value = false;
    }

    void valueToJson(JsonObject& root) override {
        if (readable()) {
            root[F("value")] = (uint8_t)_value;     
        }
    }

    void metaToJson(JsonObject& root) override {
        root[F("format")] = F("bool");
    }


#if HAP_USE_STD_STRING
    std::string valueString() {
        return _value == 1 ? std::string("1") : std::string("0");
    }
#else
    String valueString(){
        return _value == 1 ? F("1") : F("0");
    }
#endif

    void valueFromString(const char* value) {
        if (strcmp(value, "1") == 0) {
            _value = true;
        } else if (strcmp(value, "0") == 0) {
            _value = false;
        }
    }

protected:
    
};

// 
// float
// 
template <>
class HAPCharacteristicT<float> : public HAPCharacteristicNumericBase<float> {    
public:
    HAPCharacteristicT(uint8_t type, uint8_t permissions, float minVal, float maxVal, float step, HAP_UNIT unit) : HAPCharacteristicNumericBase<float>(type, permissions, minVal, maxVal, step, unit) {
        this->_value = minVal;
    }

    HAPCharacteristicT(const char* type, uint8_t permissions, float minVal, float maxVal, float step, HAP_UNIT unit) : HAPCharacteristicNumericBase<float>(type, permissions, minVal, maxVal, step, unit) {
        this->_value = minVal;
    }

    void valueToJson(JsonObject& root) override {
        if (readable()) {
            root[F("value")] = HAPHelper::round2(_value);
        }                   
    }

    void metaToJson(JsonObject& root) override {
        root[F("minValue")] = HAPHelper::round2(_minVal);
        root[F("maxValue")] = HAPHelper::round2(_maxVal);
        root[F("minStep")]  = HAPHelper::round2(_step);

        if (_unit > HAP_UNIT_NONE) {
            unitToJson(root, _unit);
        }

        root[F("format")] = F("float"); 
    }


#if HAP_USE_STD_STRING
    std::string valueString() {
        char buffer[64] = {0,};                
        sprintf(buffer, "%.5lf", _value);        
        return std::string(buffer);
    }
#else
    String valueString(){
        return String(_value);
    }
#endif    

    void valueFromString(const char* value) {
        _value = atof(value);
    }
protected:

};



// 
// uint8_t
// 
template <>
class HAPCharacteristicT<uint8_t> : public HAPCharacteristicNumericBase<uint8_t> {    
public:

    HAPCharacteristicT(uint8_t type_, uint8_t permissions, uint8_t minVal, uint8_t maxVal, uint8_t step, HAP_UNIT charUnit, uint8_t validValuesSize, uint8_t validValues[]) : HAPCharacteristicNumericBase<uint8_t>(type_, permissions, minVal, maxVal, step, charUnit), _validValuesSize(validValuesSize) {
        _value = minVal;
        _iid = 0;

        if (validValuesSize > 0){
            _validValues = new uint8_t[validValuesSize];
            memcpy(_validValues, validValues, validValuesSize);            
        } else {
            _validValues = nullptr;
        }           
    }

    HAPCharacteristicT(const char* type_, uint8_t permissions, uint8_t minVal, uint8_t maxVal, uint8_t step, HAP_UNIT charUnit, uint8_t validValuesSize, uint8_t validValues[]) : HAPCharacteristicNumericBase<uint8_t>(type_, permissions, minVal, maxVal, step, charUnit), _validValuesSize(validValuesSize) {
        _value = minVal;
        _iid = 0;

        if (validValuesSize > 0){
            _validValues = new uint8_t[validValuesSize];
            memcpy(_validValues, validValues, validValuesSize);            
        } else {
            _validValues = nullptr;
        }           
    }

    ~HAPCharacteristicT(){
        if (_validValues) delete[] _validValues;
    }

    void valueToJson(JsonObject& root) override {
        if (readable()) {
            root[F("value")] = _value;    
        }
    }

    void metaToJson(JsonObject& root) override {
        root[F("minValue")] = _minVal;
        root[F("maxValue")] = _maxVal;
        root[F("minStep")]  = _step;
        
        if (_unit != HAP_UNIT_NONE) {
            unitToJson(root, _unit);                    
        } 

        if (_validValues != nullptr && _validValuesSize > 0){
            JsonArray validValuesJson = root.createNestedArray(F("valid-values"));
            for (int i = 0; i < _validValuesSize; i++){
                validValuesJson.add(_validValues[i]);
            }
        }

        root[F("format")] = F("uint8");
    }

#if HAP_USE_STD_STRING
    std::string valueString() {
        char temp[16];
        snprintf(temp, 16, "%" PRIu8, _value);
        return std::string(temp);
    }
#else
    String valueString(){
        return String(_value);
    }
#endif


    void valueFromString(const char* value) {
        _value = atoi(value);
    }

protected:
    uint8_t* _validValues;
    const uint8_t  _validValuesSize;
};




// 
// uint16_t
// 
template <>
class HAPCharacteristicT<uint16_t> : public HAPCharacteristicNumericBase<uint16_t> {    
public:

    HAPCharacteristicT(uint8_t type, uint8_t permissions, uint16_t minVal, uint16_t maxVal, uint16_t step, HAP_UNIT unit) : HAPCharacteristicNumericBase<uint16_t>(type, permissions, minVal, maxVal, step, unit) {
        this->_value = minVal;
    }

    HAPCharacteristicT(const char* type, uint8_t permissions, uint16_t minVal, uint16_t maxVal, uint16_t step, HAP_UNIT unit) : HAPCharacteristicNumericBase<uint16_t>(type, permissions, minVal, maxVal, step, unit) {
        this->_value = minVal;
    }


    void valueToJson(JsonObject& root) override {
        if (readable()) {
            root[F("value")] = _value;
        }                   
    }

    void metaToJson(JsonObject& root) override {
        root[F("minValue")] = _minVal;
        root[F("maxValue")] = _maxVal;
        root[F("minStep")]  = _step;

        if (_unit > HAP_UNIT_NONE) {
            unitToJson(root, _unit);
        }

        root[F("format")] = F("uint16"); 
    }


#if HAP_USE_STD_STRING
    std::string valueString() {
        char temp[32];
        snprintf(temp, 32, "%" PRIu16, _value);
        return std::string(temp);
    }
#else
    String valueString(){
        return String(_value);
    }
#endif     

    void valueFromString(const char* value) {
        _value = atoi(value);
    }

protected:

};

// 
// uint32_t
// 
template <>
class HAPCharacteristicT<uint32_t> : public HAPCharacteristicNumericBase<uint32_t> {    
public:

    HAPCharacteristicT(uint8_t type, uint8_t permissions, uint32_t minVal, uint32_t maxVal, uint32_t step, HAP_UNIT unit) : HAPCharacteristicNumericBase<uint32_t>(type, permissions, minVal, maxVal, step, unit) {
        this->_value = minVal;
    }

    HAPCharacteristicT(const char* type, uint8_t permissions, uint32_t minVal, uint32_t maxVal, uint32_t step, HAP_UNIT unit) : HAPCharacteristicNumericBase<uint32_t>(type, permissions, minVal, maxVal, step, unit) {
        this->_value = minVal;
    }

    void valueToJson(JsonObject& root) override {
        if (readable()) {
            root[F("value")] = _value;
        }                   
    }

    void metaToJson(JsonObject& root) override {
        root[F("minValue")] = _minVal;
        root[F("maxValue")] = _maxVal;
        root[F("minStep")]  = _step;

        if (_unit > HAP_UNIT_NONE) {
            unitToJson(root, _unit);
        }

        root[F("format")] = "uint32"; 
    }


#if HAP_USE_STD_STRING
    std::string valueString() {
        char temp[64];
        snprintf(temp, 64, "%" PRIu32, _value);
        return std::string(temp);
    }
#else
    String valueString(){
        return String(_value);
    }
#endif     

    void valueFromString(const char* value) {
        _value = atol(value);
    }

protected:

};


// 
// uint64_t
// 
template <>
class HAPCharacteristicT<uint64_t> : public HAPCharacteristicNumericBase<uint64_t> {    
public:

    HAPCharacteristicT(uint8_t type, uint8_t permissions, uint64_t minVal, uint64_t maxVal, uint64_t step, HAP_UNIT unit) : HAPCharacteristicNumericBase<uint64_t>(type, permissions, minVal, maxVal, step, unit) {
        this->_value = minVal;
    }

    HAPCharacteristicT(const char* type, uint8_t permissions, uint64_t minVal, uint64_t maxVal, uint64_t step, HAP_UNIT unit) : HAPCharacteristicNumericBase<uint64_t>(type, permissions, minVal, maxVal, step, unit) {
        this->_value = minVal;
    }

    void valueToJson(JsonObject& root) override {
        if (readable()) {
            root[F("value")] = _value;
        }                   
    }

    void metaToJson(JsonObject& root) override {
        root[F("minValue")] = _minVal;
        root[F("maxValue")] = _maxVal;
        root[F("minStep")]  = _step;

        if (_unit > HAP_UNIT_NONE) {
            unitToJson(root, _unit);
        }

        root[F("format")] = "uint64"; 
    }

#if HAP_USE_STD_STRING
    std::string valueString() {
        char temp[128];
        snprintf(temp, 128, "%" PRIu64, _value);
        return std::string(temp);
    }
#else
    String valueString(){
        char temp[128];
        snprintf(temp, 128, "%" PRIu64, _value);
        return String(temp);
    }
#endif     

    void valueFromString(const char* value) {
        _value = atoll(value);
    }

protected:

};


// 
// String
// 
template <>
class HAPCharacteristicT<String> : public HAPCharacteristicBaseValue<String> {    
public:

    HAPCharacteristicT(uint8_t type, uint8_t permissions, size_t maxlen = 64) : HAPCharacteristicBaseValue<String>(type, permissions) {
        _value = "";
        _maxLen = maxlen;
    }
    HAPCharacteristicT(const char* type, uint8_t permissions, size_t maxlen = 64) : HAPCharacteristicBaseValue<String>(type, permissions) {
        _value = "";
        _maxLen = maxlen;
    }

    void valueToJson(JsonObject& root) override {
        if (readable()) {
            root[F("value")] = _value;
        }                   
    }

    void metaToJson(JsonObject& root) override {
        root[F("maxLen")] = _maxLen;
        root[F("format")] = F("string"); 
    }

#if HAP_USE_STD_STRING
    std::string valueString() {
        return std::string(_value.c_str());
    }
#else
    String valueString(){
        return _value;
    }
#endif     

    void valueFromString(const char* value) {
        
        if (strlen(value) > _maxLen) return;
        
        _value = value;
    }

protected:
    size_t _maxLen = 64;
};


// 
// std::string
// 
template <>
class HAPCharacteristicT<std::string> : public HAPCharacteristicBaseValue<std::string> {    
public:

    HAPCharacteristicT(uint8_t type, uint8_t permissions, size_t maxlen) : HAPCharacteristicBaseValue<std::string>(type, permissions) {
        _value = "";
        _maxLen = maxlen;
    }
    HAPCharacteristicT(const char* type, uint8_t permissions, size_t maxlen) : HAPCharacteristicBaseValue<std::string>(type, permissions) {
        _value = "";
        _maxLen = maxlen;
    }

    void valueToJson(JsonObject& root) override {
        if (readable()) {
            root[F("value")] = _value;
        }                   
    }

    void metaToJson(JsonObject& root) override {
        root[F("maxLen")] = _maxLen;
        root[F("format")] = F("string"); 
    }

#if HAP_USE_STD_STRING
    std::string valueString() {
        return _value;
    }
#else
    String valueString(){
        return String(_value.c_str());
    }
#endif     

    void valueFromString(const char* value) {
        
        if (strlen(value) > _maxLen) return;
        
        _value = value;
    }

protected:
    size_t _maxLen = 64;
};


// ToDo: Add uint8_t*
// 
// uint8_t*
// 
template <>
class HAPCharacteristicT<uint8_t*> : public HAPCharacteristicDataBase<uint8_t*> {    
public:


    void valueToJson(JsonObject& root) override {
        if (readable()) {
            // root[F("value")] = (uint8_t)_value;     
        }
    }

    void metaToJson(JsonObject& root) override {
        root[F("format")] = F("data");
    }


#if HAP_USE_STD_STRING
    std::string valueString() {
        return "";
    }
#else
    String valueString(){
        return "";
    }
#endif

    void valueFromString(const char* value) {

    }

protected:
    size_t _maxLen = 512;
};


#endif /* HAPCHARACTERISTICBASE_HPP_ */