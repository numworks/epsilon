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

uint32_t Ion::crc32PaddedString(const char * s, int length) {
  /* s is a char array. Its length in Bytes is not necessarily a multiple of 4.
   * However, crc32 method awaits input in a form of uint32_t table. To limit
   * the use of additional memory, we compute 2 crc32:
   * - one corresponding to s with a byte length truncated to be a multiple of 4
   * - the other corresponds to the remaining chars in s padded with 0 to be 4
   *   bytes long
   * The CRC32 of s is the crc32 of both. */

  // CRC32 of the truncated string
  uint32_t c[2] = {0, 0};
  size_t crc32TruncatedInputSize = length*sizeof(char)/sizeof(uint32_t);
  c[0] = Ion::crc32((const uint32_t *)s, crc32TruncatedInputSize);

  // CRC32 of the tail of the string
  uint32_t tailName = 0;
  size_t tailLength = length - crc32TruncatedInputSize*sizeof(uint32_t)/sizeof(char);
  strlcpy((char *)&tailName, s+crc32TruncatedInputSize*sizeof(uint32_t)/sizeof(char), tailLength);
  c[1] = Ion::crc32(&tailName, 1);

  return Ion::crc32((const uint32_t *)c, 2);
}
