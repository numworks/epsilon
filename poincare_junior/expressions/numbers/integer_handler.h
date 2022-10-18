#ifndef POINCARE_EXPRESSIONS_INTEGER_HANDLER_H
#define POINCARE_EXPRESSIONS_INTEGER_HANDLER_H

#include "assert.h"
#include "stdint.h"

namespace Poincare {

typedef uint16_t half_native_uint_t;
typedef int32_t native_int_t;
typedef int64_t double_native_int_t;
typedef uint32_t native_uint_t;
typedef uint64_t double_native_uint_t;

class IntegerHandler final {
public:
  IntegerHandler(const uint8_t * digits, uint8_t numberOfDigits, bool negative) : m_negative(negative), m_digitAccessor({.m_digits = digits}), m_numberOfDigits(numberOfDigits) {}
  IntegerHandler(int32_t value) : IntegerHandler(value < 0 ? -value : value, value < 0) {}
  IntegerHandler(uint32_t value, bool negative = false) : m_negative(negative), m_digitAccessor({.m_digit = value}), m_numberOfDigits(value != 0 ? 1 : 0) {}
  float approximate();
private:
  bool m_negative;
  union Digits {
    const uint8_t * m_digits;
    uint32_t m_digit;
  };
  Digits m_digitAccessor;
  uint8_t m_numberOfDigits;
};

}

#endif
