//
// HAPPrintBuffered.cpp
// Homekit
//
//  Created on: 08.05.2018
//      Author: michael
//

#include "HAPPrintBuffered.hpp"

HAPPrintBuffered::HAPPrintBuffered(Print& target_, uint8_t* buffer_, size_t size_) 
: _target(target_)
, _buffer(buffer_)
, _size(size_)
, _pos(0)
{}


size_t HAPPrintBuffered::write(uint8_t b) {
    _buffer[_pos++] = b;
    if (_pos == _size) {
        flush();
    }
    return 1;
}

void HAPPrintBuffered::flush() {
    if (!_pos)
        return;
    size_t l = _target.write(_buffer, _pos);
    _pos -= l;
    if (_pos > 0 && l > 0) { // should not happen often
        for (size_t i = 0; i < _pos; i++) {
        _buffer[i] = _buffer[i + l];
        }
    }
    if (_target.getWriteError()) {
        setWriteError();
    }
}

int HAPPrintBuffered::availableForWrite() {
    // int a = target.availableForWrite();
    // if (!a) // target doesn't report aFW or is really full
    return _size - _pos; // then return space left in our buffer
    // a = a - pos; // what will be left in target after our flush()
    // return a < 0 ? 0 : a;
}


