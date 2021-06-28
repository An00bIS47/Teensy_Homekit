// 
// HAPLogging.hpp
// Teensy_Homekit
//
//  Created on: 22.06.2021
//      Author: michael
//
#include <vector>

#ifndef LOG_LEVEL
#define LOG_LEVEL 6
#endif

#ifndef LOCAL_LOG_LEVEL
#define LOCAL_LOG_LEVEL LOG_LEVEL
#endif

// #ifndef LOGGING_HAS_HAPTIME
// #define LOGGING_HAS_HAPTIME 0
// #endif

#ifndef HAPLOGGING_HPP_
#define HAPLOGGING_HPP_

extern int HAPLoglevel;

#if !IS_LINUX
#include <Arduino.h>
#endif

#define LL_RED      "\e[1;31m"      // ERROR
#define LL_GREEN    "\e[32m"        // INFO
#define LL_YELLOW   "\e[1;33m"      // WARNING
#define LL_BLUE     "\e[34m"        // VERBOSE
#define LL_MAGENTA  "\e[35m"        // CRITICAL
#define LL_CYAN     "\e[36m"        // DEBUG
#define LL_NORM     "\e[0m"         //

#define LOG_LEVEL_NONE      (0)     // 
#define LOG_LEVEL_CRITICAL  (1)     // magenta
#define LOG_LEVEL_ERROR     (2)     // red
#define LOG_LEVEL_WARNING   (3)     // yellow
#define LOG_LEVEL_INFO      (4)     // green
#define LOG_LEVEL_DEBUG     (5)     // cyan
#define LOG_LEVEL_VERBOSE   (6)     // blue

#define LOG_COLOR_CRITICAL  LL_MAGENTA
#define LOG_COLOR_ERROR     LL_RED
#define LOG_COLOR_WARNING   LL_YELLOW
#define LOG_COLOR_INFO      LL_GREEN
#define LOG_COLOR_DEBUG     LL_CYAN
#define LOG_COLOR_VERBOSE   LL_BLUE


#define LOG_DESC() \
    LOG_C("%s - This is a %s message\n", "CRITICAL", "critical"); \
    LOG_E("%s - This is an %s!\n", "ERROR", "error"); \
    LOG_W("%s - This is a %s!\n", "WARNING", "warning"); \
    LOG_I("%s - This is an %s.\n", "INFO", "information"); \
    LOG_D("%s - This is a %s message.\n", "DEBUG", "debug"); \
    LOG_V("%s - This is a %s message.\n", "VERBOSE", "verbose");

// #if LOGGING_HAS_HAPTIME
// //                        [H]    1968 | main.cpp             [   22] setup |
// #define LOG_HEADER(x) "[" #x "] %-16s | %-20s [%5d] %s | "
// #else
#define LOG_HEADER(x) "[" #x "] %8lu | %-20s [%4d] %s | "
// #endif

constexpr const char* str_end(const char *str) {
    return *str ? str_end(str + 1) : str;
}

constexpr bool str_slant(const char *str) {
    return ((*str == '/') || (*str == '\\')) ? true : (*str ? str_slant(str + 1) : false);
}

constexpr const char* r_slant(const char* str) {
    return ((*str == '/') || (*str == '\\')) ? (str + 1) : r_slant(str - 1);
}
constexpr const char* file_name(const char* str) {
    return str_slant(str) ? r_slant(str_end(str)) : str;
}

#if IS_LINUX
  void logHexDump(const char *letter, const char *label, const uint8_t *data, const size_t length, const char* color = LL_NORM);
#else
  extern Print *LOGDEVICE;
  void logHexDump(Print *output, const char *letter, const char *label, const uint8_t *data, const size_t length, const char* color = LL_NORM);
#endif

#endif /* HAPLOGGING_HPP_ */


