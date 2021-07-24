// 
// HAPLogging.cpp
// Homekit
//
//  Created on: 22.06.2021
//      Author: michael
//

#include "HAPLogging.hpp"
#include <cinttypes>

#if HAP_LOGGING_SUPPORTS_MBEDTLS

#endif

int HAPLoglevel = LOG_LEVEL;

#if IS_LINUX
    #define PrintOut printf
#else
    Print *LOGDEVICE = &Serial;
    #define PrintOut output->printf
#endif


#if IS_LINUX
void logHexDump(const char *letter, const char *label, const uint8_t *data, const size_t length, const char* color) {
#else
void logHexDump(Print *output, const char *letter, const char *label, const uint8_t *data, const size_t length, const char* color) {
#endif
    size_t cnt = 0;
    size_t step = 0;
    char limiter = '|';
    // Use line buffer to speed up output
    const uint16_t BUFLEN(80);
    const uint16_t ascOffset(61);
    char linebuf[BUFLEN];
    char *cp = linebuf;
    const char HEXDIGIT[] = "0123456789ABCDEF";

    PrintOut(color);
    // Print out header
    PrintOut ("[%s] %s: @%" PRIXPTR "/%" PRIu32 ":\n", letter, label, (uintptr_t)data, (uint32_t)(length & 0xFFFFFFFF));

    // loop over data in steps of 16
    for (cnt = 0; cnt < length; ++cnt) {
        step = cnt % 16;
        // New line?
        if (step == 0) {
            // Yes. Clear line and print address header
            memset(linebuf, ' ', BUFLEN);
            linebuf[60] = limiter;
            linebuf[77] = limiter;
            linebuf[78] = '\n';
            linebuf[BUFLEN - 1] = 0;
            snprintf(linebuf, BUFLEN, "  %c %04X: ", limiter, (uint16_t)(cnt & 0xFFFF));
            cp = linebuf + strlen(linebuf);
        // No, but first block of 8 done?
        } else if (step == 8) {
            // Yes, put out additional space
            cp++;
        }
        // Print data byte
        uint8_t c = data[cnt];
        *cp++ = HEXDIGIT[(c >> 4) & 0x0F];
        *cp++ = HEXDIGIT[c & 0x0F];
        *cp++ = ' ';
        if (c >= 32 && c <= 127) linebuf[ascOffset + step] = c;
        else                     linebuf[ascOffset + step] = '.';
        // Line end?
        if (step == 15) {
            // Yes, print line
            PrintOut ("%s", linebuf);
        }
    }
    // Unfinished line?
    if (length && step != 15) {
        // Yes, print line
        PrintOut ("%s", linebuf);
    }

    PrintOut(LOG_COLOR_NORMAL);
}

#if IS_LINUX
void logArray(const char *letter, const char *label, const uint8_t *data, const size_t length, const char* color) {
#else
void logArray(Print *output, const char *letter, const char *label, const uint8_t *data, const size_t length, const char* color) {
#endif
    PrintOut(color);
    // Print out header
    PrintOut ("[%s] %s: @%" PRIXPTR "/%" PRIu32 ":\n", letter, label, (uintptr_t)data, (uint32_t)(length & 0xFFFFFFFF));

    PrintOut("=== [%s] (length:%d) ===\n", label, length);
    bool need_lf = true;
    for (size_t i=0; i<length; i++) {
        if ((i & 0xf) == 0xf) {
            PrintOut("%02X\n", data[i]);
            need_lf = false;
        } else {
            PrintOut("%02X ", data[i]);
            need_lf = true;
        }
    }
    if (need_lf){
        PrintOut("\n======\n");
    } else {
        PrintOut("======\n");
    }
    PrintOut(LOG_COLOR_NORMAL);
}

#if HAP_LOGGING_SUPPORTS_MBEDTLS
#if IS_LINUX
void logMpi(const char *letter, const char *label, const mbedtls_mpi* bignum, const char* color) {
#else
void logMpi(Print *output, const char *letter, const char *label, const mbedtls_mpi* bignum, const char* color) {
#endif
    size_t bignumLength = mbedtls_mpi_size(bignum);
    // uint8_t* num = (uint8_t*) malloc(sizeof(uint8_t) * bignumLength);
    uint8_t num[bignumLength];
    mbedtls_mpi_write_binary(bignum, num, bignumLength);
#if IS_LINUX
    logArray(letter, label, num, bignumLength, color);
#else
    logArray(output, letter, label, num, bignumLength, color);
#endif
    // free(num);
}
#endif

#if defined( CORE_TEENSY )

#if defined(ARDUINO_TEENSY40)
  static const unsigned DTCM_START = 0x20000000UL;
  static const unsigned OCRAM_START = 0x20200000UL;
  static const unsigned OCRAM_SIZE = 512;
  static const unsigned FLASH_SIZE = 1984;
#elif defined(ARDUINO_TEENSY41)
  static const unsigned DTCM_START = 0x20000000UL;
  static const unsigned OCRAM_START = 0x20200000UL;
  static const unsigned OCRAM_SIZE = 512;
  static const unsigned FLASH_SIZE = 7936;
  #if TEENSYDUINO>151
    extern "C" uint8_t external_psram_size;
  #endif
#endif

unsigned t_memfree(void) {
    extern unsigned long _ebss;
    extern unsigned long _sdata;
    extern unsigned long _estack;
    const unsigned DTCM_START = 0x20000000UL;
    unsigned dtcm = (unsigned)&_estack - DTCM_START;
    unsigned stackinuse = (unsigned) &_estack -  (unsigned) __builtin_frame_address(0);
    unsigned varsinuse = (unsigned)&_ebss - (unsigned)&_sdata;
    unsigned freemem = dtcm - (stackinuse + varsinuse);
    return freemem;
}

unsigned t_heapfree(void) {
    // https://forum.pjrc.com/threads/33443-How-to-display-free-ram?p=99128&viewfull=1#post99128
    void* hTop = malloc(1);// current position of heap.
    unsigned heapTop = (unsigned) hTop;
    free(hTop);
    unsigned freeheap = (OCRAM_START + (OCRAM_SIZE * 1024)) - heapTop;
    return freeheap;
}

unsigned long t_maxstack(void) {
    extern unsigned long _ebss;
    extern unsigned long _estack;
    uint32_t e = (uintptr_t)&_ebss;
    uint32_t * p = (uint32_t*)e + 32;
    while (*p == 0) p++;
    return (unsigned) &_estack - (unsigned) p;
}
#endif