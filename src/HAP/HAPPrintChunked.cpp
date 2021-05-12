//
// HAPPrintChunked.cpp
// Homekit
//
//  Created on: 08.05.2018
//      Author: michael
//
#include "HAPPrintChunked.hpp"

#define ENCODING_RESERVE 7

HAPPrintChunked::HAPPrintChunked(Print &target, char* buffer, size_t size)
  : HAPPrintBuffered(target, (uint8_t*)buffer + ENCODING_RESERVE, size - ENCODING_RESERVE) {
}

HAPPrintChunked::HAPPrintChunked(Print &target, uint8_t* buffer, size_t size)
  : HAPPrintBuffered(target, buffer + ENCODING_RESERVE, size - ENCODING_RESERVE) {
}

void HAPPrintChunked::begin() {
    HAPPrintBuffered::flush();
    _encode = true;
    _firstChunk = true;
}

void HAPPrintChunked::end() {
    if (_pos > 0) {
        flush();
    }
    if (_encode) {
        _target.println(F("\r\n0\r\n"));
        _encode = false;
    }
}

void HAPPrintChunked::flush() {
    if (!_encode) {
        HAPPrintBuffered::flush();
        return;
    }
    if (!_pos)
        return;
    uint8_t l = (_firstChunk ? 2 : 4) + (_pos > 0xFF ? 3 : (_pos <= 0xF ? 1 : 2));
    sprintf_P((char*)_buffer - l, (_firstChunk ? PSTR("%x\r") : PSTR("\r\n%x\r")), _pos);
    (_buffer - 1)[0] = '\n';
    if (_firstChunk) {
        _firstChunk = false;
    }
    _target.write((char*)_buffer - l, l + _pos);
    _pos = 0;
}

