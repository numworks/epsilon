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
  IntegerHandler(const uint8_t * digits, uint8_t numberOfDigits, NonStrictSign sign) : m_sign(sign), m_digitAccessor({.m_digits = digits}), m_numberOfDigits(numberOfDigits) {}
  IntegerHandler(int8_t value) : IntegerHandler(std::abs(value), value >= 0 ? NonStrictSign::Positive : NonStrictSign::Negative) {}
  IntegerHandler(uint8_t value, NonStrictSign sign) : m_sign(sign), m_digitAccessor({.m_digit = value}), m_numberOfDigits(value != 0 ? 1 : 0) {}

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
  StrictSign sign() const { return isZero() ? StrictSign::Null : static_cast<StrictSign>(m_sign); }
  void setSign(NonStrictSign sign) {
    m_sign = sign;
  }

  bool isOne() const { return (numberOfDigits() == 1 && digit(0) == 1 && m_sign == NonStrictSign::Positive); };
  bool isMinusOne() const { return (numberOfDigits() == 1 && digit(0) == 1 && m_sign == NonStrictSign::Negative); };
  bool isTwo() const { return (numberOfDigits() == 1 && digit(0) == 2 && m_sign == NonStrictSign::Positive); };
  bool isZero() const {
    assert(m_numberOfDigits != 0 || m_sign == NonStrictSign::Positive); // TODO: should we represent -0?
    return m_numberOfDigits == 0;
  }

  template <typename T>
  bool isSignedType() const {
    size_t maxNumberOfDigits = sizeof(T)/sizeof(uint8_t);
    return m_numberOfDigits < maxNumberOfDigits || (m_numberOfDigits == maxNumberOfDigits && digit(maxNumberOfDigits - 1) <= INT8_MAX);
  }
  template <typename T>
  bool isUnsignedType() const {
    size_t maxNumberOfDigits = sizeof(T)/sizeof(uint8_t);
    return m_numberOfDigits <= maxNumberOfDigits && m_sign == NonStrictSign::Positive;
  }
  operator int8_t() const { assert(isSignedType<int8_t>()); return numberOfDigits() == 0 ? 0 : static_cast<int8_t>(m_sign) * digit(0); }
  operator uint8_t() const { assert(isUnsignedType<uint8_t>()); return numberOfDigits() == 0 ? 0 : digit(0); }

  void pushDigitsOnEditionPool();
  template <typename T>
  T to();
private:
  bool usesImmediateDigit() const { return m_numberOfDigits == 1; }
  NonStrictSign m_sign;
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
  static bool IsUint8(const Node expression);
  static uint8_t Uint8(const Node expression);
  static EditionReference Addition(IntegerHandler a, IntegerHandler b);
};


}

#endif
