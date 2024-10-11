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

// constexpr version of strlen
constexpr static int StringLength(const char* string) {
  int result = 0;
  while (string[result] != 0) {
    result++;
  }
  return result;
}

int UInt32(Base base, uint32_t integer, LeadingZeros printLeadingZeros,
           char* buffer, int bufferSize);
int IntLeft(uint32_t integer, char* buffer, int bufferLength);
int IntRight(uint32_t integer, char* buffer, int bufferLength);

inline constexpr bool IsLowercaseLetter(char c) { return 'a' <= c && c <= 'z'; }

inline constexpr bool IsDigit(char c) { return '0' <= c && c <= '9'; }

/* FIXME : This can be replaced by std::string_view when moving to C++17 */
constexpr static bool StringsAreEqual(const char* s1, const char* s2) {
  return *s1 == *s2 &&
         ((*s1 == '\0' && *s2 == '\0') || StringsAreEqual(s1 + 1, s2 + 1));
}

}  // namespace Print

}  // namespace OMG

#endif
