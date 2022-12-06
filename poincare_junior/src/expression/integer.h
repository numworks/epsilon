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
  IntegerHandler(int8_t value) : IntegerHandler(value < 0 ? -value : value, value < 0) {}
  IntegerHandler(uint8_t value, bool negative) : m_negative(negative), m_digitAccessor({.m_digit = value}), m_numberOfDigits(value != 0 ? 1 : 0) {}

  uint8_t numberOfDigits() const { return m_numberOfDigits; }
  const uint8_t * digits() const {
    if (usesImmediateDigit()) {
      return &m_digitAccessor.m_digit;
    }
    return m_digitAccessor.m_digits;
  }
  uint8_t digit(int i) const {
    assert(m_numberOfDigits > i);
    return digits()[i];
  }
  int sign() const { return isZero() ? 0 : (m_negative ? -1 : 1); }

  void setSign(bool negative) {
    assert(negative != m_negative);
    m_negative = negative;
  }

  bool isOne() const { return (numberOfDigits() == 1 && digit(0) == 1 && !m_negative); };
  bool isTwo() const { return (numberOfDigits() == 1 && digit(0) == 2 && !m_negative); };
  bool isZero() const {
    assert(m_numberOfDigits != 0 || !m_negative); // TODO: should we represent -0?
    return m_numberOfDigits == 0;
  }

  bool isInt8() const {
    return m_numberOfDigits == 0 || (m_numberOfDigits <= 1 && digit(0) <= INT8_MAX);
  }
  operator int8_t() const { assert(isInt8()); return numberOfDigits() == 0 ? 0 : (m_negative ? -digit(0) : digit(0)); }
  bool isUint8() const {
    return numberOfDigits() <= 1 && !m_negative;
  }
  operator uint8_t() const { assert(isUint8()); return numberOfDigits() == 0 ? 0 : digit(0); }

  void pushDigitsOnEditionPool();
  template <typename T>
  T to();
private:
  bool usesImmediateDigit() const { return m_numberOfDigits == 1; }
  bool m_negative;
  union Digits {
    const uint8_t * m_digits;
    uint8_t m_digit;
  };
  Digits m_digitAccessor;
  uint8_t m_numberOfDigits;
};

class Integer {
public:
  static EditionReference PushNode(IntegerHandler integer);
  static EditionReference Addition(IntegerHandler a, IntegerHandler b);
};


}

#endif
