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
constexpr size_t numberOfBitsInType() {
  return sizeof(T) * k_numberOfBitsInByte;
}

template <typename T>
constexpr bool bitAtIndex(T mask, size_t i) {
  assert(i >= 0 && i < numberOfBitsInType<T>());
  return (mask >> i) & 1U;
}

template <typename T>
constexpr void setBitAtIndex(T & mask, size_t i, bool b) {
  assert(i < numberOfBitsInType<T>());
  T one = 1;
  if (b) {
    mask |= (one << i);
  } else {
    mask &= ~(one << i);
  }
}

constexpr inline size_t countLeadingZeros(unsigned int i) {
  return __builtin_clz(i);
}

constexpr inline size_t countTrailingZeros(unsigned int i) {
  return __builtin_ctz(i);
}

constexpr inline size_t numberOfOnes(unsigned int i) {
  return __builtin_popcount(i);
}

constexpr inline size_t indexOfMostSignificantBit(unsigned int i) {
  return numberOfBitsInType<unsigned int>() - countLeadingZeros(i);
}

constexpr inline size_t numberOfBitsToCountUpTo(unsigned int i) {
  assert(i >= 2);
  return indexOfMostSignificantBit(i - 1);
}

}  // namespace BitHelper
}  // namespace Ion

#endif /* ION_BIT_HELPER_H */
