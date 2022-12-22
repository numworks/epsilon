#ifndef OMG_PRINT_H
#define OMG_PRINT_H

#include <assert.h>
#include <omg/bit_helper.h>
#include <omg/enums.h>
#include <stdint.h>

namespace OMG {

/* TODO:
 * - merge Poincare::PrintInt into OMG::UInt32(Base::Decimal),
 * - move Poincare::PrintFloat here
 * - move Poincare::Print here
 */

namespace Print {

enum class LeadingZeros : bool {
  Trim = false,
  Keep = true
};

inline char CharacterForDigit(Base base, uint8_t d) {
  assert(d >= 0 && d < static_cast<uint8_t>(base));
  if (d >= 10) {
    return 'A' + d - 10;
  }
  return d + '0';
}

inline uint8_t DigitForCharacter(char c) {
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

constexpr size_t MaxLengthOfUInt32(Base base) { return OMG::BitHelper::numberOfBitsInType<uint32_t>() / OMG::BitHelper::numberOfBitsToCountUpTo(static_cast<uint8_t>(base)); }

constexpr size_t LengthOfUInt32(Base base, uint32_t integer) { return integer == 0 ? 1 : OMG::BitHelper::indexOfMostSignificantBit(integer) / OMG::BitHelper::numberOfBitsToCountUpTo(static_cast<uint8_t>(base)) + 1; }

int UInt32(Base base, uint32_t integer, LeadingZeros printLeadingZeros, char * buffer, int bufferSize);

}

}

#endif
