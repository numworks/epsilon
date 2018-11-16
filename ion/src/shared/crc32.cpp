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
  const uint8_t * dataByte = (const uint8_t *)data;
  size_t uint32ByteLength = sizeof(uint32_t)/sizeof(uint8_t);
  uint32_t crc = 0xFFFFFFFF;
  for (int i = 0; i < (int)length; i++) {
  // FIXME: Assumes little-endian byte order!
    for (int j = uint32ByteLength-1; j >= 0; j--) {
      // scan byte by byte to avoid alignment issue when building for emscripten platform
      crc = ::crc32(crc, dataByte[i*uint32ByteLength+j]);
    }
  }
  return crc;
}
