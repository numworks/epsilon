#ifndef ION_CRC_H
#define ION_CRC_H

#include <omg/memory.h>
#include <stddef.h>
#include <stdint.h>

namespace Ion {

// CRC32 : non xor-ed, non reversed, direct, polynomial 4C11DB7
// Only accepts whole 32bit values
uint32_t crc32DoubleWord(const uint32_t* data, size_t length);
uint32_t crc32Word(const uint16_t* data, size_t length);
uint32_t crc32Byte(const uint8_t* data, size_t length);

}  // namespace Ion

#endif
