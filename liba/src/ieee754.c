#include <private/ieee754.h>
#include "external/softfloat/include/internals.h"

uint32_t ieee754man32(float x) {
  uint32_t * address = (uint32_t *)&x;
  return fracF32UI( *address );
}

uint8_t ieee754exp32(float x) {
  uint32_t * address = (uint32_t *)&x;
  return expF32UI( *address );
}

uint64_t ieee754man64(double x) {
  uint64_t * address = (uint64_t *)&x;
  return fracF64UI( *address );
}

uint16_t ieee754exp64(double x) {
  uint64_t * address = (uint64_t *)&x;
  return expF64UI( *address );
}