// The remainder may need to be redefined if LOCAL_LOG_LEVEL was set differently before
#ifdef LOG_LINE_T
  #undef LOG_LINE_C
  #undef LOG_LINE_E
  #undef LOG_LINE_W
  #undef LOG_LINE_I
  #undef LOG_LINE_D
  #undef LOG_LINE_V
  #undef LOG_LINE_T
  #undef LOG_RAW_C
  #undef LOG_RAW_E
  #undef LOG_RAW_W
  #undef LOG_RAW_I
  #undef LOG_RAW_D
  #undef LOG_RAW_V
  #undef LOG_RAW_T
  #undef HEX_DUMP_T
  #undef HEX_DUMP_C
  #undef HEX_DUMP_E
  #undef HEX_DUMP_W
  #undef HEX_DUMP_I
  #undef HEX_DUMP_D
  #undef HEX_DUMP_V
  #undef LOG_N
  #undef LOG_C
  #undef LOG_E
  #undef LOG_W
  #undef LOG_I
  #undef LOG_D
  #undef LOG_V
  #undef LOGRAW_N
  #undef LOGRAW_C
  #undef LOGRAW_E
  #undef LOGRAW_W
  #undef LOGRAW_I
  #undef LOGRAW_D
  #undef LOGRAW_V
  #undef HEXDUMP_N
  #undef HEXDUMP_C
  #undef HEXDUMP_E
  #undef HEXDUMP_W
  #undef HEXDUMP_I
  #undef HEXDUMP_D
  #undef HEXDUMP_V
  #undef LOG_HEAP
#endif

// Now we can define the macros based on LOCAL_LOG_LEVEL
#if IS_LINUX

  #define LOG_LINE_C(level, x, format, ...) if (HAPLoglevel >= level) printf(LOG_COLOR_CRITICAL LOG_HEADER(x) format LL_NORM, millis(), file_name(__FILE__), __LINE__, __func__, ##__VA_ARGS__)
  #define LOG_LINE_E(level, x, format, ...) if (HAPLoglevel >= level) printf(LOG_COLOR_ERROR LOG_HEADER(x) format LL_NORM, millis(), file_name(__FILE__), __LINE__, __func__, ##__VA_ARGS__)
  #define LOG_LINE_W(level, x, format, ...) if (HAPLoglevel >= level) printf(LOG_COLOR_WARNING LOG_HEADER(x) format LL_NORM, millis(), file_name(__FILE__), __LINE__, __func__, ##__VA_ARGS__)
  #define LOG_LINE_I(level, x, format, ...) if (HAPLoglevel >= level) printf(LOG_COLOR_INFO LOG_HEADER(x) format LL_NORM, millis(), file_name(__FILE__), __LINE__, __func__, ##__VA_ARGS__)
  #define LOG_LINE_D(level, x, format, ...) if (HAPLoglevel >= level) printf(LOG_COLOR_DEBUG LOG_HEADER(x) format LL_NORM, millis(), file_name(__FILE__), __LINE__, __func__, ##__VA_ARGS__)
  #define LOG_LINE_V(level, x, format, ...) if (HAPLoglevel >= level) printf(LOG_COLOR_VERBOSE LOG_HEADER(x) format LL_NORM, millis(), file_name(__FILE__), __LINE__, __func__, ##__VA_ARGS__)

  #define LOG_LINE_T(level, x, format, ...) if (HAPLoglevel >= level) printf(LOG_HEADER(x) format, millis(), file_name(__FILE__), __LINE__, __func__, ##__VA_ARGS__)


  #define LOG_RAW_C(level, x, format, ...) if (HAPLoglevel >= level) printf(LOG_COLOR_CRITICAL format LL_NORM, ##__VA_ARGS__)
  #define LOG_RAW_E(level, x, format, ...) if (HAPLoglevel >= level) printf(LOG_COLOR_ERROR format LL_NORM, ##__VA_ARGS__)
  #define LOG_RAW_W(level, x, format, ...) if (HAPLoglevel >= level) printf(LOG_COLOR_WARNING format LL_NORM, ##__VA_ARGS__)
  #define LOG_RAW_I(level, x, format, ...) if (HAPLoglevel >= level) printf(LOG_COLOR_INFO format LL_NORM, ##__VA_ARGS__)
  #define LOG_RAW_D(level, x, format, ...) if (HAPLoglevel >= level) printf(LOG_COLOR_DEBUG format LL_NORM, ##__VA_ARGS__)
  #define LOG_RAW_V(level, x, format, ...) if (HAPLoglevel >= level) printf(LOG_COLOR_VERBOSE format LL_NORM, ##__VA_ARGS__)

  #define LOG_RAW_T(level, x, format, ...) if (HAPLoglevel >= level) printf(format, ##__VA_ARGS__)

  #define HEX_DUMP_C(x, level, label, address, length) if (HAPLoglevel >= level) logHexDump(#x, label, address, length, LOG_COLOR_CRITICAL)
  #define HEX_DUMP_E(x, level, label, address, length) if (HAPLoglevel >= level) logHexDump(#x, label, address, length, LOG_COLOR_ERROR)
  #define HEX_DUMP_W(x, level, label, address, length) if (HAPLoglevel >= level) logHexDump(#x, label, address, length, LOG_COLOR_WARNING)
  #define HEX_DUMP_I(x, level, label, address, length) if (HAPLoglevel >= level) logHexDump(#x, label, address, length, LOG_COLOR_INFO)
  #define HEX_DUMP_D(x, level, label, address, length) if (HAPLoglevel >= level) logHexDump(#x, label, address, length, LOG_COLOR_DEBUG)
  #define HEX_DUMP_V(x, level, label, address, length) if (HAPLoglevel >= level) logHexDump(#x, label, address, length, LOG_COLOR_VERBOSE)


  #define HEX_DUMP_T(x, level, label, address, length) if (HAPLoglevel >= level) logHexDump(#x, label, address, length)

