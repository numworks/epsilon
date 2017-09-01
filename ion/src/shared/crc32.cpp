#include <ion.h>

constexpr uint32_t polynomial = 0x04C11DB7;

static uint32_t crc32(uint32_t crc, uint8_t data) {
    crc ^= data << 24;
    for (int i=8; i--;) {
      crc = crc & 0x80000000 ? ((crc<<1)^polynomial) : (crc << 1);
    }
    return crc;
}

uint32_t Ion::crc32(const uint32_t * data, size_t length) {
  uint32_t crc = 0xFFFFFFFF;
  for (size_t i=0; i<length; i++) {
    // FIXME: Assumes little-endian byte order!
    crc = ::crc32(crc, (uint8_t)((data[i] >> 24) & 0xFF));
    crc = ::crc32(crc, (uint8_t)((data[i] >> 16) & 0xFF));
    crc = ::crc32(crc, (uint8_t)((data[i] >> 8) & 0xFF));
    crc = ::crc32(crc, (uint8_t)(data[i] & 0xFF));
  }
  return crc;
}
