//
// HAPPrintChunked.hpp
// Homekit
//
//  Created on: 08.05.2018
//      Author: michael
//
#ifndef _HAPPRINTBUFFERED_HPP_
#define _HAPPRINTBUFFERED_HPP_

#include "HAPPrintPlus.hpp"

class HAPPrintBuffered : public HAPPrintPlus {

protected:
    Print &_target;
    uint8_t* _buffer;
    size_t _size;
    size_t _pos;

public:
    HAPPrintBuffered(Print &target_, uint8_t* buffer_, size_t size_);
    virtual ~HAPPrintBuffered() { flush(); };

    virtual size_t write(uint8_t b);

    using Print::write; // pull in write(str) and write(buf, size) from Print

    virtual void flush();

    virtual int availableForWrite();

};

#endif /* _HAPPRINTBUFFERED_HPP_ */