#else
  // #if LOGGING_HAS_HAPTIME
  //   #define LOG_LINE_C(level, x, format, ...) if (HAPLoglevel >= level) LOGDEVICE->printf(LOG_COLOR_CRITICAL LOG_HEADER(x) format LL_NORM, HAPTime::timestring(), file_name(__FILE__), __LINE__, __func__, ##__VA_ARGS__)
  //   #define LOG_LINE_E(level, x, format, ...) if (HAPLoglevel >= level) LOGDEVICE->printf(LOG_COLOR_ERROR LOG_HEADER(x) format LL_NORM, HAPTime::timestring(), file_name(__FILE__), __LINE__, __func__, ##__VA_ARGS__)
  //   #define LOG_LINE_W(level, x, format, ...) if (HAPLoglevel >= level) LOGDEVICE->printf(LOG_COLOR_WARNING LOG_HEADER(x) format LL_NORM, HAPTime::timestring(), file_name(__FILE__), __LINE__, __func__, ##__VA_ARGS__)
  //   #define LOG_LINE_I(level, x, format, ...) if (HAPLoglevel >= level) LOGDEVICE->printf(LOG_COLOR_INFO LOG_HEADER(x) format LL_NORM, HAPTime::timestring(), file_name(__FILE__), __LINE__, __func__, ##__VA_ARGS__)
  //   #define LOG_LINE_D(level, x, format, ...) if (HAPLoglevel >= level) LOGDEVICE->printf(LOG_COLOR_DEBUG LOG_HEADER(x) format LL_NORM, HAPTime::timestring(), file_name(__FILE__), __LINE__, __func__, ##__VA_ARGS__)
  //   #define LOG_LINE_V(level, x, format, ...) if (HAPLoglevel >= level) LOGDEVICE->printf(LOG_COLOR_VERBOSE LOG_HEADER(x) format LL_NORM, HAPTime::timestring(), file_name(__FILE__), __LINE__, __func__, ##__VA_ARGS__)

  //   #define LOG_LINE_T(level, x, format, ...) if (HAPLoglevel >= level) LOGDEVICE->printf(LOG_HEADER(x) format, HAPTime::timestring(), file_name(__FILE__), __LINE__, __func__, ##__VA_ARGS__)
  // #else
    #define LOG_LINE_C(level, x, format, ...) if (HAPLoglevel >= level) LOGDEVICE->printf(LOG_COLOR_CRITICAL LOG_HEADER(x) format LL_NORM, millis(), file_name(__FILE__), __LINE__, __func__, ##__VA_ARGS__)
    #define LOG_LINE_E(level, x, format, ...) if (HAPLoglevel >= level) LOGDEVICE->printf(LOG_COLOR_ERROR LOG_HEADER(x) format LL_NORM, millis(), file_name(__FILE__), __LINE__, __func__, ##__VA_ARGS__)
    #define LOG_LINE_W(level, x, format, ...) if (HAPLoglevel >= level) LOGDEVICE->printf(LOG_COLOR_WARNING LOG_HEADER(x) format LL_NORM, millis(), file_name(__FILE__), __LINE__, __func__, ##__VA_ARGS__)
    #define LOG_LINE_I(level, x, format, ...) if (HAPLoglevel >= level) LOGDEVICE->printf(LOG_COLOR_INFO LOG_HEADER(x) format LL_NORM, millis(), file_name(__FILE__), __LINE__, __func__, ##__VA_ARGS__)
    #define LOG_LINE_D(level, x, format, ...) if (HAPLoglevel >= level) LOGDEVICE->printf(LOG_COLOR_DEBUG LOG_HEADER(x) format LL_NORM, millis(), file_name(__FILE__), __LINE__, __func__, ##__VA_ARGS__)
    #define LOG_LINE_V(level, x, format, ...) if (HAPLoglevel >= level) LOGDEVICE->printf(LOG_COLOR_VERBOSE LOG_HEADER(x) format LL_NORM, millis(), file_name(__FILE__), __LINE__, __func__, ##__VA_ARGS__)

    #define LOG_LINE_T(level, x, format, ...) if (HAPLoglevel >= level) LOGDEVICE->printf(LOG_HEADER(x) format, millis(), file_name(__FILE__), __LINE__, __func__, ##__VA_ARGS__)
  // #endif

  #define LOG_RAW_C(level, x, format, ...) if (HAPLoglevel >= level) LOGDEVICE->printf(LOG_COLOR_CRITICAL format LL_NORM, ##__VA_ARGS__)
  #define LOG_RAW_E(level, x, format, ...) if (HAPLoglevel >= level) LOGDEVICE->printf(LOG_COLOR_ERROR format LL_NORM, ##__VA_ARGS__)
  #define LOG_RAW_W(level, x, format, ...) if (HAPLoglevel >= level) LOGDEVICE->printf(LOG_COLOR_WARNING format LL_NORM, ##__VA_ARGS__)
  #define LOG_RAW_I(level, x, format, ...) if (HAPLoglevel >= level) LOGDEVICE->printf(LOG_COLOR_INFO format LL_NORM, ##__VA_ARGS__)
  #define LOG_RAW_D(level, x, format, ...) if (HAPLoglevel >= level) LOGDEVICE->printf(LOG_COLOR_DEBUG format LL_NORM, ##__VA_ARGS__)
  #define LOG_RAW_V(level, x, format, ...) if (HAPLoglevel >= level) LOGDEVICE->printf(LOG_COLOR_VERBOSE format LL_NORM, ##__VA_ARGS__)

  #define LOG_RAW_T(level, x, format, ...) if (HAPLoglevel >= level) LOGDEVICE->printf(format, ##__VA_ARGS__)

  #define HEX_DUMP_C(x, level, label, address, length) if (HAPLoglevel >= level) logHexDump(LOGDEVICE, #x, label, address, length, LOG_COLOR_CRITICAL)
  #define HEX_DUMP_E(x, level, label, address, length) if (HAPLoglevel >= level) logHexDump(LOGDEVICE, #x, label, address, length, LOG_COLOR_ERROR)
  #define HEX_DUMP_W(x, level, label, address, length) if (HAPLoglevel >= level) logHexDump(LOGDEVICE, #x, label, address, length, LOG_COLOR_WARNING)
  #define HEX_DUMP_I(x, level, label, address, length) if (HAPLoglevel >= level) logHexDump(LOGDEVICE, #x, label, address, length, LOG_COLOR_INFO)
  #define HEX_DUMP_D(x, level, label, address, length) if (HAPLoglevel >= level) logHexDump(LOGDEVICE, #x, label, address, length, LOG_COLOR_DEBUG)
  #define HEX_DUMP_V(x, level, label, address, length) if (HAPLoglevel >= level) logHexDump(LOGDEVICE, #x, label, address, length, LOG_COLOR_VERBOSE)

  #define HEX_DUMP_T(x, level, label, address, length) if (HAPLoglevel >= level) logHexDump(LOGDEVICE, #x, label, address, length)
