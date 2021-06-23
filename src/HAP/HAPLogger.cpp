//
// HAPLogger.cpp
// Homekit
//
//  Created on: 08.08.2017
//      Author: michael
//

#include "HAPLogger.hpp"
//#include "HAPServer.hpp"
#include "HAPTime.hpp"

#if defined (ARDUINO_ARCH_ESP32)
#include <esp_log.h>
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

// int staticVar ;

// uint32_t FreeMem(){
// 	uint32_t heapTop;
// 	// current position of heap.
// 	void* hTop = malloc(1);
// 	heapTop = (uint32_t) hTop;
// 	free(hTop);
// 	// Serial.printf("heapTop    =%08X\n",heapTop) ;
// 	// Serial.printf("&staticVar =%08H\n",&staticVar) ;
// 	// The difference is (approximately) the free, available ram.
// 	return (0x20280000 - heapTop);
// }


unsigned memfree(void) {
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

unsigned heapfree(void) {
	// https://forum.pjrc.com/threads/33443-How-to-display-free-ram?p=99128&viewfull=1#post99128
	void* hTop = malloc(1);// current position of heap.
	unsigned heapTop = (unsigned) hTop;
	free(hTop);
	unsigned freeheap = (OCRAM_START + (OCRAM_SIZE * 1024)) - heapTop;
	return freeheap;
}

unsigned long maxstack(void) {
	extern unsigned long _ebss;
	extern unsigned long _estack;
	uint32_t e = (uintptr_t)&_ebss;
	uint32_t * p = (uint32_t*)e + 32;
	while (*p == 0) p++;
	return (unsigned) &_estack - (unsigned) p;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void progInfo(void) {
  Serial.println(__FILE__ " " __DATE__ " " __TIME__ );
  Serial.print(F("Teensyduino version "));
  Serial.println(TEENSYDUINO / 100.0f);
  Serial.println();
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void flexRamInfo(void) {

  //extern unsigned long _stext;
  extern unsigned long _etext;
  extern unsigned long _sdata;
  extern unsigned long _ebss;
  extern unsigned long _flashimagelen;
  extern unsigned long _heap_start;
  extern unsigned long _estack;
  extern unsigned long _itcm_block_count;

  int itcm = (unsigned long)&_itcm_block_count;
  int dtcm = 0;
  int ocram = 0;
  uint32_t gpr17 = IOMUXC_GPR_GPR17;

  char __attribute__((unused)) dispstr[17] = {0};
  dispstr[16] = 0;

  for (int i = 15; i >= 0; i--) {
    switch ((gpr17 >> (i * 2)) & 0b11) {
      default: dispstr[15 - i] = '.'; break;
      case 0b01: dispstr[15 - i] = 'O'; ocram++; break;
      case 0b10: dispstr[15 - i] = 'D'; dtcm++; break;
      case 0b11: dispstr[15 - i] = 'I'; break;
    }
  }

  const char* fmtstr = "%-6s%7d %5.02f%% of %4dkB (%7d Bytes free) %s\n";

  Serial.printf(fmtstr, "FLASH:",
                (unsigned)&_flashimagelen,
                (double)((unsigned)&_flashimagelen) / (FLASH_SIZE * 1024) * 100,
                FLASH_SIZE,
                FLASH_SIZE * 1024 - ((unsigned)&_flashimagelen), "FLASHMEM, PROGMEM");

  unsigned long szITCM = itcm>0?(unsigned long)&_etext:0;
  Serial.printf(fmtstr, "ITCM:",
                szITCM,
                (double)(itcm>0?(((double)szITCM / (itcm * 32768) * 100)):0),
                itcm * 32,
                itcm * 32768 - szITCM, "(RAM1) FASTRUN");

  void* hTop = malloc(8);// current position of heap.
  unsigned heapTop = (unsigned) hTop;
  free(hTop);
  unsigned freeheap = (OCRAM_START + (OCRAM_SIZE * 1024)) - heapTop;
#if defined(ARDUINO_TEENSY41) && TEENSYDUINO>151
  if (external_psram_size > 0) {
	Serial.printf(F("PSRAM: %d MB\n"), external_psram_size);
  } else {
	Serial.printf(F("PSRAM: none\n"), external_psram_size);
  }
#endif
  Serial.printf(F("OCRAM:\n  %7d Bytes (%d kB)\n"), OCRAM_SIZE * 1024, OCRAM_SIZE);
  Serial.printf(F("- %7d Bytes (%d kB) DMAMEM\n"), ((unsigned)&_heap_start - OCRAM_START), ((unsigned)&_heap_start - OCRAM_START) / 1024);
  Serial.printf(F("- %7d Bytes (%d kB) Heap\n"), (heapTop - (unsigned)&_heap_start ), (heapTop - (unsigned)&_heap_start ) / 1024);
  Serial.printf(F("  %7d Bytes heap free (%d kB), %d Bytes OCRAM in use (%d kB).\n"),
                freeheap, freeheap / 1024,
                heapTop - OCRAM_START, (heapTop - OCRAM_START) / 1024);

  unsigned _dtcm = (unsigned)&_estack - DTCM_START; //or, one could use dtcm * 32768 here.
  //unsigned stackinuse = (unsigned) &_estack -  (unsigned) __builtin_frame_address(0);
  unsigned stackinuse = maxstack();
  unsigned varsinuse = (unsigned)&_ebss - (unsigned)&_sdata;
  //unsigned freemem = _dtcm - stackinuse - varsinuse;
  Serial.printf(F("DTCM:\n  %7d Bytes (%d kB)\n"), _dtcm, _dtcm / 1024);
  Serial.printf(F("- %7d Bytes (%d kB) global variables\n"), varsinuse, varsinuse / 1024);
  Serial.printf(F("- %7d Bytes (%d kB) max. stack so far\n"), stackinuse, stackinuse / 1024);
  Serial.println(F("========="));
  Serial.printf(F("  %7d Bytes free (%d kB), %d Bytes in use (%d kB).\n"),
                _dtcm - (varsinuse + stackinuse), (_dtcm - (varsinuse + stackinuse)) / 1024,
                varsinuse + stackinuse, (varsinuse + stackinuse) / 1024
               );
}
#endif

LogLevel HAPLogger::_logLevel(LogLevel::DEBUG);
Stream* HAPLogger::_printer(&Serial);

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
HAPLogger::HAPLogger() {

}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
HAPLogger::~HAPLogger() {
	// TODO Auto-generated destructor stub
}

void HAPLogger::logFreeHeap(int clients, int queue, const char* color){
	if (HAPLogger::_logLevel >= LogLevel::DEBUG) {
		_printer->print(color);
#if HAP_ENABLE_NTP
		_printer->print(HAPTime::timeString());
		_printer->print(F(" "));
#else
		_printer->printf(F("%lu "), millis());
#endif

#if defined(ARDUINO_ARCH_ESP32)
		_printer->print( "HAPServer->heap [   ] current: ") ;
		_printer->print(ESP.getFreeHeap());
		_printer->print(  " - minimum: ") ;
		_printer->print(xPortGetMinimumEverFreeHeapSize());
#elif defined( CORE_TEENSY )
		_printer->print(F("HAPServer->heap [   ] current: ")) ;
		_printer->print(heapfree());
#endif
		_printer->print(F(" [clients:"));
		_printer->print(clients);
		_printer->print(F("]"));
		_printer->print(F(" [queue:"));
		_printer->print(queue);
		_printer->print(F("]"));
		_printer->println(COLOR_RESET);
	}

#if defined( CORE_TEENSY )
	Serial.send_now();

	// flexRamInfo();

#endif
}

void HAPLogger::logInfo(const String &str, bool newLine){
	if (HAPLogger::_logLevel >= LogLevel::INFO) {
		colorPrint(COLOR_INFO, str.c_str(), newLine);
	}
}

void HAPLogger::logError(const String &str, bool newLine){
	if (HAPLogger::_logLevel >= LogLevel::ERROR) {
		colorPrint(COLOR_ERROR, str.c_str(), newLine);
	}
}

void HAPLogger::logDebug(const String &str, bool newLine){
	if (HAPLogger::_logLevel >= LogLevel::DEBUG) {
		colorPrint(COLOR_DEBUG, str.c_str(), newLine);
	}
}

void HAPLogger::logDebug(uint8_t str, bool newLine){
	if (HAPLogger::_logLevel >= LogLevel::DEBUG) {
		colorPrint(COLOR_DEBUG, str, newLine);
	}
}

void HAPLogger::logWarning(const String &str, bool newLine){
	if (HAPLogger::_logLevel >= LogLevel::WARNING) {
		colorPrint(COLOR_WARNING, str.c_str(), newLine);
	}
}

void HAPLogger::logVerbose(const String &str, bool newLine){
	if (HAPLogger::_logLevel >= LogLevel::VERBOSE) {
		colorPrint(COLOR_VERBOSE, str.c_str(), newLine);
	}
}

#if !defined(__APPLE__)
void HAPLogger::colorPrint(const char* color, const __FlashStringHelper * text, bool newLine){
	colorPrint(color, (PGM_P)text, newLine);
}
#endif

void HAPLogger::colorPrint(const char* color, int num, bool newLine) {
	_printer->print(color);
	_printer->print(num, DEC);

	if (newLine)
		_printer->println(COLOR_RESET);
	else
		_printer->print(COLOR_RESET);

#if defined( CORE_TEENSY )
	Serial.send_now();
#endif
}

void HAPLogger::colorPrint(const char* color, const char* text, bool newLine) {

	if (strcmp(text, "") != 0){
		_printer->print(color);
		_printer->print(text);

		if (newLine)
			_printer->println(COLOR_RESET);
		else
			_printer->print(COLOR_RESET);
	}

#if defined( CORE_TEENSY )
	Serial.send_now();
#endif
}


void HAPLogger::checkErrorOK(int err_code) {
	if (err_code != 0) {
		colorPrint(COLOR_ERROR, " ERROR: ", false);
		colorPrint(COLOR_ERROR, err_code, true);
	} else
		colorPrint(COLOR_GREEN, " OK", true);

	//	Serial.print("Free Heap: "); Serial.println(ESP.getFreeHeap());
}


void HAPLogger::checkError(int err_code) {
	if (err_code != 0) {
		colorPrint(COLOR_ERROR, " ERROR: ", false);
		colorPrint(COLOR_ERROR, err_code, true);
	}
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPLogger::setLogLevel(uint8_t lvl){
	_logLevel = (LogLevel)lvl;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPLogger::setLogLevel(LogLevel lvl){
	_logLevel = lvl;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
LogLevel HAPLogger::getLogLevel(){
	return _logLevel;
}


void HAPLogger::logOK(const char* color) {
	colorPrint(color, "OK", true);
}

void HAPLogger::setPrinter(Stream* printer) {
	_printer = printer;
}

void HAPLogger::flush(){
	_printer->flush();
}

/*
Stream* HAPLogger::stream(){
	return _printer;
}
*/
#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPLogger::printInfo(){
	_printer->println();
	_printer->println(F("These are the color definitions of the log output:"));
	LogE(F("ERROR   - This is an error message."), true);
	LogW(F("WARNING - This is a warning."), true);
	LogI(F("INFO    - This is information"), true);
	LogV(F("VERBOSE - These are verbose infos"), true);
	LogD(F("DEBUG   - And finally the debug messages"), true);
	_printer->println();
}



#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
void HAPLogger::printTeensyInfo(){
	progInfo();
	flexRamInfo();
}

size_t HAPLogger::print(const __FlashStringHelper *ifsh) {

    PGM_P p = reinterpret_cast<PGM_P>(ifsh);

    char buff[128] __attribute__ ((aligned(4)));

    auto len = strlen_P(p);

    size_t n = 0;

    while (n < len) {

        int to_write = std::min(sizeof(buff), len - n);

        memcpy_P(buff, p, to_write);

        auto written = _printer->write(buff, to_write);

        n += written;

        p += written;

        if (!written) {

            // Some error, write() should write at least 1 byte before returning

            break;

        }

    }

    return n;

}