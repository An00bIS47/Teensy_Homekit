//
// HAPPrintPlus.hpp
// Homekit
//
//  Created on: 08.05.2018
//      Author: michael
//

#ifndef _HAPPRINTPLUS_HPP_
#define _HAPPRINTPLUS_HPP_

#include <Arduino.h>

class HAPPrintPlus : public Print {

public:

  size_t printf(const char *fmt, ... );

  size_t printf(const __FlashStringHelper *fmt, ... );

  uint32_t copyFrom(Stream& stream, uint32_t limit = 0);
  uint32_t copyFromUntil(char terminator, Stream& stream, uint32_t limit = 0);
  uint32_t copyAvailableFrom(Stream& stream, uint32_t limit = 0);

};

typedef HAPPrintPlus FormattedPrint; // the previous name of the class

#endif