#endif

#if LOCAL_LOG_LEVEL >= LOG_LEVEL_NONE
  #define LOG_N(format, ...) LOG_LINE_T(LOG_LEVEL_NONE, N, format, ##__VA_ARGS__)
  #define LOGRAW_N(format, ...) LOG_RAW_T(LOG_LEVEL_NONE, N, format, ##__VA_ARGS__)
  #define HEXDUMP_N(label, address, length) HEX_DUMP_T(N, LOG_LEVEL_NONE, label, address, length)
#else
  #define LOG_N(format, ...)
  #define LOGRAW_N(format, ...)
  #define HEXDUMP_N(label, address, length)
#endif

#if LOCAL_LOG_LEVEL >= LOG_LEVEL_CRITICAL
  #define LOG_C(format, ...) LOG_LINE_C(LOG_LEVEL_CRITICAL, C, format, ##__VA_ARGS__)
  #define LOGRAW_C(format, ...) LOG_RAW_C(LOG_LEVEL_CRITICAL, C, format, ##__VA_ARGS__)
  #define HEXDUMP_C(label, address, length) HEX_DUMP_C(C, LOG_LEVEL_CRITICAL, label, address, length)
#else
  #define LOG_C(format, ...)
  #define LOGRAW_C(format, ...)
  #define HEXDUMP_C(label, address, length)
