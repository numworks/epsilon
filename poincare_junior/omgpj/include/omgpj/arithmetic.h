#ifndef UTILS_ARITHMETIC_H
#define UTILS_ARITHMETIC_H

#include <omg/bit_helper.h>
#include <stddef.h>

namespace Arithmetic {

size_t Gcd(size_t a, size_t b);

template <typename T>
T constexpr CeilDivision(T numerator, T denominator) {
  return (numerator + denominator - 1) / denominator;
}

constexpr size_t NumberOfDigits(uint32_t value) {
  return CeilDivision<size_t>(
      OMG::BitHelper::numberOfBitsToCountUpTo(value + 1),
      OMG::BitHelper::k_numberOfBitsInByte);
}

}  // namespace Arithmetic

#endif
