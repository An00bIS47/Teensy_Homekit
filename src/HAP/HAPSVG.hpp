//
// HAPSVG.hpp
// Homekit
//
//  Generated on: 23.09.2019
//

#ifndef HAPSVG_HPP_
#define HAPSVG_HPP_

#if HAP_PRINT_QRCODE
#include <Arduino.h>
#include "qrcode.h"

class HAPSVG {
public:
    static void drawQRCode(Print& prt, QRCode* qrcode, const char* colorFill = "#000", const char* colorBackground = "#FFF");
    
private:
    static void begin(Print& prt, QRCode* qrcode);
    static void rectangle(Print& prt, int width, int height, int x, int y, const char* fill, const char* stroke, int strokeWidth, int radiusx, int radiusy);
    static void end(Print& prt);
};
#endif

#endif /* HAPSVG_HPP_ */