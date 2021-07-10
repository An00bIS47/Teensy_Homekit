//
// HAPCharacteristic.hpp
// Teensy_Homekit
//
//  Created on: 20.02.2021
//      Author: michael
//
//
// class HAPCharacteristicBase
//   |
//   +-> HAPCharacteristicData : public HAPCharacteristicBase
//   |    ]
//   |    +-> == uint8_t*
//   |
//   +-> template <class T> class HAPCharacteristic : public HAPCharacteristicBase
//   |
//   +-> template <class T> class HAPCharacteristicBaseValue : public HAPCharacteristicBase
//        |
//        +-> template <> class HAPCharacteristic<...> : public HAPCharacteristicBaseValue<...>
//        +-> ... bool
//        +-> ... String
//        +-> ... std::string
//        |
//        +-> template <class T> class HAPCharacteristicNumericBase : public HAPCharacteristicBaseValue<T>
//            |
//            +-> template <> class HAPCharacteristic<...> : public HAPCharacteristicNumericBase<...>
//            +-> ... float
//            +-> ... int
//            +-> ... int16_t
//            +-> ... int32_t
//            +-> ... uint8_t
//            +-> ... uint16_t
//            +-> ... uint32_t
//            +-> ... uint64_t
//



#ifndef HAPCHARACTERISTIC_HPP_
#define HAPCHARACTERISTIC_HPP_

#include <Arduino.h>
#include <functional>
#include <ArduinoJson.h>
#include <Base64.h>
#include "HAPCharacteristicType.hpp"


#ifndef HAP_ADD_DESC_TO_JSON
#define HAP_ADD_DESC_TO_JSON 1
#endif

#ifndef HAP_HOMEKIT_DEFAULT_STRING_LENGTH
#define HAP_HOMEKIT_DEFAULT_STRING_LENGTH   64
#endif

#ifndef HAP_USE_STD_STRING
#define HAP_USE_STD_STRING 1
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

enum class HAPUnit {
    None      = 0x00,   // None
    Celsius,            // °C
    Percentage,         // %
    ArcDegree,          // ° 
    Lux,                // lux
    Seconds,            // sec
    HPA,                // hPa
    Watt,               // W
    Voltage,            // V
    KWh,                // kWh
    KMh,                // km/h
    
    KM,                 // kilometer
    M,                  // meter
    MM,                 // millimeter
    
    Kelvin,             // Kelvin
    DU,                 // DU
    Mired,              // mired
    PPM                 // ppm
};

// typedef enum {
// 	HAP_UNIT_NONE = 0,          // none
// 	HAP_UNIT_CELSIUS,           // °C
// 	HAP_UNIT_PERCENTAGE,        // %
// 	HAP_UNIT_ARC_DEGREE,        // °
// 	HAP_UNIT_LUX,               // lux
// 	HAP_UNIT_SECONDS,           // sec

// 	HAP_UNIT_HPA,               // hPa

// 	HAP_UNIT_WATT,              // W
// 	HAP_UNIT_VOLTAGE,           // V
//     HAP_UNIT_KWH,               // kWh

// 	HAP_UNIT_KMH,               // km/h

//     HAP_UNIT_KM,                // km
// 	HAP_UNIT_M,                 // m
// 	HAP_UNIT_MM,                // mm

//     HAP_UNIT_KELVIN,            // K

//     HAP_UNIT_DU,                // DU

//     HAP_UNIT_MIRED,             // Mired

//     HAP_UNIT_PPM,               // ppm
// } HAP_UNIT;

enum class HAPBatteryLevel : uint8_t {
	Normal = 0x00,
	Low    = 0x01
};


enum class HAPChargingState : uint8_t {
	NotCharging  = 0x00,
	Charging     = 0x01,
	NotChargable = 0x02
};


// =========================================================================================================
//
//  Base Characteristic
//
// =========================================================================================================
class HAPCharacteristicBase {
public:

    HAPCharacteristicBase(HAPCharacteristicType type, uint8_t permissions): _iid(0), _type(type), _permissions(permissions), _typeString(nullptr) {
#if HAP_ADD_DESC_TO_JSON
        _desc = nullptr;
#endif
	}

	HAPCharacteristicBase(const char* typeString, uint8_t permissions): _iid(0), _type(HAPCharacteristicType::None), _permissions(permissions), _typeString(typeString) {

		// typeString = new char[HAP_UUID_LENGTH + 1];
		// strncpy(typeString, _typeString, HAP_UUID_LENGTH + 1);
		// typeString[HAP_UUID_LENGTH] = '\0';
#if HAP_ADD_DESC_TO_JSON
        _desc = nullptr;
#endif

	}


    // std::function<void(void)> valueGetFunctionCall = nullptr;

    virtual ~HAPCharacteristicBase() {
        //if (_typeString) delete[] _typeString;

#if HAP_ADD_DESC_TO_JSON
        if (_desc) delete[] _desc;
#endif

    }

