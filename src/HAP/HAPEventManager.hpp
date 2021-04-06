// 
// HAPEventManager.hpp
// Teensy_Homekit
//
//  Created on: 30.03.2021
//      Author: michael
//

#ifndef HAPEVENTMANAGER_HPP_
#define HAPEVENTMANAGER_HPP_

#include <vector>
#include <functional>
#include "HAPClient.hpp"

template<typename T, typename... U>
size_t getFunctionAddress(std::function<T(U...)> f) {
    typedef T(fnType)(U...);
    fnType ** fnPointer = f.template target<fnType*>();
    return (size_t) *fnPointer;
}

typedef enum {
  HAP_EVENT_PRIORITY_LOW    = 0x00,
  HAP_EVENT_PRIORITY_HIGH   = 0x01  
} HAP_EVENT_PRIORITY;


typedef enum {
    HAP_EVENT_ALL = 0x00,
    
    //
    // Errors
    //
    HAP_EVENT_ERROR_FATAL = 0x01,    

    // 
    // System
    //
    HAP_EVENT_SYSTEM_RESTART = 0x20,
    
    // 
    // Homekit
    // 
    HAP_EVENT_HOMEKIT_NOTIFY = 0x50,
    HAP_EVENT_HOMEKIT_UPDATE_CONFIGNUM,
    HAP_EVENT_HOMEKIT_PAIRINGS_DELETED_ALL,
    HAP_EVENT_HOMEKIT_PAIRINGS_ADDED,

    // 
    // Fakegato
    // 
    HAP_EVENT_TIMER_EVENT_START = 0x70,
    HAP_EVENT_TIMER_EVENT_END,
    
    // 
    // Config
    // 
    HAP_EVENT_CONFIG_NEEDS_UPDATE = 0x90,
    HAP_EVENT_CONFIG_NEEDS_SAVE,
    HAP_EVENT_CONFIG_NEEDS_LOAD,
    HAP_EVENT_CONFIG_RESET,

} HAP_EVENT_TYPE;


struct HAPEventListener {    
    HAP_EVENT_TYPE  eventType;
    std::function<void(HAPEvent)> callback;

    HAPEventListener(std::function<void(HAPEvent event)> callback_, HAP_EVENT_TYPE  eventType_ = HAP_EVENT_ALL)
    : eventType(eventType_)
    , callback(callback_)
    {}
};

struct HAPEventNotifyPayload {
    const uint8_t aid;
    const uint32_t iid;
    const char* value;

    HAPEventNotifyPayload(uint8_t aid_, uint32_t iid_, const char* value_)
    : aid(aid_)
    , iid(iid_)
    , value(value_)
    {}
};


struct HAPEvent{
    
    HAP_EVENT_TYPE eventType;
    HAP_EVENT_PRIORITY priority;
    void* payload;

    HAPEvent(HAP_EVENT_TYPE type, void* payload_, HAP_EVENT_PRIORITY prio = HAP_EVENT_PRIORITY_LOW)
    : eventType(type)
    , priority(prio)    
    {
        payload = std::move(payload_);
    }
};


class HAPEventManager {
public:
    HAPEventManager();
    ~HAPEventManager();

    void queueNotifyEvent(uint8_t aid, uint32_t iid, const char* value, HAP_EVENT_PRIORITY priority = HAP_EVENT_PRIORITY_LOW){
        HAPEventNotifyPayload eventPayload(aid, iid, value);

        HAPEvent event(HAP_EVENT_HOMEKIT_NOTIFY, &eventPayload, priority); 
        queueEvent(event);       
    }

    void queueEvent(const HAPEvent& event){
        if (event.priority == HAP_EVENT_PRIORITY_HIGH){
            // handle Event immediately
            handleEvent(event);
        } else {
            _eventqueue.push_back(std::move(event));
        }
    }

    size_t numberOfEvents(){ return _eventqueue.size(); }
    size_t numberOfListeners(){ return _listeners.size(); }

    void handleEvent(const HAPEvent& event){
        for (auto &listener : _listeners){
            if ( (listener.eventType == event.eventType) || (listener.eventType == HAP_EVENT_ALL) ) {
                listener.callback(event);
            }
        }
    }

    void addListener(std::function<void(HAPEvent)> callback_, HAP_EVENT_TYPE eventType_ = HAP_EVENT_ALL){
        _listeners.push_back(HAPEventListener(callback_, eventType_));
    }

    void removeListener(std::function<void(HAPEvent)> callback_, HAP_EVENT_TYPE eventType_ = HAP_EVENT_ALL){
        for (size_t i=0; i < _listeners.size(); i++){
            if ((_listeners[i].eventType == eventType_) && (getFunctionAddress(_listeners[i].callback) == getFunctionAddress(callback_))){
                _listeners.erase(_listeners.begin() + i);
                return;
            }
        }
    }

    void handle(){
        for (auto &event : _eventqueue){                    
            handleEvent(event);
        }

        clearEvents();
    }

    void clearEvents(){
        _eventqueue.clear();
    }

    void clearListeners(){
        _listeners.clear();
    }

protected:

    std::vector<HAPEvent> _eventqueue;
    std::vector<HAPEventListener> _listeners;
};

#endif /* HAPEVENTMANAGER_HPP_ */