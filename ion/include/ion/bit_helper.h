#ifndef ION_BIT_HELPER_H
#define ION_BIT_HELPER_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

namespace Ion {
namespace BitHelper {

constexpr static size_t k_numberOfBitsInByte = 8;
constexpr static size_t k_numberOfBitsInInt = sizeof(int) * k_numberOfBitsInByte;

template <typename T>
size_t numberOfBitsInType() {
  return sizeof(T) * k_numberOfBitsInByte;
}

template <typename T>
bool bitAtIndex(T mask, size_t i) {
  assert(i >= 0 && i < numberOfBitsInType<T>());
  return (mask >> i) & 1U;
}

template <typename T>
void setBitAtIndex(T & mask, size_t i, bool b) {
  assert(i < numberOfBitsInType<T>());
  constexpr static T one = 1;
  if (b) {
    mask |= (one << i);
  } else {
    mask &= ~(one << i);
  }
}

inline size_t countLeadingZeros(unsigned int i) {
  return __builtin_clz(i);
}

inline size_t countTrailingZeros(unsigned int i) {
  return __builtin_ctz(i);
}

inline size_t numberOfOnes(unsigned int i) {
  return __builtin_popcount(i);
}

}  // namespace BitHelper
}  // namespace Ion

#endif /* ION_BIT_HELPER_H */
