//
// HAPConfigurationT41SPIFFSInt.cpp
// Homekit
//
//  Created on: 15.02.2021
//      Author: michael
//
#include "HAPConfigurationT41SPIFFSInt.hpp"
#include "HAPHelper.hpp"
#include "HAPLogger.hpp"


#define HAP_DEBUG_SPIFFS 1
#define LOG_PAGE_SIZE       256
#define HAP_SPIFFS_FLASH_SIZE   (1024 * 1024 * 3)   // use 3 MB

spiffs HAPConfigurationT41SPIFFSInt::_fs;

static u8_t spiffs_work_buf[LOG_PAGE_SIZE * 2];
static u8_t spiffs_fds[32 * 4];
static u8_t spiffs_cache_buf[(LOG_PAGE_SIZE + 32) * 4];

extern unsigned long _flashimagelen;
const unsigned long _startFS = 0x60000000UL + ((unsigned)&_flashimagelen & ~0x3FFF) + 0x4000;


#define LUT0(opcode, pads, operand) (FLEXSPI_LUT_INSTRUCTION((opcode), (pads), (operand)))
#define LUT1(opcode, pads, operand) (FLEXSPI_LUT_INSTRUCTION((opcode), (pads), (operand)) << 16)
#define CMD_SDR         FLEXSPI_LUT_OPCODE_CMD_SDR
#define ADDR_SDR        FLEXSPI_LUT_OPCODE_RADDR_SDR
#define READ_SDR        FLEXSPI_LUT_OPCODE_READ_SDR
#define WRITE_SDR       FLEXSPI_LUT_OPCODE_WRITE_SDR
#define PINS1           FLEXSPI_LUT_NUM_PADS_1
#define PINS4           FLEXSPI_LUT_NUM_PADS_4



HAPConfigurationT41SPIFFSInt::HAPConfigurationT41SPIFFSInt(){

}

HAPConfigurationT41SPIFFSInt::~HAPConfigurationT41SPIFFSInt(){

}


void HAPConfigurationT41SPIFFSInt::flash_wait(){
    FLEXSPI_LUT60 = LUT0(CMD_SDR, PINS1, 0x05) | LUT1(READ_SDR, PINS1, 1); // 05 = read status
    FLEXSPI_LUT61 = 0;
    uint8_t status;
    do {
        FLEXSPI_IPRXFCR = FLEXSPI_IPRXFCR_CLRIPRXF; // clear rx fifo
        FLEXSPI_IPCR0 = 0;
        FLEXSPI_IPCR1 = FLEXSPI_IPCR1_ISEQID(15) | FLEXSPI_IPCR1_IDATSZ(1);
        FLEXSPI_IPCMD = FLEXSPI_IPCMD_TRG;
        while (!(FLEXSPI_INTR & FLEXSPI_INTR_IPCMDDONE)) {
            asm("nop");
        }
        FLEXSPI_INTR = FLEXSPI_INTR_IPCMDDONE;
        status = *(uint8_t *)&FLEXSPI_RFDR0;
    } while (status & 1);
    FLEXSPI_MCR0 |= FLEXSPI_MCR0_SWRESET; // purge stale data from FlexSPI's AHB FIFO
    while (FLEXSPI_MCR0 & FLEXSPI_MCR0_SWRESET) ; // wait
    __enable_irq();
}

