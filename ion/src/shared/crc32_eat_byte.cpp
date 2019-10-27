#include <ion.h>

constexpr uint32_t polynomial = 0x04C11DB7;

uint32_t Ion::crc32EatByte(uint32_t crc, uint8_t data) {
    crc ^= data << 24;
    for (int i=8; i--;) {
      crc = crc & 0x80000000 ? ((crc<<1)^polynomial) : (crc << 1);
    }
    return crc;
}
