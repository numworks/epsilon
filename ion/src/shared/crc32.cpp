#include <ion.h>

uint32_t crc32Helper(const uint8_t * data, size_t length, bool wordAccess) {
  size_t uint32ByteLength = sizeof(uint32_t)/sizeof(uint8_t);
  uint32_t crc = 0xFFFFFFFF;
  size_t byteLength = (wordAccess ? length * uint32ByteLength : length);
  size_t wordLength = byteLength / uint32ByteLength;

  for (int i = 0; i < (int)wordLength; i++) {
    // FIXME: Assumes little-endian byte order!
    for (int j = uint32ByteLength-1; j >= 0; j--) {
      // scan byte by byte to avoid alignment issue when building for emscripten platform
      crc = Ion::crc32EatByte(crc, data[i*uint32ByteLength+j]);
    }
  }
  for (int i = (int) wordLength * uint32ByteLength; i < byteLength; i++) {
    crc = Ion::crc32EatByte(crc, data[i]);
  }
  return crc;
}

uint32_t Ion::crc32Word(const uint32_t * data, size_t length) {
  return crc32Helper((const uint8_t *) data, length, true);
}

uint32_t Ion::crc32Byte(const uint8_t * data, size_t length) {
  return crc32Helper(data, length, false);
}