// write bytes into flash memory (which is already erased to 0xFF)
void HAPConfigurationT41SPIFFSInt::flash_write(void *addr, const void *data, uint32_t len){
    __disable_irq();
    FLEXSPI_LUTKEY = FLEXSPI_LUTKEY_VALUE;
    FLEXSPI_LUTCR = FLEXSPI_LUTCR_UNLOCK;
    FLEXSPI_IPCR0 = 0;
    FLEXSPI_LUT60 = LUT0(CMD_SDR, PINS1, 0x06); // 06 = write enable
    FLEXSPI_LUT61 = 0;
    FLEXSPI_LUT62 = 0;
    FLEXSPI_LUT63 = 0;
    FLEXSPI_IPCR1 = FLEXSPI_IPCR1_ISEQID(15);
    FLEXSPI_IPCMD = FLEXSPI_IPCMD_TRG;
    arm_dcache_delete(addr, len); // purge old data from ARM's cache
    while (!(FLEXSPI_INTR & FLEXSPI_INTR_IPCMDDONE)) ; // wait
    FLEXSPI_INTR = FLEXSPI_INTR_IPCMDDONE;
    FLEXSPI_LUT60 = LUT0(CMD_SDR, PINS1, 0x32) | LUT1(ADDR_SDR, PINS1, 24); // 32 = quad write
    FLEXSPI_LUT61 = LUT0(WRITE_SDR, PINS4, 1);
    FLEXSPI_IPTXFCR = FLEXSPI_IPTXFCR_CLRIPTXF; // clear tx fifo
    FLEXSPI_IPCR0 = (uint32_t)addr & 0x007FFFFF;
    FLEXSPI_IPCR1 = FLEXSPI_IPCR1_ISEQID(15) | FLEXSPI_IPCR1_IDATSZ(len);
    FLEXSPI_IPCMD = FLEXSPI_IPCMD_TRG;
    const uint8_t *src = (const uint8_t *)data;
    uint32_t n;
    while (!((n = FLEXSPI_INTR) & FLEXSPI_INTR_IPCMDDONE)) {
        if (n & FLEXSPI_INTR_IPTXWE) {
        uint32_t wrlen = len;
        if (wrlen > 8) wrlen = 8;
        if (wrlen > 0) {
            memcpy((void *)&FLEXSPI_TFDR0, src, wrlen);
            src += wrlen;
            len -= wrlen;
        }
        FLEXSPI_INTR = FLEXSPI_INTR_IPTXWE;
        }
    }
    FLEXSPI_INTR = FLEXSPI_INTR_IPCMDDONE | FLEXSPI_INTR_IPTXWE;
    flash_wait();
}

// erase a 4K sector
void HAPConfigurationT41SPIFFSInt::flash_erase_sector(void *addr){
    __disable_irq();
    FLEXSPI_LUTKEY = FLEXSPI_LUTKEY_VALUE;
    FLEXSPI_LUTCR = FLEXSPI_LUTCR_UNLOCK;
    FLEXSPI_LUT60 = LUT0(CMD_SDR, PINS1, 0x06); // 06 = write enable
    FLEXSPI_LUT61 = 0;
    FLEXSPI_LUT62 = 0;
    FLEXSPI_LUT63 = 0;
    FLEXSPI_IPCR0 = 0;
    FLEXSPI_IPCR1 = FLEXSPI_IPCR1_ISEQID(15);
    FLEXSPI_IPCMD = FLEXSPI_IPCMD_TRG;
    arm_dcache_delete((void *)((uint32_t)addr & 0xFFFFF000), 4096); // purge data from cache
    while (!(FLEXSPI_INTR & FLEXSPI_INTR_IPCMDDONE)) ; // wait
    FLEXSPI_INTR = FLEXSPI_INTR_IPCMDDONE;
    FLEXSPI_LUT60 = LUT0(CMD_SDR, PINS1, 0x20) | LUT1(ADDR_SDR, PINS1, 24); // 20 = sector erase
    FLEXSPI_IPCR0 = (uint32_t)addr & 0x007FF000;
    FLEXSPI_IPCR1 = FLEXSPI_IPCR1_ISEQID(15);
    FLEXSPI_IPCMD = FLEXSPI_IPCMD_TRG;
    while (!(FLEXSPI_INTR & FLEXSPI_INTR_IPCMDDONE)) ; // wait
    FLEXSPI_INTR = FLEXSPI_INTR_IPCMDDONE;
    flash_wait();
}

s32_t HAPConfigurationT41SPIFFSInt::read(u32_t addr, u32_t size, u8_t *dst) {
    memcpy((void *)dst, (void *)addr, size);
    return SPIFFS_OK;
}

s32_t HAPConfigurationT41SPIFFSInt::write(u32_t addr, u32_t size, u8_t *src) {
    flash_write((void *)addr, (const void *)src, size);

#if HAP_DEBUG_SPIFFS
    Serial.printf("write %0X len %d : ", addr, size);
    for (unsigned ii = 0; ii < size; ii++) Serial.printf("%0x ",src[ii]);
    Serial.println();
#endif

    return SPIFFS_OK;
}

s32_t HAPConfigurationT41SPIFFSInt::erase(u32_t addr, u32_t size) {
    int s = size;
    while (s > 0) {
        flash_erase_sector((void *)addr);
        addr += 4096;
        s -= 4096;
    }
#if HAP_DEBUG_SPIFFS
    Serial.printf("erase %0X len %d\n", addr, size);
#endif
    return SPIFFS_OK;
}

