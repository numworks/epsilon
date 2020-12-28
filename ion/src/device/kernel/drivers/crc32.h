#ifndef ION_DEVICE_KERNEL_DRIVERS_CRC32_H
#define ION_DEVICE_KERNEL_DRIVERS_CRC32_H

#include <stdint.h>
#include <stddef.h>

namespace Ion {
namespace Device {

// CRC32 : non xor-ed, non reversed, direct, polynomial 4C11DB7
uint32_t crc32Byte(const uint8_t * data, size_t length);
uint32_t crc32Word(const uint32_t * data, size_t length);
uint32_t crc32EatByte(uint32_t previousCRC, uint8_t data);

}
}

#endif
