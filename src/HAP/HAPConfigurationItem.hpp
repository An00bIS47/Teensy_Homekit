//
// HAPConfigurationItem.hpp
// Homekit
//
//  Created on: 19.01.2021
//      Author: michael
//

#ifndef HAPCONFIGURATIONITEM_HPP_
#define HAPCONFIGURATIONITEM_HPP_

class HAPConfigurationItem {
public:
    HAPConfigurationItem() : _callbackSave(nullptr) {};
    virtual ~HAPConfigurationItem() {};

    virtual void clear() = 0;
    virtual void setDefaults() = 0;
    virtual void toJson(Print& prt) = 0;


	void setSaveCallback(std::function<bool(void)> callback){
		_callbackSave = callback;
	}

	bool save(){
		if (_callbackSave){
			return _callbackSave();
		}
		return false;
	}


protected:
    std::function<bool(void)> _callbackSave = nullptr;


};

#endif /* HAPCONFIGURATIONITEM_HPP_ */