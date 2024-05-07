#ifndef ION_CRC_H
#define ION_CRC_H

#include <omg/memory.h>
#include <stddef.h>
#include <stdint.h>

namespace Ion {

// CRC32 : non xor-ed, non reversed, direct, polynomial 4C11DB7
// Only accepts whole 32bit values
uint32_t crc32DoubleWord(const uint32_t *data, size_t length);
uint32_t crc32Word(const uint16_t *data, size_t length);
uint32_t crc32Byte(const uint8_t *data, size_t length);

/* FIXME This function only exists to avoid having to update epsilon-core,
 * remove it when appropriate. */
constexpr uint32_t crc32EatByte(uint32_t crc, uint8_t data) {
  return OMG::Memory::crc32EatByte(crc, data);
}

}  // namespace Ion

#endif
