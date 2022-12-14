#ifndef OMG_PRINT_H
#define OMG_PRINT_H

#include <assert.h>
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

int Integer(Base base, uint32_t integer, char * buffer, int bufferSize);

}

}

#endif