    void setIID(uint32_t iid){ _iid = iid; }
    uint32_t iid() { return _iid; }


    uint16_t typeAsInt() {
        return static_cast<uint16_t>(_type);
    }

    HAPCharacteristicType type(){
        return _type;
    }

    const char* typeString(){
        return _typeString;
    }

#if HAP_ADD_DESC_TO_JSON
	void setDescription(const char* str){
		if (_desc) delete[] _desc;

		_desc = new char[HAP_HOMEKIT_DEFAULT_STRING_LENGTH];
		strncpy(_desc, str, HAP_HOMEKIT_DEFAULT_STRING_LENGTH);
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


	void toJson(JsonObject& root, bool type_ = false, bool perms = false, bool event = false, bool meta = false, bool addDesc = false, bool withCallback = true) {

        root[F("iid")] = _iid;

        valueToJson(root, withCallback);

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
            if (_type == HAPCharacteristicType::None) {
                root[F("type")] = _typeString;
            } else {
                char hexType[5];
                sprintf(hexType, "%X", static_cast<uint16_t>(_type));
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


    virtual void valueToJson(JsonObject& root, bool withCallback = true) = 0;
    virtual void metaToJson(JsonObject& root) = 0;

    static void unitToJson(JsonObject& root, HAPUnit unit) {
        switch (unit) {
            case HAPUnit::ArcDegree:
                root[F("unit")] = F("arcdegrees");
            case HAPUnit::Celsius:
                root[F("unit")] = F("celsius");
            case HAPUnit::Percentage:
                root[F("unit")] = F("percentage");
            case HAPUnit::Lux:
                root[F("unit")] = F("lux");
            case HAPUnit::Seconds:
                root[F("unit")] = F("seconds");
            case HAPUnit::HPA:
                root[F("unit")] = F("hPa");
            case HAPUnit::Watt:
                root[F("unit")] = F("W");
            case HAPUnit::Voltage:
                root[F("unit")] = F("V");
            case HAPUnit::KWh:
                root[F("unit")] = F("kWh");
            case HAPUnit::KMh:
                root[F("unit")] = F("km/h");
            case HAPUnit::KM:
                root[F("unit")] = F("km");
            case HAPUnit::M:
                root[F("unit")] = F("m");
            case HAPUnit::MM:
                root[F("unit")] = F("mm");
            case HAPUnit::Kelvin:
                root[F("unit")] = F("K");
            case HAPUnit::DU:
                root[F("unit")] = F("DU");
            case HAPUnit::Mired:
                root[F("unit")] = F("Mired");
            case HAPUnit::PPM:
                root[F("unit")] = F("ppm");
            case HAPUnit::None:
                root[F("unit")] = "";
            default:
                root[F("unit")] = "";
        }
    }

    static const char* unitString(HAPUnit unit) {
        switch (unit) {
            case HAPUnit::ArcDegree:
                return "°";
            case HAPUnit::Celsius:
                return "°C";
            case HAPUnit::Percentage:
                return "%";
            case HAPUnit::Lux:
                return "lux";
            case HAPUnit::Seconds:
                return "seconds";

            case HAPUnit::HPA:
                return "hPa";
            case HAPUnit::Watt:
                return "W";
            case HAPUnit::Voltage:
                return "V";
            case HAPUnit::KWh:
                return "kWh";
            case HAPUnit::KMh:
                return "km/h";
            case HAPUnit::KM:
                return "km";
            case HAPUnit::M:
                return "m";
            case HAPUnit::MM:
                return "mm";
            case HAPUnit::Kelvin:
                return "K";
            case HAPUnit::DU:
                return "DU";
            case HAPUnit::Mired:
                return "Mired";
            case HAPUnit::PPM:
                return "ppm";
            case HAPUnit::None:
                return "";
            default:
                return "";
        }
    }

#if HAP_USE_STD_STRING
    virtual std::string valueString() = 0;

    virtual void valueFromString(const std::string& value){
        valueFromString(value.c_str());
    }
#else
    virtual String valueString() = 0;

    virtual void valueFromString(const String& val){
        valueFromString(val.c_str());
    }
#endif

    virtual void valueFromString(const char* val) = 0;
    // virtual void valueString(char* val, size_t* length) = 0;

protected:

    uint32_t        _iid = 0;
	const HAPCharacteristicType   _type;
	const uint8_t   _permissions;
	const char*     _typeString;

#if HAP_ADD_DESC_TO_JSON
	char*           _desc = nullptr;
#endif

    double round2(double val) {
   		return (int)(val * 100) / 100.0;
	}

};



// =========================================================================================================
//
//  Base Value Characteristic
//
// =========================================================================================================
//
template <class T>
class HAPCharacteristicBaseValue : public HAPCharacteristicBase {

public:

    HAPCharacteristicBaseValue(HAPCharacteristicType type, uint8_t permissions) : HAPCharacteristicBase(type, permissions) {

    }

    HAPCharacteristicBaseValue(const char* type, uint8_t permissions) : HAPCharacteristicBase(type, permissions) {

    }

    ~HAPCharacteristicBaseValue(){

    }

    virtual T value(bool withCallback = true){
        if (_valueGetFunctionCall && withCallback) {

            T callbackValue = _valueGetFunctionCall();

            if (callbackValue != _value){
                if (_valueChangeFunctionCall != nullptr) _valueChangeFunctionCall(_value, callbackValue);
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

    virtual void valueToJson(JsonObject& root, bool withCallback = true) = 0;
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

    virtual void valueFromString(const char* val) = 0;
    // virtual void valueString(char* val, size_t* length) = 0;


    void setValueGetCallback(std::function<T(void)> callback){
        _valueGetFunctionCall = callback;
    }

    void setValueChangeCallback(std::function<void(T, T)> callback){
        _valueChangeFunctionCall = callback;
    }

// 	void* operator new(size_t size)
//     {
//         Serial.printf(PSTR("Overloading new operator with size: %d\n"), size);
//         //void * p = ::operator new(size);

// #if defined(ARDUINO_TEENSY41)
// 		void* ptr = extmem_malloc(size);
// #else
//         void* ptr = malloc(size); // will also work fine
// #endif
//         return ptr;
//     }

//     void operator delete(void* ptr)
//     {
//         Serial.println(F("Overloading delete operator"));

// #if defined(ARDUINO_TEENSY41)
// 		extmem_free(ptr);
// #else
//         free(ptr);
// #endif
//     }

protected:
    T _value;
    std::function<T(void)>      _valueGetFunctionCall       = nullptr;
    std::function<void(T, T)>   _valueChangeFunctionCall    = nullptr;
};


// =========================================================================================================
//
//  Numeric Base Characteristic
//
// =========================================================================================================
//
template <class T>
class HAPCharacteristicNumericBase : public HAPCharacteristicBaseValue<T> {

public:
    HAPCharacteristicNumericBase(HAPCharacteristicType type, uint8_t permissions, T minVal, T maxVal, T step, HAPUnit unit) : HAPCharacteristicBaseValue<T>(type, permissions), _minVal(minVal), _maxVal(maxVal), _step(step), _unit(unit) {
        this->_value = minVal;
    }

    HAPCharacteristicNumericBase(const char* type, uint8_t permissions, T minVal, T maxVal, T step, HAPUnit unit) : HAPCharacteristicBaseValue<T>(type, permissions), _minVal(minVal), _maxVal(maxVal), _step(step), _unit(unit) {
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
            if (this->_valueChangeFunctionCall && withCallback == true) {
                this->_valueChangeFunctionCall(this->_value, value);
            }
        }

        this->_value = value;

    }

    virtual void valueToJson(JsonObject& root, bool withCallback = true) = 0;
    virtual void metaToJson(JsonObject& root)  = 0;

#if HAP_USE_STD_STRING
    virtual std::string valueString() = 0;
#else
    virtual String valueString() = 0;
#endif


    // virtual void valueString(char* val, size_t* length) = 0;
    virtual void valueFromString(const char* val) = 0;

protected:
    const T _minVal, _maxVal, _step;
    const HAPUnit _unit;
};


// =========================================================================================================
//
//  Generic HAPCharacteristic
//
// =========================================================================================================
template <class T>
class HAPCharacteristic : public HAPCharacteristicBase {
public:

    virtual void valueToJson(JsonObject& root, bool withCallback = true) = 0;
    virtual void metaToJson(JsonObject& root)  = 0;

#if HAP_USE_STD_STRING
    virtual std::string valueString() = 0;
#else
    virtual String valueString() = 0;
#endif
    virtual void valueFromString(const char* val) = 0;
    // virtual void valueString(char* val, size_t* length) = 0;
};



// =========================================================================================================
//
//  Data Characteristic
//
// =========================================================================================================
class HAPCharacteristicData : public HAPCharacteristicBase {

public:
    HAPCharacteristicData(HAPCharacteristicType type, uint8_t permissions, size_t maxDataLen, const char* desc = "") : HAPCharacteristicBase(type, permissions), _value(nullptr), _valueLen(0), _maxDataLen(maxDataLen) {
        if (strcmp(desc, "") == 0){
            setDescription(desc);
        }

        _value = nullptr;
    }

    HAPCharacteristicData(const char* type, uint8_t permissions, size_t maxDataLen, const char* desc = "") : HAPCharacteristicBase(type, permissions), _value(nullptr), _valueLen(0), _maxDataLen(maxDataLen) {
        if (strcmp(desc, "") == 0){
            setDescription(desc);
        }

        _value = nullptr;
    }

    ~HAPCharacteristicData(){
        if (_value) free(_value);
    }

    virtual size_t setValueRaw(const uint8_t* data, const size_t dataLen) {

        if (dataLen > _maxDataLen) return 0;



        if (_value != nullptr) {
            if (memcmp(_value, data, dataLen) == 0) return dataLen;

            delete[] _value;
        }

        _value = (uint8_t*) malloc(sizeof(uint8_t) * dataLen + 1);
        _valueLen = dataLen;
        memcpy(_value, data, dataLen);
        _value[_valueLen] = '\0';

        return dataLen;
    }


    virtual void setValue(const uint8_t* value, const size_t dataLen, bool withCallback = true){

        if (_dataChangeFunctionCall && withCallback) {
            _dataChangeFunctionCall(value, dataLen);
        }

        setValueRaw(value, dataLen);
    }

    virtual void value(uint8_t* output, size_t* dataLen, bool withCallback = true){

        if (_dataGetFunctionCall && withCallback){

            uint8_t bufferCallback[_maxDataLen] = {0,};
            size_t valueLenCallback = 0;
            _dataGetFunctionCall(bufferCallback, &valueLenCallback);
            setValueRaw(bufferCallback, valueLenCallback);
        }

        if (output){
            memcpy(output, _value, _valueLen);
        }

        *dataLen = _valueLen;
    }
    

    virtual void valueToJson(JsonObject& root, bool withCallback = true) override {
        if (readable()) {
            if (withCallback && _dataGetFunctionCall){
                size_t dataLen = 0;
                value(nullptr, &dataLen, true);
            }

            size_t b64Len = base64_enc_len(_valueLen);
            char b64String[b64Len + 1];
            base64_encode(b64String, (char*)_value, _valueLen);
            b64String[b64Len] = '\0';

            root[F("value")] = b64String;

        }
    }


    /**
     * @brief set value from base64 decoded string
     *
     * @param val base64 decoded string
     */
    void valueFromString(const char* val) {
        // ToDo: decode base64
        size_t outputLength = base64_dec_len((char*)val, strlen(val));

        if (outputLength > _maxDataLen) return;

        uint8_t decoded[outputLength];
        base64_decode((char*)decoded, (char*)val, strlen(val));

        setValue(decoded, outputLength, true);
    }


    virtual void metaToJson(JsonObject& root) {
        root[F("format")] = F("data");
    }

    void setDataGetCallback(std::function<void(uint8_t*, size_t*)> callback){
        _dataGetFunctionCall = callback;
    }

    void setDataChangeCallback(std::function<void(const uint8_t*, const size_t)> callback){
        _dataChangeFunctionCall = callback;
    }

#if HAP_USE_STD_STRING
    /**
     * @brief get the value as base64 encoded std::string
     *
     * @return base64 encoded std::string
     */
    std::string valueString() {
        int b64Len = base64_enc_len(_valueLen);

        if (b64Len < 0) return "AA==";

        char b64String[b64Len + 1];
        base64_encode(b64String, (char*)_value, _valueLen);
        b64String[b64Len] = '\0';

        return std::string(b64String);
    }
#else

    /**
     * @brief get the value as base64 encoded String
     *
     * @return base64 encoded String
     */
    String valueString(){
        int b64Len = base64_enc_len(_valueLen);

        if (b64Len < 0) return "AA==";

        char b64String[b64Len + 1];
        base64_encode(b64String, (char*)_value, _valueLen);
        b64String[b64Len] = '\0';

        return String(b64String);
    }
#endif


protected:
    std::function<void(uint8_t*, size_t*)>  _dataGetFunctionCall       = nullptr;
    std::function<void(const uint8_t*, const size_t)>   _dataChangeFunctionCall    = nullptr;

    uint8_t* _value = nullptr;
    size_t _valueLen = 0;
    size_t _maxDataLen = 0;
};




// =========================================================================================================
//
//  Specific Characteristics
//
// =========================================================================================================


// ==========================================================
//
// bool
//
// ==========================================================
template <>
class HAPCharacteristic<bool> : public HAPCharacteristicBaseValue<bool> {
public:

    HAPCharacteristic(HAPCharacteristicType type, uint8_t permissions) : HAPCharacteristicBaseValue<bool>(type, permissions) {
        _value = false;
    }
    HAPCharacteristic(const char* type, uint8_t permissions) : HAPCharacteristicBaseValue<bool>(type, permissions) {
        _value = false;
    }

    void valueToJson(JsonObject& root, bool withCallback = true) override {
        if (readable()) {
            root[F("value")] = (uint8_t) value(withCallback);
        }
    }

    void metaToJson(JsonObject& root) override {
        root[F("format")] = F("bool");
    }


#if HAP_USE_STD_STRING
    std::string valueString() {
        return value() == 1 ? std::string("1") : std::string("0");
    }
#else
    String valueString(){
        return value() == 1 ? F("1") : F("0");
    }
#endif

    void valueFromString(const char* val) {
        if (strcmp(val, "1") == 0) {
            setValue(true);
        } else if (strcmp(val, "0") == 0) {
            setValue(false);
        }
    }

    // void valueString(char* val, size_t* length, bool withCallback = false){
    //     if (val){
    //        memcpy(val, value(withCallback) == true ? "1" : "0", 1);
    //        val[1] = '\0';
    //     }
    //     *length = 2;
    // }




protected:

};

//
// float
//
template <>
class HAPCharacteristic<float> : public HAPCharacteristicNumericBase<float> {
public:
    HAPCharacteristic(HAPCharacteristicType type, uint8_t permissions, float minVal, float maxVal, float step, HAPUnit unit) : HAPCharacteristicNumericBase<float>(type, permissions, minVal, maxVal, step, unit) {
        this->_value = minVal;
    }

    HAPCharacteristic(const char* type, uint8_t permissions, float minVal, float maxVal, float step, HAPUnit unit) : HAPCharacteristicNumericBase<float>(type, permissions, minVal, maxVal, step, unit) {
        this->_value = minVal;
    }

    void valueToJson(JsonObject& root, bool withCallback = true) override {
        if (readable()) {
            root[F("value")] = round2(value(withCallback));
        }
    }

    void metaToJson(JsonObject& root) override {
        root[F("minValue")] = round2(_minVal);
        root[F("maxValue")] = round2(_maxVal);
        root[F("minStep")]  = round2(_step);

        if (_unit != HAPUnit::None) {
            unitToJson(root, _unit);
        }

        root[F("format")] = F("float");
    }


#if HAP_USE_STD_STRING
    std::string valueString() {
        char buffer[64] = {0,};
        sprintf(buffer, "%.2lf", value());
        return std::string(buffer);
    }
#else
    String valueString(){
        return String(value());
    }
#endif

    void valueFromString(const char* val) {
        setValue(atof(val));
    }

    // void valueString(char* val, size_t* length, bool withCallback = false){
    //     char buffer[64] = {'\0', };
    //     int ret = snprintf(buffer, 64, "%.2lf", value(withCallback));

    //     if (ret < 0){
    //         *length = 0;
    //         return;
    //     }

    //     *length = ret;

    //     if (value){
    //         snprintf(value, strlen(buffer), "%.2lf", value(false));
    //     }
    // }
protected:

};



//
// int
//
template <>
class HAPCharacteristic<int> : public HAPCharacteristicNumericBase<int> {
public:
    HAPCharacteristic(HAPCharacteristicType type, uint8_t permissions, int minVal, int maxVal, int step, HAPUnit unit) : HAPCharacteristicNumericBase<int>(type, permissions, minVal, maxVal, step, unit) {
        this->_value = minVal;
    }

    HAPCharacteristic(const char* type, uint8_t permissions, int minVal, int maxVal, int step, HAPUnit unit) : HAPCharacteristicNumericBase<int>(type, permissions, minVal, maxVal, step, unit) {
        this->_value = minVal;
    }

    void valueToJson(JsonObject& root, bool withCallback = true) override {
        if (readable()) {
            root[F("value")] = value(withCallback);
        }
    }

    void metaToJson(JsonObject& root) override {
        root[F("minValue")] = _minVal;
        root[F("maxValue")] = _maxVal;
        root[F("minStep")]  = _step;

        if (_unit != HAPUnit::None) {
            unitToJson(root, _unit);
        }

        root[F("format")] = F("int");
    }


#if HAP_USE_STD_STRING
    std::string valueString() {
        char buffer[64] = {0,};
        sprintf(buffer, "%i", value());
        return std::string(buffer);
    }
#else
    String valueString(){
        return String(value());
    }
#endif

    void valueFromString(const char* val) {
        setValue(atoi(val));
    }

    // void valueString(char* val, size_t* length, bool withCallback = false){
    //     char buffer[64] = {'\0', };
    //     int ret = snprintf(buffer, 64, "%i", value(withCallback));

    //     if (ret < 0){
    //         *length = 0;
    //         return;
    //     }

    //     *length = ret;

    //     if (value){
    //         snprintf(value, strlen(buffer), "%i", value(false));
    //     }
    // }
protected:

};


//
// int16_t
//
template <>
class HAPCharacteristic<int16_t> : public HAPCharacteristicNumericBase<int16_t> {
public:
    HAPCharacteristic(HAPCharacteristicType type, uint8_t permissions, int16_t minVal, int16_t maxVal, int16_t step, HAPUnit unit) : HAPCharacteristicNumericBase<int16_t>(type, permissions, minVal, maxVal, step, unit) {
        this->_value = minVal;
    }

    HAPCharacteristic(const char* type, uint8_t permissions, int16_t minVal, int16_t maxVal, int16_t step, HAPUnit unit) : HAPCharacteristicNumericBase<int16_t>(type, permissions, minVal, maxVal, step, unit) {
        this->_value = minVal;
    }

    void valueToJson(JsonObject& root, bool withCallback = true) override {
        if (readable()) {
            root[F("value")] = value(withCallback);
        }
    }

    void metaToJson(JsonObject& root) override {
        root[F("minValue")] = _minVal;
        root[F("maxValue")] = _maxVal;
        root[F("minStep")]  = _step;

        if (_unit != HAPUnit::None) {
            unitToJson(root, _unit);
        }

        root[F("format")] = F("int");
    }


#if HAP_USE_STD_STRING
    std::string valueString() {
        char buffer[64] = {0,};
        sprintf(buffer, "%" PRIi16, value());
        return std::string(buffer);
    }
#else
    String valueString(){
        return String(value());
    }
#endif

    void valueFromString(const char* val) {
        setValue(atoi(val));
    }

    // void valueString(char* val, size_t* length, bool withCallback = false){
    //     char buffer[64] = {'\0', };
    //     int ret = snprintf(buffer, 64, "%" PRIi16, value(withCallback));

    //     if (ret < 0){
    //         *length = 0;
    //         return;
    //     }

    //     *length = ret;

    //     if (value){
    //         snprintf(value, strlen(buffer), "%" PRIi16, value(false));
    //     }
    // }
protected:

};


//
// int32_t
//
template <>
class HAPCharacteristic<int32_t> : public HAPCharacteristicNumericBase<int32_t> {
public:
    HAPCharacteristic(HAPCharacteristicType type, uint8_t permissions, int32_t minVal, int32_t maxVal, int32_t step, HAPUnit unit) : HAPCharacteristicNumericBase<int32_t>(type, permissions, minVal, maxVal, step, unit) {
        this->_value = minVal;
    }

    HAPCharacteristic(const char* type, uint8_t permissions, int32_t minVal, int32_t maxVal, int32_t step, HAPUnit unit) : HAPCharacteristicNumericBase<int32_t>(type, permissions, minVal, maxVal, step, unit) {
        this->_value = minVal;
    }

    void valueToJson(JsonObject& root, bool withCallback = true) override {
        if (readable()) {
            root[F("value")] = value(withCallback);
        }
    }

    void metaToJson(JsonObject& root) override {
        root[F("minValue")] = _minVal;
        root[F("maxValue")] = _maxVal;
        root[F("minStep")]  = _step;

        if (_unit != HAPUnit::None) {
            unitToJson(root, _unit);
        }

        root[F("format")] = F("int");
    }


#if HAP_USE_STD_STRING
    std::string valueString() {
        char buffer[64] = {0,};
        sprintf(buffer, "%" PRIi32, value());
        return std::string(buffer);
    }
#else
    String valueString(){
        return String(value());
    }
#endif

    void valueFromString(const char* val) {
        setValue(atoi(val));
    }

    // void valueString(char* val, size_t* length, bool withCallback = false){
    //     char buffer[64] = {'\0', };
    //     int ret = snprintf(buffer, 64, "%" PRIi32, value(withCallback));

    //     if (ret < 0){
    //         *length = 0;
    //         return;
    //     }

    //     *length = ret;

    //     if (value){
    //         snprintf(value, strlen(buffer), "%" PRIi32, value(false));
    //     }
    // }
protected:

};



//
// uint8_t
//
template <>
class HAPCharacteristic<uint8_t> : public HAPCharacteristicNumericBase<uint8_t> {
public:

    HAPCharacteristic(HAPCharacteristicType type_, uint8_t permissions, uint8_t minVal, uint8_t maxVal, uint8_t step, HAPUnit charUnit, uint8_t validValuesSize, uint8_t validValues[]) : HAPCharacteristicNumericBase<uint8_t>(type_, permissions, minVal, maxVal, step, charUnit), _validValuesSize(validValuesSize) {
        _value = minVal;
        _iid = 0;

        if (validValuesSize > 0){
            _validValues = new uint8_t[validValuesSize];
            memcpy(_validValues, validValues, validValuesSize);
        } else {
            _validValues = nullptr;
        }
    }

    HAPCharacteristic(const char* type_, uint8_t permissions, uint8_t minVal, uint8_t maxVal, uint8_t step, HAPUnit charUnit, uint8_t validValuesSize, uint8_t validValues[]) : HAPCharacteristicNumericBase<uint8_t>(type_, permissions, minVal, maxVal, step, charUnit), _validValuesSize(validValuesSize) {
        _value = minVal;
        _iid = 0;

        if (validValuesSize > 0){
            _validValues = new uint8_t[validValuesSize];
            memcpy(_validValues, validValues, validValuesSize);
        } else {
            _validValues = nullptr;
        }
    }

    ~HAPCharacteristic(){
        if (_validValues) delete[] _validValues;
    }

    void valueToJson(JsonObject& root, bool withCallback = true) override {
        if (readable()) {
            root[F("value")] = value(withCallback);
        }
    }

    void metaToJson(JsonObject& root) override {
        root[F("minValue")] = _minVal;
        root[F("maxValue")] = _maxVal;
        root[F("minStep")]  = _step;

        if (_unit != HAPUnit::None) {
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
        snprintf(temp, 16, "%" PRIu8, value());
        return std::string(temp);
    }
#else
    String valueString(){
        return String(value());
    }
#endif


    void valueFromString(const char* val) {
        setValue(atoi(val));
    }

    // void valueString(char* val, size_t* length, bool withCallback = false){
    //     char buffer[5] = {'\0', };
    //     int ret = snprintf(buffer, 5, "%" PRIu8, value(withCallback));

    //     if (ret < 0){
    //         *length = 0;
    //         return;
    //     }

    //     *length = ret;

    //     if (val){
    //         snprintf(val, strlen(buffer), "%" PRIu8, value(false));
    //     }
    // }

protected:
    uint8_t* _validValues;
    const uint8_t  _validValuesSize;
};




//
// uint16_t
//
template <>
class HAPCharacteristic<uint16_t> : public HAPCharacteristicNumericBase<uint16_t> {
public:

    HAPCharacteristic(HAPCharacteristicType type, uint8_t permissions, uint16_t minVal, uint16_t maxVal, uint16_t step, HAPUnit unit) : HAPCharacteristicNumericBase<uint16_t>(type, permissions, minVal, maxVal, step, unit) {
        this->_value = minVal;
    }

    HAPCharacteristic(const char* type, uint8_t permissions, uint16_t minVal, uint16_t maxVal, uint16_t step, HAPUnit unit) : HAPCharacteristicNumericBase<uint16_t>(type, permissions, minVal, maxVal, step, unit) {
        this->_value = minVal;
    }


    void valueToJson(JsonObject& root, bool withCallback = true) override {
        if (readable()) {
            root[F("value")] = value(withCallback);
        }
    }

    void metaToJson(JsonObject& root) override {
        root[F("minValue")] = _minVal;
        root[F("maxValue")] = _maxVal;
        root[F("minStep")]  = _step;

        if (_unit != HAPUnit::None) {
            unitToJson(root, _unit);
        }

        root[F("format")] = F("uint16");
    }


#if HAP_USE_STD_STRING
    std::string valueString() {
        char temp[32];
        snprintf(temp, 32, "%" PRIu16, value());
        return std::string(temp);
    }
#else
    String valueString(){
        return String(value());
    }
#endif

    void valueFromString(const char* val) {
        setValue(atoi(val));
    }

    // void valueString(char* val, size_t* length, bool withCallback = false){
    //     char buffer[32] = {'\0', };
    //     int ret = snprintf(buffer, 32, "%" PRIu16, value(withCallback));

    //     if (ret < 0){
    //         *length = 0;
    //         return;
    //     }

    //     *length = ret;

    //     if (val){
    //         snprintf(val, strlen(buffer), "%" PRIu16, value(false));
    //     }
    // }

protected:

};

//
// uint32_t
//
template <>
class HAPCharacteristic<uint32_t> : public HAPCharacteristicNumericBase<uint32_t> {
public:

    HAPCharacteristic(HAPCharacteristicType type, uint8_t permissions, uint32_t minVal, uint32_t maxVal, uint32_t step, HAPUnit unit) : HAPCharacteristicNumericBase<uint32_t>(type, permissions, minVal, maxVal, step, unit) {
        this->_value = minVal;
    }

    HAPCharacteristic(const char* type, uint8_t permissions, uint32_t minVal, uint32_t maxVal, uint32_t step, HAPUnit unit) : HAPCharacteristicNumericBase<uint32_t>(type, permissions, minVal, maxVal, step, unit) {
        this->_value = minVal;
    }

    void valueToJson(JsonObject& root, bool withCallback = true) override {
        if (readable()) {
            root[F("value")] = value(withCallback);
        }
    }

    void metaToJson(JsonObject& root) override {
        root[F("minValue")] = _minVal;
        root[F("maxValue")] = _maxVal;
        root[F("minStep")]  = _step;

        if (_unit != HAPUnit::None) {
            unitToJson(root, _unit);
        }

        root[F("format")] = "uint32";
    }


#if HAP_USE_STD_STRING
    std::string valueString() {
        char temp[64];
        snprintf(temp, 64, "%" PRIu32, value());
        return std::string(temp);
    }
#else
    String valueString(){
        return String(value());
    }
#endif

    void valueFromString(const char* val) {
        setValue(atol(val));
    }

    // void valueString(char* val, size_t* length, bool withCallback = false){
    //     char buffer[64] = {'\0', };
    //     int ret = snprintf(buffer, 64, "%" PRIu32, value(withCallback));

    //     if (ret < 0){
    //         *length = 0;
    //         return;
    //     }

    //     *length = ret;

    //     if (val){
    //         snprintf(val, strlen(buffer), "%" PRIu32, value(false));
    //     }
    // }

protected:

};


//
// uint64_t
//
template <>
class HAPCharacteristic<uint64_t> : public HAPCharacteristicNumericBase<uint64_t> {
public:

    HAPCharacteristic(HAPCharacteristicType type, uint8_t permissions, uint64_t minVal, uint64_t maxVal, uint64_t step, HAPUnit unit) : HAPCharacteristicNumericBase<uint64_t>(type, permissions, minVal, maxVal, step, unit) {
        this->_value = minVal;
    }

    HAPCharacteristic(const char* type, uint8_t permissions, uint64_t minVal, uint64_t maxVal, uint64_t step, HAPUnit unit) : HAPCharacteristicNumericBase<uint64_t>(type, permissions, minVal, maxVal, step, unit) {
        this->_value = minVal;
    }

    void valueToJson(JsonObject& root, bool withCallback = true) override {
        if (readable()) {
            root[F("value")] = value(withCallback);
        }
    }

    void metaToJson(JsonObject& root) override {
        root[F("minValue")] = _minVal;
        root[F("maxValue")] = _maxVal;
        root[F("minStep")]  = _step;

        if (_unit != HAPUnit::None) {
            unitToJson(root, _unit);
        }

        root[F("format")] = "uint64";
    }

#if HAP_USE_STD_STRING
    std::string valueString() {
        char temp[128];
        snprintf(temp, 128, "%" PRIu64, value());
        return std::string(temp);
    }
#else
    String valueString(){
        char temp[128];
        snprintf(temp, 128, "%" PRIu64, value());
        return String(temp);
    }
#endif

    void valueFromString(const char* val) {
        setValue(atoll(val));
    }


    // void valueString(char* val, size_t* length, bool withCallback = false){
    //     char buffer[128] = {'\0', };
    //     int ret = snprintf(buffer, 128, "%" PRIu64, value(withCallback));

    //     if (ret < 0){
    //         *length = 0;
    //         return;
    //     }

    //     *length = ret;

    //     if (val){
    //         snprintf(val, strlen(buffer), "%" PRIu64, value(false));
    //     }
    // }

protected:

};


//
// String
//
template <>
class HAPCharacteristic<String> : public HAPCharacteristicBaseValue<String> {
public:

    HAPCharacteristic(HAPCharacteristicType type, uint8_t permissions, String format = F("string"), size_t maxlen = 64) : HAPCharacteristicBaseValue<String>(type, permissions) {
        _value = "";
        _maxLen = maxlen;
        _format = format;
    }
    HAPCharacteristic(const char* type, uint8_t permissions, String format = F("string"), size_t maxlen = 64) : HAPCharacteristicBaseValue<String>(type, permissions) {
        _value = "";
        _maxLen = maxlen;
        _format = format;
    }

    void valueToJson(JsonObject& root, bool withCallback = true) override {
        if (readable()) {
            root[F("value")] = value(withCallback);
        }
    }

    void metaToJson(JsonObject& root) override {
        root[F("maxLen")] = _maxLen;
        root[F("format")] = _format;
    }

#if HAP_USE_STD_STRING
    std::string valueString() {
        return std::string(value().c_str());
    }
#else
    String valueString(){
        return value();
    }
#endif

    void valueFromString(const char* val) {

        if (strlen(val) > _maxLen) return;

        setValue(val);
    }


    // void valueString(char* val, size_t* length, bool withCallback = false){
    //     *length = _value.length();
    //     if (val) {
    //         _value.toCharArray(val, *length);
    //     }
    // }

protected:
    size_t _maxLen = 64;
    String _format;
};


//
// std::string
//
template <>
class HAPCharacteristic<std::string> : public HAPCharacteristicBaseValue<std::string> {
public:

    HAPCharacteristic(HAPCharacteristicType type, uint8_t permissions, size_t maxlen) : HAPCharacteristicBaseValue<std::string>(type, permissions) {
        _value = "";
        _maxLen = maxlen;
    }
    HAPCharacteristic(const char* type, uint8_t permissions, size_t maxlen) : HAPCharacteristicBaseValue<std::string>(type, permissions) {
        _value = "";
        _maxLen = maxlen;
    }

    void valueToJson(JsonObject& root, bool withCallback = true) override {
        if (readable()) {
            root[F("value")] = value(withCallback);
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
        return String(value().c_str());
    }
#endif

    void valueFromString(const char* val) {

        if (strlen(val) > _maxLen) return;

        setValue(val);
    }


    // void valueString(char* val, size_t* length, bool withCallback = false){
    //     *length = _value.length();
    //     if (val) {
    //         strncpy(val, value(withCallback).c_str(), _value.length());
    //     }
    // }

protected:
    size_t _maxLen = 64;
};



#endif /* HAPCHARACTERISTIC_HPP_ */