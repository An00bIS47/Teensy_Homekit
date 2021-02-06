//
// HAPPrintChunked.hpp
// Homekit
//
//  Created on: 08.05.2018
//      Author: michael
//
#ifndef _HAPPRINTCHUNKED_HPP_
#define _HAPPRINTCHUNKED_HPP_

#include "HAPPrintBuffered.hpp"



class HAPPrintChunked : public HAPPrintBuffered {

protected:
  bool _encode = false;
  bool _firstChunk = false;

public:
  HAPPrintChunked(Print &target, char* buffer, size_t size);
  ~HAPPrintChunked() { end(); }

  void begin();
  void end();

  void flush() override;
};

#endif /* _HAPPRINTCHUNKED_HPP_ */
