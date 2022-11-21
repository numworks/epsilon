#ifndef POINCARE_EXPRESSION_INTEGER_H
#define POINCARE_EXPRESSION_INTEGER_H

#include <poincare_junior/src/memory/edition_reference.h>

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

  template <typename T>
  T to();
private:
  bool m_negative;
  union Digits {
    const uint8_t * m_digits;
    native_uint_t m_digit;
  };
  Digits m_digitAccessor;
  uint8_t m_numberOfDigits;
};

class Integer {
public:
  static EditionReference Addition(IntegerHandler a, IntegerHandler b);
};


}

#endif
