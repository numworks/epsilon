#ifndef OMG_PRINT_H
#define OMG_PRINT_H

#include <assert.h>
#include <omg/bit_helper.h>
#include <stdint.h>

namespace OMG {

/* TODO: merge Poincare::PrintInt, Poincare::PrintFloat and Poincare::Print
 * into Utils::Print
 * TODO: use Print::BinaryCharacterForDigit in poincare/src/integer.cpp */

namespace Print {

enum class Base : uint8_t {
  Binary = 2,
  Decimal = 10,
  Hexadecimal = 16
};

inline char CharacterForDigit(Base base, uint8_t d) {
  assert(d >= 0 && d < static_cast<uint8_t>(base));
  if (d >= 10) {
    return 'A' + d - 10;
  }
  return d + '0';
}

constexpr size_t LengthOfUInt32(Base base) { return OMG::BitHelper::numberOfBitsInType<uint32_t>() / OMG::BitHelper::numberOfBitsToCountUpTo(static_cast<uint8_t>(base)); }

int UInt32(Base base, uint32_t integer, char * buffer, int bufferSize);

}

}

#endif
