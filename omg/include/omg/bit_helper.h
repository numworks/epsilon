#ifndef OMG_BIT_HELPER_H
#define OMG_BIT_HELPER_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

namespace OMG {
namespace BitHelper {

constexpr static size_t k_numberOfBitsInByte = 8;
constexpr static size_t k_numberOfBitsInInt = sizeof(int) * k_numberOfBitsInByte;
constexpr static size_t k_numberOfBitsInUint32 = sizeof(uint32_t) * k_numberOfBitsInByte;

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

constexpr inline size_t countLeadingZeros(uint32_t i) {
  return __builtin_clz(i);
}

constexpr inline size_t countTrailingZeros(uint32_t i) {
  return __builtin_ctz(i);
}

constexpr inline size_t numberOfOnes(uint32_t i) {
  return __builtin_popcount(i);
}

constexpr inline size_t indexOfMostSignificantBit(uint32_t i) {
  return numberOfBitsInType<uint32_t>() - countLeadingZeros(i) - 1;
}

constexpr inline size_t numberOfBitsToCountUpTo(uint32_t i) {
  assert(i >= 2);
  return indexOfMostSignificantBit(i - 1) + 1;
}

template <typename T>
uint8_t log2(T v) {
  constexpr int nativeUnsignedIntegerBitCount = k_numberOfBitsInByte * sizeof(T);
  static_assert(nativeUnsignedIntegerBitCount < 256, "uint8_t cannot contain the log2 of a templated class T");
  for (uint8_t i = 0; i < nativeUnsignedIntegerBitCount; i++) {
    if (v < (static_cast<T>(1) << i)) {
      return i;
    }
  }
  return numberOfBitsInType<T>();
}

}
}

#endif
