#include <ion.h>
#include <ion/src/shared/crc32_eat_byte.h>

namespace Ion {

static uint32_t crc32Helper(const uint8_t *data, size_t length,
                            bool wordAccess) {
  if (length == 0) {
    return 0;
  }
  assert(data != nullptr);
  size_t uint32ByteLength = sizeof(uint32_t) / sizeof(uint8_t);
  uint32_t crc = 0xFFFFFFFF;
  size_t byteLength = (wordAccess ? length * uint32ByteLength : length);
  size_t wordLength = byteLength / uint32ByteLength;

  for (size_t i = 0; i < wordLength; i++) {
    // FIXME: Assumes little-endian byte order!
    for (int j = uint32ByteLength - 1; j >= 0; j--) {
      // scan byte by byte to avoid alignment issue when building for emscripten
      // platform
      crc = crc32EatByte(crc, data[i * uint32ByteLength + j]);
    }
  }
  for (size_t i = wordLength * uint32ByteLength; i < byteLength; i++) {
    crc = crc32EatByte(crc, data[i]);
  }
  return crc;
}

uint32_t crc32Word(const uint32_t *data, size_t length) {
  return crc32Helper(reinterpret_cast<const uint8_t *>(data), length, true);
}

uint32_t crc32Byte(const uint8_t *data, size_t length) {
  return crc32Helper(data, length, false);
}

}  // namespace Ion
