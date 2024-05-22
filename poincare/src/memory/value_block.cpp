#include "value_block.h"

#include <string.h>

#include <bit>

#if __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace Poincare::Internal {

#if __EMSCRIPTEN__
template <>
uint32_t ValueBlock::get<uint32_t>() const {
  return *reinterpret_cast<const emscripten_align1_int*>(this);
}
#else
template <>
uint16_t ValueBlock::get<uint16_t>() const {
  return *reinterpret_cast<const uint16_t*>(this);
}

template <>
uint32_t ValueBlock::get<uint32_t>() const {
  /* Unaligned 32-bits accesses are ok on the f7 but not on the h7.
   * The portable way to do unaligned accesses is to use memcpy and hope for the
   * compiler to optimize it away when possible. However it is not optimized
   * here therefore we build it manually. */
  uint32_t value = static_cast<const ValueBlock*>(nextNth(2))->get<uint16_t>();
  value <<= 16;
  value |= get<uint16_t>();
  return value;
}
#endif

template <>
uint64_t ValueBlock::get<uint64_t>() const {
  // 64 bit unaligned accesses need to be done in 2 * 32bits on device
  uint64_t value = static_cast<const ValueBlock*>(nextNth(4))->get<uint32_t>();
  value <<= 32;
  value |= get<uint32_t>();
  return value;
}

#if __EMSCRIPTEN__
template <>
float ValueBlock::get<float>() const {
  return *reinterpret_cast<const emscripten_align1_float*>(this);
}

template <>
double ValueBlock::get<double>() const {
  return *reinterpret_cast<const emscripten_align1_double*>(this);
}
#else
template <>
float ValueBlock::get<float>() const {
  return std::bit_cast<float, uint32_t>(get<uint32_t>());
}

template <>
double ValueBlock::get<double>() const {
  return std::bit_cast<double, uint64_t>(get<uint64_t>());
}
#endif

template <typename T>
void ValueBlock::set(T value) {
  memcpy(this, &value, sizeof(value));
}

template void ValueBlock::set(float);
template void ValueBlock::set(double);

};  // namespace Poincare::Internal