#endif

#if LOCAL_LOG_LEVEL >= LOG_LEVEL_ERROR
  #define LOG_E(format, ...) LOG_LINE_E(LOG_LEVEL_ERROR, E, format, ##__VA_ARGS__)
  #define LOGRAW_E(format, ...) LOG_RAW_E(LOG_LEVEL_ERROR, E, format, ##__VA_ARGS__)
  #define HEXDUMP_E(label, address, length) HEX_DUMP_E(E, LOG_LEVEL_ERROR, label, address, length)
#else
  #define LOG_E(format, ...)
  #define LOGRAW_E(format, ...)
  #define HEXDUMP_E(label, address, length)
#endif

#if LOCAL_LOG_LEVEL >= LOG_LEVEL_WARNING
  #define LOG_W(format, ...) LOG_LINE_W(LOG_LEVEL_WARNING, W, format, ##__VA_ARGS__)
  #define LOGRAW_W(format, ...) LOG_RAW_W(LOG_LEVEL_WARNING, W, format, ##__VA_ARGS__)
  #define HEXDUMP_W(label, address, length) HEX_DUMP_W(W, LOG_LEVEL_WARNING, label, address, length)
#else
  #define LOG_W(format, ...)
  #define LOGRAW_W(format, ...)
  #define HEXDUMP_W(label, address, length)
#endif

#if LOCAL_LOG_LEVEL >= LOG_LEVEL_INFO
  #define LOG_I(format, ...) LOG_LINE_I(LOG_LEVEL_INFO, I, format, ##__VA_ARGS__)
  #define LOGRAW_I(format, ...) LOG_RAW_I(LOG_LEVEL_INFO, I, format, ##__VA_ARGS__)
  #define HEXDUMP_I(label, address, length) HEX_DUMP_I(I, LOG_LEVEL_INFO, label, address, length)
#else
  #define LOG_I(format, ...)
  #define LOGRAW_I(format, ...)
  #define HEXDUMP_I(label, address, length)
#endif

#if LOCAL_LOG_LEVEL >= LOG_LEVEL_DEBUG
  #define LOG_D(format, ...) LOG_LINE_D(LOG_LEVEL_DEBUG, D, format, ##__VA_ARGS__)
  #define LOGRAW_D(format, ...) LOG_RAW_D(LOG_LEVEL_DEBUG, D, format, ##__VA_ARGS__)
  #define HEXDUMP_D(label, address, length) HEX_DUMP_D(D, LOG_LEVEL_DEBUG, label, address, length)
#else
  #define LOG_D(format, ...)
  #define LOGRAW_D(format, ...)
  #define HEXDUMP_D(label, address, length)
#endif

#if LOCAL_LOG_LEVEL >= LOG_LEVEL_VERBOSE
  #define LOG_V(format, ...) LOG_LINE_V(LOG_LEVEL_VERBOSE, V, format, ##__VA_ARGS__)
  #define LOGRAW_V(format, ...) LOG_RAW_V(LOG_LEVEL_VERBOSE, V, format, ##__VA_ARGS__)
  #define HEXDUMP_V(label, address, length) HEX_DUMP_V(V, LOG_LEVEL_VERBOSE, label, address, length)
#else
  #define LOG_V(format, ...)
  #define LOGRAW_V(format, ...)
  #define HEXDUMP_V(label, address, length)
#endif

#if LOCAL_LOG_LEVEL >= LOG_LEVEL_DEBUG
  #if defined( CORE_TEENSY )
    unsigned t_memfree(void);
    unsigned t_heapfree(void);
    unsigned long t_maxstack(void);
    #define LOG_HEAP(timestring, clients, queue) LOG_LINE_W(LOG_LEVEL_WARNING, H, "%s - heap:%6u | mem:%6u | stack:%6u | clients:%2d | queue:%2d \n", timestring, t_heapfree(), t_memfree(), t_maxstack(), clients, queue)
  #endif
#else
  #define LOG_HEAP()
#endif