#ifndef UTILS_ARITHMETIC_H
#define UTILS_ARITHMETIC_H

#include <omg/bit_helper.h>
#include <stddef.h>

namespace Arithmetic {

size_t Gcd(size_t a, size_t b);

// TODO should be std::gcd
constexpr uint64_t GcdU64(uint64_t i, uint64_t j) {
  // it has this special name to avoid issues when size_t = uint64_t
  do {
    if (i == 0) {
      return j;
    }
    if (j == 0) {
      return i;
    }
    if (i > j) {
      i = i - j * (i / j);
    } else {
      j = j - i * (j / i);
    }
  } while (true);
}

constexpr int64_t GcdI64(int64_t i, int64_t j) {
  return GcdU64(i >= 0 ? i : -i, j >= 0 ? j : -j);
}

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
