#include <private/ieee754.h>

uint32_t ieee754man(float x) {
  union {
    float f;
    uint32_t i;
  } u;
  u.f = x;
  return (u.i & ((1<<23)-1));
}

uint8_t ieee754exp(float x) {
  union {
    float f;
    uint32_t i;
  } u;
  u.f = x;
  return (u.i >> 23) & 0xFF;
}
