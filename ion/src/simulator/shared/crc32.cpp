#include <ion/crc.h>

namespace Ion {

constexpr size_t k_uint32ByteLength = sizeof(uint32_t) / sizeof(uint8_t);

uint32_t crc32Byte(const uint8_t *data, size_t length) {
  if (length == 0) {
    return 0;
  }
  assert(data != nullptr);
  uint32_t crc = 0xFFFFFFFF;
  size_t lengthInDoubleWords = length / k_uint32ByteLength;

  for (size_t i = 0; i < lengthInDoubleWords; i++) {
    // FIXME: Assumes little-endian byte order!
    for (int j = k_uint32ByteLength - 1; j >= 0; j--) {
      // scan byte by byte to avoid alignment issue when building for emscripten
      // platform
      crc = OMG::Memory::crc32EatByte(crc, data[i * k_uint32ByteLength + j]);
    }
  }
  for (size_t i = lengthInDoubleWords * k_uint32ByteLength; i < length; i++) {
    crc = OMG::Memory::crc32EatByte(crc, data[i]);
  }
  return crc;
}

uint32_t crc32Word(const uint16_t *data, size_t length) {
  return crc32Byte(reinterpret_cast<const uint8_t *>(data),
                   length * (sizeof(uint16_t) / sizeof(uint8_t)));
}

uint32_t crc32DoubleWord(const uint32_t *data, size_t length) {
  return crc32Byte(reinterpret_cast<const uint8_t *>(data),
                   length * k_uint32ByteLength);
}

}  // namespace Ion
