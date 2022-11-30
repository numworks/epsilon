#ifndef UTILS_BIT_H
#define UTILS_BIT_H

namespace Bit {

// TODO use in ion/src/device/shared/regs/register.h
// TODO merge with ion/include/bit_helper.h

constexpr static uint8_t k_numberOfBitsInByte = 8;

template<typename T>
constexpr static T bitRangeMask(uint8_t high, uint8_t low) {
  // Same comment as for getBitRange: we should assert (high-low+1) < 8*sizeof(T)
  return ((((T)1)<<(high-low+1))-1)<<low;
}

template<typename T>
constexpr static T getBitRange(T value, uint8_t high, uint8_t low) {
  /* "Shift behavior is undefined if the right operand is negative, or greater
   * than or equal to the length in bits of the promoted left operand" according
   * to C++ spec. */
  assert(low < 8*sizeof(T));
  return (value & bitRangeMask<T>(high,low)) >> low;
}

template<typename T>
constexpr static T getByteAtIndex(T value, uint8_t index) {
  return getBitRange(value, (index + 1) * k_numberOfBitsInByte - 1, index * k_numberOfBitsInByte);
}

}

#endif
