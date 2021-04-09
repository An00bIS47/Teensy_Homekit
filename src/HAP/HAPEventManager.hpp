// // 
// // HAPEventManager.hpp
// // Teensy_Homekit
// //
// //  Created on: 30.03.2021
// //      Author: michael
// //

// #ifndef HAPEVENTMANAGER_HPP_
// #define HAPEVENTMANAGER_HPP_

// #include <vector>
// #include <functional>
// #include "HAPClient.hpp"
// #include "HAPEventListener.hpp"

// struct HAPEventNotifyPayload {
//     const uint8_t aid;
//     const uint32_t iid;
//     const char* value;

//     HAPEventNotifyPayload(uint8_t aid_, uint32_t iid_, const char* value_)
//     : aid(aid_)
//     , iid(iid_)
//     , value(value_)
//     {}
// };




// class HAPEventManager {
// public:
//     HAPEventManager() {

//     }
//     ~HAPEventManager() {
//         _eventqueue.clear();
//         _listeners.clear();
//     }

//     void queueNotifyEvent(uint8_t aid, uint32_t iid, const char* value, HAP_EVENT_PRIORITY priority = HAP_EVENT_PRIORITY_LOW){
//         HAPEventNotifyPayload eventPayload(aid, iid, value);

//         HAPEventTwo event(HAP_EVENT_HOMEKIT_NOTIFY, &eventPayload, priority); 
//         queueEvent(event);       
//     }

//     void queueEvent(const HAPEventTwo& event){
//         if (event.priority == HAP_EVENT_PRIORITY_HIGH){
//             // handle Event immediately
//             handleEvent(event);
//         } else {
//             _eventqueue.push_back(std::move(event));
//         }
//     }

//     size_t numberOfEvents(){ return _eventqueue.size(); }
//     size_t numberOfListeners(){ return _listeners.size(); }

//     void handleEvent(const HAPEventTwo& event){
//         for (auto &item : _listeners){
//             if ( (item.eventType == event.eventType) || (item.eventType == HAP_EVENT_ALL) ) {
//                 (*item)->callback(event);
//             }
//         }
//     }


//     void addListener(HAP_EVENT_TYPE eventType, HAPEventListener* listener){        
        
//         HAPListenerItem item;
//         item.callback = listener;
//         item.eventType = eventType;
//         item.enabled   = true;
//         _listeners.push_back(item);        
//     }

//     void removeListener(HAPEventListener* listener){
//         // for (auto &it = _listeners.begin(); it != _listeners.end(); it++){
//         //     // Compare address of these two functions
//         //     if ((it.callback) == listener){
//         //         _listeners.erase(it);
//         //         return true;
//         //     }
//         // }
//         // return false;

//         for (int i=0; i < _listeners.size(); i++){
//             if (_listeners[i].callback == listener){
//                 _listeners.erase(_listeners.begin() + i);
//                 return;
//             }
//         }
//     }

//     void handle(){
//         for (auto &event : _eventqueue){                    
//             handleEvent(event);
//         }

//         clearEvents();
//     }

//     void clearEvents(){
//         _eventqueue.clear();
//     }

//     void clearListeners(){
//         _listeners.clear();
//     }

// protected:
    
//     // Listener structure and corresponding array
//     struct HAPListenerItem
//     {
//         HAPEventListener*	callback;		// The listener function
//         HAP_EVENT_TYPE		eventType;		// The event type
//         bool			    enabled;	    // Each listener can be enabled or disabled
//     };

//     std::vector<HAPEventTwo> _eventqueue;
//     std::vector<HAPListenerItem> _listeners;
// };

// #endif /* HAPEVENTMANAGER_HPP_ */