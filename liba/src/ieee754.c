#include <private/ieee754.h>

uint32_t ieee754man32(float x) {
  union {
    float f;
    uint32_t i;
  } u;
  u.f = x;
  return (u.i & ((1<<23)-1));
}

uint8_t ieee754exp32(float x) {
  union {
    float f;
    uint32_t i;
  } u;
  u.f = x;
  return (u.i >> 23) & 0xFF;
}

uint64_t ieee754man64(double x) {
  union {
    double d;
    uint64_t i;
  } u;
  u.d = x;
  return (u.i & (((uint64_t)1<<52)-1));
}

uint16_t ieee754exp64(double x) {
  union {
    double d;
    uint64_t i;
  } u;
  u.d = x;
  return (uint16_t)((u.i >> 52) & 0x7FF);
}