#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
bool HAPConfigurationT41SPIFFSInt::mount() {

    spiffs_config cfg;

    cfg.phys_size = HAP_SPIFFS_FLASH_SIZE; // use 1 MB flash
    cfg.phys_addr = _startFS; // start spiffs here
    cfg.phys_erase_block = 4096;
    cfg.log_block_size = cfg.phys_erase_block; // let us not complicate things
    cfg.log_page_size = LOG_PAGE_SIZE; // as we said

    cfg.hal_read_f = HAPConfigurationT41SPIFFSInt::read;
    cfg.hal_write_f = HAPConfigurationT41SPIFFSInt::write;
    cfg.hal_erase_f = HAPConfigurationT41SPIFFSInt::erase;

    int res = SPIFFS_mount(&_fs,
                            &cfg,
                            spiffs_work_buf,
                            spiffs_fds,
                            sizeof(spiffs_fds),
                            spiffs_cache_buf,
                            sizeof(spiffs_cache_buf),
                            0);
#if HAP_DEBUG_SPIFFS
    LogD("SPIFFS mount result: " + String(res), true);
    Serial.printf("mount address 0x%X res: %i\n", cfg.phys_addr, res);
#endif
    return (res == 0);
}


#if defined(ARDUINO_TEENSY41)
FLASHMEM
#endif
bool HAPConfigurationT41SPIFFSInt::begin(){
    return mount();
}


size_t HAPConfigurationT41SPIFFSInt::readBytes(const char* label, uint8_t* output, const size_t expectedDataLen){
    spiffs_file  fd = SPIFFS_open(&_fs, label, SPIFFS_RDWR, 0);
    if (fd < 0) {
        // Serial.printf("errno %i\n", SPIFFS_errno(&_fs));
        LogE("ERROR: Failed to read SPIFFS file " + String(label) + ": Reason: " + String(SPIFFS_errno(&_fs)), true);
        SPIFFS_close(&_fs, fd);
        return 0;
    }

    size_t read = SPIFFS_read(&_fs, fd, (u8_t *)output, expectedDataLen);
    if (read < 0) {
        LogE("ERROR: Failed to read from SPIFFS file " + String(label) + ": Reason: " + String(SPIFFS_errno(&_fs)), true);
        read = 0;
    }

    SPIFFS_close(&_fs, fd);
    return read;
}

size_t HAPConfigurationT41SPIFFSInt::writeBytes(const char* label, const uint8_t* input, const size_t expectedDataLen){

    Serial.println("1");
    Serial.send_now();

    spiffs_file fd = SPIFFS_open(&_fs, label, SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR, 0);
    Serial.println("2");
    Serial.send_now();

    if (fd < 0) {
        // Serial.printf("errno %i\n", SPIFFS_errno(&_fs));
        LogE("ERROR: Failed to open SPIFFS file " + String(label) + ": Reason: " + String(SPIFFS_errno(&_fs)), true);
        return 0;
    }
    size_t written = SPIFFS_write(&_fs, fd, (u8_t *)input, expectedDataLen);
    if (written < 0) {
        LogE("ERROR: Failed to write to SPIFFS file " + String(label) + ": Reason: " + String(SPIFFS_errno(&_fs)), true);
        written = 0;
    }

    Serial.println("3");
    Serial.send_now();
    SPIFFS_close(&_fs, fd);
    Serial.println("4");
    Serial.send_now();
    SPIFFS_fflush(&_fs, fd);

    return written;
}

size_t HAPConfigurationT41SPIFFSInt::getBytesLength(const char* label){
    spiffs_stat s;

    int res = SPIFFS_stat(&_fs, label, &s);

    if (res < 0) return 0;
    return s.size;
}


bool HAPConfigurationT41SPIFFSInt::getBytesForPlugin(const char* name, uint8_t* data, size_t dataSize){
    char label[20];
	sprintf(label, "p%s", name);
    size_t read = readBytes(label, data, dataSize);
    if (read == dataSize) return true;

    return false;
}

void HAPConfigurationT41SPIFFSInt::reset(){

}

size_t HAPConfigurationT41SPIFFSInt::getDataLengthForPlugin(const char* name){
    char label[20];
	sprintf(label, "p%s", name);
	return getBytesLength(label);
}

void HAPConfigurationT41SPIFFSInt::validateConfig(){

    uint8_t magNum[2];
    HAPHelper::u16_to_u8(HAP_SPIFFS_MAGIC_BYTE, magNum);

    writeBytes("validate", magNum, 2);
}

bool HAPConfigurationT41SPIFFSInt::validConfig(){

    uint8_t magNum[2];
    size_t res = readBytes("validate", magNum, 2);

    if (res == 0) return false;

    uint16_t magicNum = HAPHelper::u8_to_u16(magNum);
    return (magicNum == HAP_SPIFFS_MAGIC_BYTE);
}