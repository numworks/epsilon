#include "value_block.h"

#include "bit"

#if __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace PoincareJ {

#if __EMSCRIPTEN__
template <>
uint32_t ValueBlock::get<uint32_t>() const {
  return *reinterpret_cast<const emscripten_align1_int*>(this);
}
#else
template <>
uint32_t ValueBlock::get<uint32_t>() const {
  return *reinterpret_cast<const uint32_t*>(this);
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

};  // namespace PoincareJ
