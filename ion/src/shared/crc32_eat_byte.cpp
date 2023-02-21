#include "crc32_eat_byte.h"

namespace Ion {

constexpr uint32_t polynomial = 0x04C11DB7;

uint32_t crc32EatByte(uint32_t crc, uint8_t data) {
  crc ^= data << 24;
  for (int i = 8; i--;) {
    crc = crc & 0x80000000 ? ((crc << 1) ^ polynomial) : (crc << 1);
  }
  return crc;
}

}  // namespace Ion
