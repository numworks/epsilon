#ifndef ION_SHARED_CRC32_EAT_BYTE_H
#define ION_SHARED_CRC32_EAT_BYTE_H

#include <stdint.h>

namespace Ion {

uint32_t crc32EatByte(uint32_t crc, uint8_t data);

}

#endif
