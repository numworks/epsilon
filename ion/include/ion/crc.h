#ifndef ION_CRC_H
#define ION_CRC_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

namespace Ion {

// CRC32 : non xor-ed, non reversed, direct, polynomial 4C11DB7
// Only accepts whole 32bit values
uint32_t crc32DoubleWord(const uint32_t *data, size_t length);
uint32_t crc32Word(const uint16_t *data, size_t length);
uint32_t crc32Byte(const uint8_t *data, size_t length);

constexpr uint32_t k_crcPolynomial = 0x04C11DB7;

constexpr uint32_t crc32EatByte(uint32_t crc, uint8_t data) {
  crc ^= data << 24;
  for (int i = 8; i--;) {
    crc = crc & 0x80000000 ? ((crc << 1) ^ k_crcPolynomial) : (crc << 1);
  }
  return crc;
}

}  // namespace Ion

#endif
