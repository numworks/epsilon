#ifndef OMG_MEMORY_H
#define OMG_MEMORY_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

namespace OMG::Memory {

size_t AlignedSize(size_t realSize, size_t alignment);
bool Rotate(uint8_t* dst, uint8_t* src, size_t len);

constexpr uint32_t k_crcPolynomial = 0x04C11DB7;

constexpr uint32_t crc32EatByte(uint32_t crc, uint8_t data) {
  crc ^= data << 24;
  for (int i = 8; i--;) {
    crc = crc & 0x80000000 ? ((crc << 1) ^ k_crcPolynomial) : (crc << 1);
  }
  return crc;
}

constexpr uint32_t crc32String(const char* str) {
  assert(str);
  uint32_t crc = ~0u;
  for (size_t i = 0; str[i] != '\0'; i++) {
    crc = crc32EatByte(crc, str[i]);
  }
  return crc;
}

}  // namespace OMG::Memory

#endif
