#ifndef OMG_BIT_HELPER_H
#define OMG_BIT_HELPER_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include <bit>

namespace OMG {
namespace BitHelper {

constexpr static uint8_t k_numberOfBitsInByte = 8;
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
  return std::countl_zero(i);
}

constexpr inline size_t numberOfOnes(uint32_t i) { return std::popcount(i); }

constexpr inline size_t indexOfMostSignificantBit(uint32_t i) {
  return numberOfBitsIn<uint32_t>() - countLeadingZeros(i) - 1;
}

constexpr inline size_t numberOfBitsToCountUpTo(uint64_t i) {
  return i == 0 ? 0 : std::bit_width(i - 1);
}

template <typename T>
uint8_t log2(T v) {
  static_assert(numberOfBitsIn<T>() < 256,
                "uint8_t cannot contain the log2 of a templated class T");
  return std::bit_width(v);
}

// TODO use in ion/src/device/shared/regs/register.h
// TODO merge with ion/include/bit_helper.h

template <typename T>
constexpr static T bitRangeMask(uint8_t high, uint8_t low) {
  // Same comment as for getBitRange: we should assert (high-low+1) <
  // 8*sizeof(T)
  return ((((T)1) << (high - low + 1)) - 1) << low;
}

template <typename T>
constexpr static T getBitRange(T value, uint8_t high, uint8_t low) {
  /* "Shift behavior is undefined if the right operand is negative, or greater
   * than or equal to the length in bits of the promoted left operand" according
   * to C++ spec. */
  assert(low < 8 * sizeof(T));
  return (value & bitRangeMask<T>(high, low)) >> low;
}

template <typename T>
constexpr static T getByteAtIndex(T value, uint8_t index) {
  return getBitRange(value, (index + 1) * k_numberOfBitsInByte - 1,
                     index * k_numberOfBitsInByte);
}

}  // namespace BitHelper
}  // namespace OMG

#endif
