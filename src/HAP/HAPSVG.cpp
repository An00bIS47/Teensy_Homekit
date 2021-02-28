//
// HAPSVG.hpp
// Homekit
//
//  Generated on: 23.09.2019
//

#if HAP_PRINT_QRCODE
#include "HAPSVG.hpp"

#define QR_CODE_X_OFFSET    0
#define QR_CODE_Y_OFFSET    0
#define QR_CODE_ZOOM_FACTOR 5


void HAPSVG::begin(Print& prt, QRCode* qrcode){
    int width = (((4 * qrcode->version) + 17) * QR_CODE_ZOOM_FACTOR ) + QR_CODE_X_OFFSET + QR_CODE_Y_OFFSET;
    int height = width;

    // Serial.println(String(width) + "x" + String(height));

    // String svg = "";
    // appendstringtosvg(_svg, "<svg width='");
    prt.print(F("<svg width='"));    
    
    // appendnumbertosvg(_svg, width);
    prt.print(width);

    // appendstringtosvg(_svg, "px' height='");
    prt.print(F("px' height='"));

    // appendnumbertosvg(_svg, height);
    prt.print(height);

    // appendstringtosvg(_svg, "px' xmlns='http://www.w3.org/2000/svg' version='1.1' xmlns:xlink='http://www.w3.org/1999/xlink'>\n");
    prt.print(F("px' xmlns='http://www.w3.org/2000/svg' version='1.1' xmlns:xlink='http://www.w3.org/1999/xlink'>"));
}


void HAPSVG::end(Print& prt)
{
    prt.print(F("</svg>"));
}



void HAPSVG::rectangle(Print& prt, int width, int height, int x, int y, const char* fill, const char* stroke, int strokeWidth, int radiusx, int radiusy)
{       
    prt.print(F("<rect width='"));
    prt.print(width);
    
    prt.print(F("' height='"));
    prt.print(height);
    

    prt.print(F("' y='"));    
    prt.print(y);
    
    
    prt.print(F("' x='"));
    prt.print(x);
    
    // appendstringtosvg(_svg, "' ry='");
    prt.print(F("' ry='"));
    
    // appendnumbertosvg(_svg, radiusy);
    prt.print(radiusy);
    
    // appendstringtosvg(_svg, "' rx='");
    prt.print(F("' rx='"));
    
    // appendnumbertosvg(_svg, radiusx);
    prt.print(radiusx);

    if (strcmp_P(fill, PSTR("#000")) != 0 ) {
        prt.print(F("' fill='"));        
        prt.print(fill);
    }

    if (strokeWidth > 0) {
        prt.print(F("' stroke='"));
        prt.print(stroke);

        prt.print(F("' stroke-width='"));
        prt.print(strokeWidth);
    }

    
    // appendstringtosvg(_svg, "' />\n");
    prt.print(F("' />"));       
}

void HAPSVG::drawQRCode(Print& prt, QRCode* qrcode, const char* colorFill, const char* colorBackground){

    begin(prt, qrcode);

    int k = 0;         
    for (uint8_t y = 0; y < qrcode->size; y++) {
        int n = 0;
        for (uint8_t x = 0; x < qrcode->size; x++) {

            int cx = x + QR_CODE_X_OFFSET + (n * QR_CODE_ZOOM_FACTOR) - n;
            int cy = y + QR_CODE_Y_OFFSET + (k * QR_CODE_ZOOM_FACTOR) - k;

            if (qrcode_getModule(qrcode, x, y) ) {  
                // draw black rect                
                rectangle(prt, QR_CODE_ZOOM_FACTOR, QR_CODE_ZOOM_FACTOR, cx, cy, colorFill, "", 0 , 0, 0);	
            } else {
                // draw white rect
                // rectangle(prt, QR_CODE_ZOOM_FACTOR, QR_CODE_ZOOM_FACTOR, cx, cy, colorBackground, "", 0, 0, 0);	
            }
            n++;
        }            
        k++;            
    }

    end(prt);
}

#endif