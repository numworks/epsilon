#include <private/ieee754.h>

uint32_t ieee754man32(float x) {
  uint32_t* address = (uint32_t*)&x;
  return *address & 0x007FFFFF;
}

uint8_t ieee754exp32(float x) {
  uint32_t* address = (uint32_t*)&x;
  return (*address >> 23) & 0xFF;
}

uint64_t ieee754man64(double x) {
  uint64_t* address = (uint64_t*)&x;
  uint64_t mask = 0x000FFFFFFFFFFFFF;
  return *address & mask;
}

uint16_t ieee754exp64(double x) {
  uint64_t* address = (uint64_t*)&x;
  return (*address >> 52) & 0x7FF;
}
