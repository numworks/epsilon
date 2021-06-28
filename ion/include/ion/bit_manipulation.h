#ifndef POINCARE_BIT_MANIPULATION_H
#define POINCARE_BIT_MANIPULATION_H

#include <stddef.h>
#include <stdint.h>

namespace Ion {
namespace BitHelper {

constexpr static size_t numberOfBitsInByte = 8;
constexpr static size_t numberOfBitsInInt = 32;

template <typename T>
bool bitInMaskAtIndex(T mask, int i) {
  int t = sizeof(T);
  assert(i >= 0 && i < numberOfBitsInByte * sizeof(T));
  return (mask >> i) & 1U;
}

template bool bitInMaskAtIndex<uint8_t>(uint8_t mask, int i);

template <typename T>
void setBitInMaskAtIndex(T mask, int i, bool b) {
  assert(i >= 0 && i < sizeof(T));
  if (b) {
    mask |= (1U << i);
  } else {
    mask &= ~(1U << i);
  }
}

inline size_t countLeadingZeroes(unsigned int i) {
  return __builtin_clz(i);
}

inline size_t countTrailingZeroes(unsigned int i) {
  return __builtin_ctz(i);
}

inline size_t numberOfOnes(unsigned int i) {
  return __builtin_popcount(i);
}

}  // namespace BitHelper
}  // namespace Ion

#endif /* POINCARE_BIT_MANIPULATION_H */
