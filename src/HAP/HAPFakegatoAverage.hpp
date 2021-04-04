// 
// HAPFakegatoAverage.hpp
// Teensy_Homekit
//
//  Created on: 03.04.2021
//      Author: michael
//

#ifndef HAPFAKEGATOAVERAGE_HPP_
#define HAPFAKEGATOAVERAGE_HPP_

template <class T>
class HAPFakegatoAverage {
public:
    HAPFakegatoAverage() {

    }
    ~HAPFakegatoAverage(){

    }

    void addValue(T value){
        _averagedValue += value;
        _measureCount++;
    }

    T getAverage(){
        T value = (_averagedValue / _measureCount);
        reset();
        return value;
    }

    void reset(){
        _averagedValue = 0;
        _measureCount = 0;
    }

protected:
    T _averagedValue = 0;
    uint16_t _measureCount = 0;
};

#endif /* HAPFAKEGATOAVERAGE_HPP_ */