// //
// // HAPEventListener.hpp
// // Teensy_Homekit
// //
// //  Created on: 09.04.2021
// //      Author: michael
// //

// #ifndef HAPEVENTLISTENER_HPP_
// #define HAPEVENTLISTENER_HPP_

// typedef enum {
//   HAP_EVENT_PRIORITY_LOW    = 0x00,
//   HAP_EVENT_PRIORITY_HIGH   = 0x01
// } HAP_EVENT_PRIORITY;


// typedef enum {
//     HAP_EVENT_ALL = 0x00,

//     //
//     // Errors
//     //
//     HAP_EVENT_ERROR_FATAL = 0x01,

//     //
//     // System
//     //
//     HAP_EVENT_SYSTEM_RESTART = 0x20,

//     //
//     // Homekit
//     //
//     HAP_EVENT_HOMEKIT_NOTIFY = 0x50,
//     HAP_EVENT_HOMEKIT_UPDATE_CONFIGNUM,
//     HAP_EVENT_HOMEKIT_PAIRINGS_DELETED_ALL,
//     HAP_EVENT_HOMEKIT_PAIRINGS_ADDED,

//     //
//     // Fakegato
//     //
//     HAP_EVENT_TIMER_EVENT_START = 0x70,
//     HAP_EVENT_TIMER_EVENT_END,

//     //
//     // Config
//     //
//     HAP_EVENT_CONFIG_NEEDS_UPDATE = 0x90,
//     HAP_EVENT_CONFIG_NEEDS_SAVE,
//     HAP_EVENT_CONFIG_NEEDS_LOAD,
//     HAP_EVENT_CONFIG_RESET,

// } HAP_EVENT_TYPE;


// struct HAPEventTwo {

//     HAP_EVENT_TYPE eventType;
//     HAP_EVENT_PRIORITY priority;
//     void* payload;

//     HAPEventTwo(HAP_EVENT_TYPE type, void* payload_, HAP_EVENT_PRIORITY prio = HAP_EVENT_PRIORITY_LOW)
//     : eventType(type)
//     , priority(prio)
//     {
//         payload = std::move(payload_);
//     }
// };




// class HAPEventListener
// {
// public:
// 	virtual void operator()( int eventCode, struct HAPEvent eventParam ) = 0;
// };


// template<typename F>
// class GenericCallable : public HAPEventListener
// {
// 	F* mf;

// public:

// 	GenericCallable( F f )
// 	: mf( f )
// 	{}

// 	virtual void operator()( int eventCode, struct HAPEvent eventParam )
// 	{
// 		mf( eventCode, eventParam );
// 	}
// };


// template<class C>
// class MemberFunctionCallable : public HAPEventListener
// {
// 	typedef void (C::*memberfPointer)( int, struct HAPEvent );

// public:

// 	C* mObj;
// 	memberfPointer mf;

// 	MemberFunctionCallable()
// 	{}

// 	MemberFunctionCallable( C* obj, memberfPointer f )
// 	: mObj( obj ), mf( f )
// 	{}

// 	virtual void operator()( int eventCode, struct HAPEvent eventParam )
// 	{
// 		((*mObj).*mf)( eventCode, eventParam );
// 	}

// };
// #endif /* HAPEVENTLISTENER_HPP_ */


