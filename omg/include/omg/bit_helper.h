#ifndef OMG_BIT_HELPER_H
#define OMG_BIT_HELPER_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

namespace OMG {
namespace BitHelper {

constexpr static size_t k_numberOfBitsInByte = 8;
constexpr static size_t k_numberOfBitsInInt =
    sizeof(int) * k_numberOfBitsInByte;
constexpr static size_t k_numberOfBitsInUint32 =
    sizeof(uint32_t) * k_numberOfBitsInByte;

template <typename T>
constexpr size_t numberOfBitsIn() {
  return sizeof(T) * k_numberOfBitsInByte;
}

template <typename T>
constexpr size_t numberOfBitsIn(const T&) {
  return numberOfBitsIn<T>();
}

template <typename T>
constexpr bool bitAtIndex(T mask, size_t i) {
  assert(i >= 0 && i < numberOfBitsIn<T>());
  return (mask >> i) & 1U;
}
template <typename T, typename I>
constexpr bool bitAtIndex(T mask, I i) {
  return bitAtIndex(mask, static_cast<size_t>(i));
}

template <typename T>
constexpr T bitsBetweenIndexes(T bits, size_t high, size_t low) {
  return (bits >> low) & ((static_cast<T>(1) << (high - low + 1)) - 1);
}
template <typename T, typename I>
constexpr T bitsBetweenIndexes(T bits, I high, I low) {
  return bitsBetweenIndexes<T>(bits, static_cast<size_t>(high),
                               static_cast<size_t>(low));
}

template <typename T>
constexpr T withBitsBetweenIndexes(T bits, size_t high, size_t low, T value) {
  T mask = ((static_cast<T>(1) << (high - low + 1)) - static_cast<T>(1)) << low;
  return (bits & ~mask) | ((value << low) & mask);
}

template <typename T>
constexpr void setBitsBetweenIndexes(T& bits, size_t high, size_t low,
                                     T value) {
  bits = withBitsBetweenIndexes(bits, high, low, value);
}
template <typename T, typename I>
constexpr void setBitsBetweenIndexes(T& bits, I high, I low, T value) {
  setBitsBetweenIndexes<T>(bits, static_cast<size_t>(high),
                           static_cast<size_t>(low), value);
}

template <typename T>
constexpr void setBitAtIndex(T& mask, size_t i, bool b) {
  mask = withBitsBetweenIndexes(mask, i, i, static_cast<T>(b));
}
template <typename T, typename I>
constexpr void setBitAtIndex(T& mask, I i, bool b) {
  setBitAtIndex(mask, static_cast<size_t>(i), b);
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
  return numberOfBitsIn<uint32_t>() - countLeadingZeros(i) - 1;
}

constexpr inline size_t numberOfBitsToCountUpTo(uint32_t i) {
  return i <= 1 ? 0 : indexOfMostSignificantBit(i - 1) + 1;
}

template <typename T>
uint8_t log2(T v) {
  constexpr int nativeUnsignedIntegerBitCount =
      k_numberOfBitsInByte * sizeof(T);
  static_assert(nativeUnsignedIntegerBitCount < 256,
                "uint8_t cannot contain the log2 of a templated class T");
  for (uint8_t i = 0; i < nativeUnsignedIntegerBitCount; i++) {
    if (v < (static_cast<T>(1) << i)) {
      return i;
    }
  }
  return numberOfBitsIn<T>();
}

}  // namespace BitHelper
}  // namespace OMG

#endif
