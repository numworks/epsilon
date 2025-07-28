#ifndef OMG_PRINT_H
#define OMG_PRINT_H

#include <assert.h>
#include <omg/bit_helper.h>
#include <omg/enums.h>
#include <stdint.h>

namespace OMG {

/* TODO:
 * - merge IntLeft & IntRight into OMG::UInt32(Base::Decimal),
 * - move Poincare::PrintFloat here
 * - move Poincare::Print here
 */

namespace Print {

enum class LeadingZeros : bool { Trim = false, Keep = true };

inline char CharacterForDigit(Base base, uint8_t d) {
  assert(d >= 0 && d < static_cast<uint8_t>(base));
  if (d >= 10) {
    return 'A' + d - 10;
  }
  return d + '0';
}

inline constexpr uint8_t DigitForCharacter(char c) {
  assert(c >= '0');
  if (c <= '9') {
    return c - '0';
  }
  if (c <= 'F') {
    assert(c >= 'A');
    return c - 'A' + 10;
  }
  assert(c >= 'a' && c <= 'f');
  return c - 'a' + 10;
}

uint32_t ParseDecimalInt(const char* text, int maxNumberOfDigits);

constexpr size_t MaxLengthOfUInt32(Base base) {
  assert(OMG::BitHelper::numberOfBitsToCountUpTo(static_cast<uint8_t>(base)) >
         0);
  return OMG::BitHelper::numberOfBitsIn<uint32_t>() /
         OMG::BitHelper::numberOfBitsToCountUpTo(static_cast<uint8_t>(base));
}

constexpr size_t LengthOfUInt32(Base base, uint32_t integer) {
  return integer == 0 ? 1
                      : OMG::BitHelper::indexOfMostSignificantBit(integer) /
                                OMG::BitHelper::numberOfBitsToCountUpTo(
                                    static_cast<uint8_t>(base)) +
                            1;
}

int UInt32(Base base, uint32_t integer, LeadingZeros printLeadingZeros,
           char* buffer, int bufferSize);
int IntLeft(uint32_t integer, char* buffer, int bufferLength);
int IntRight(uint32_t integer, char* buffer, int bufferLength);

inline constexpr bool IsLowercaseLetter(char c) { return 'a' <= c && c <= 'z'; }

inline constexpr bool IsDigit(char c) { return '0' <= c && c <= '9'; }

}  // namespace Print

}  // namespace OMG

#endif
