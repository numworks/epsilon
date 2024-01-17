#ifndef POINCARE_EXPRESSION_SIGN_H
#define POINCARE_EXPRESSION_SIGN_H

#include <assert.h>
#include <omgpj/bit.h>
#include <stdint.h>

namespace PoincareJ {
class Tree;

/* Note: The expressions handled here are assumed to have been systematic
 * reduced beforehand. Otherwise, we would have to deal with unprojected
 * expressions, as well as powers of non-integers and other unreduced trees.
 * TODO: Some logic could be optimized using this constraint. */

class Sign {
 public:
  constexpr Sign(bool canBeNull, bool canBePositive, bool canBeNegative,
                 bool isInteger = false)
      : m_canBeNull(canBeNull),
        m_canBePositive(canBePositive),
        m_canBeNegative(canBeNegative),
        m_isInteger(isInteger || !(canBePositive || canBeNegative)) {
    // By ensuring its members can't be modified, a Sign is always valid.
    assert(isValid());
  }
  constexpr Sign(uint8_t value)
      : Sign(Bit::getBitRange(value, 0, 0), Bit::getBitRange(value, 1, 1),
             Bit::getBitRange(value, 2, 2), Bit::getBitRange(value, 3, 3)) {}

  constexpr bool canBeNull() const { return m_canBeNull; }
  constexpr bool canBePositive() const { return m_canBePositive; }
  constexpr bool canBeNegative() const { return m_canBeNegative; }
  constexpr bool isInteger() const { return m_isInteger; }
  constexpr bool isZero() const {
    return !(m_canBePositive || m_canBeNegative);
  }
  constexpr bool isStrictlyPositive() const {
    return !(m_canBeNull || m_canBeNegative);
  }
  constexpr bool isStrictlyNegative() const {
    return !(m_canBeNull || m_canBePositive);
  }
  constexpr bool isNegative() const { return !m_canBePositive; }
  constexpr bool isPositive() const { return !m_canBeNegative; }
  // It can be positive, negative and null
  constexpr bool isUnknown() const {
    return m_canBeNull && m_canBePositive && m_canBeNegative;
  }
  // It's either strictly positive, strictly negative or null.
  constexpr bool isKnown() const {
    return isZero() || isStrictlyPositive() || isStrictlyNegative();
  }

  bool operator==(const Sign&) const = default;

  constexpr uint8_t getValue() {
    // Cannot use bit_cast because it doesn't handle bitfields.
    return m_canBeNull << 0 | m_canBePositive << 1 | m_canBeNegative << 2 |
           m_isInteger << 3;
  }

  constexpr static Sign Zero() { return Sign(true, false, false); }
  constexpr static Sign Positive() { return Sign(false, true, false); }
  constexpr static Sign PositiveOrNull() { return Sign(true, true, false); }
  constexpr static Sign Negative() { return Sign(false, false, true); }
  constexpr static Sign NegativeOrNull() { return Sign(true, false, true); }
  constexpr static Sign Unknown() { return Sign(true, true, true); }
  constexpr static Sign PositiveInteger() {
    return Sign(false, true, false, true);
  }
  constexpr static Sign NegativeInteger() {
    return Sign(false, false, true, true);
  }
  constexpr static Sign Integer() { return Sign(true, true, true, true); }

  static Sign Get(const Tree* t);

 private:
  constexpr bool isValid() const {
    return m_canBePositive || m_canBeNegative || (m_canBeNull && m_isInteger);
  }

  bool m_canBeNull : 1 = false;
  bool m_canBePositive : 1 = false;
  bool m_canBeNegative : 1 = false;
  bool m_isInteger : 1 = false;  // = !canBeNonIntegral
};

struct ComplexSign {
 public:
  constexpr ComplexSign(Sign realSign, Sign imagSign)
      : m_realValue(realSign.getValue()), m_imagValue(imagSign.getValue()) {}
  constexpr ComplexSign(uint8_t value)
      : m_realValue(Bit::getBitRange(value, 3, 0)),
        m_imagValue(Bit::getBitRange(value, 7, 4)) {}

  constexpr uint8_t getValue() const {
    return Bit::getBitRange(m_realValue, 3, 0) | m_imagValue << 4;
  }

  constexpr Sign realSign() const { return Sign(m_realValue); }
  constexpr Sign imagSign() const { return Sign(m_imagValue); }

  constexpr bool isReal() const { return imagSign().isZero(); }
  constexpr bool isZero() const {
    return realSign().isZero() && imagSign().isZero();
  }
  // Anything is possible
  constexpr bool isUnknown() const {
    return realSign().isUnknown() && imagSign().isUnknown();
  }
  constexpr bool canBeNull() const {
    return realSign().canBeNull() && imagSign().canBeNull();
  }
  constexpr bool isInteger() const {
    return realSign().isInteger() && imagSign().isInteger();
  }

  bool operator==(const ComplexSign&) const = default;

  static constexpr ComplexSign RealInteger() {
    return ComplexSign(Sign::Integer(), Sign::Zero());
  }
  static constexpr ComplexSign RealUnknown() {
    return ComplexSign(Sign::Unknown(), Sign::Zero());
  }
  static constexpr ComplexSign ComplexUnknown() {
    return ComplexSign(Sign::Unknown(), Sign::Unknown());
  }
  static constexpr ComplexSign ComplexZero() {
    return ComplexSign(Sign::Zero(), Sign::Zero());
  }
  static constexpr ComplexSign ComplexOne() {
    return ComplexSign(Sign::PositiveInteger(), Sign::Zero());
  }

  static ComplexSign Get(const Tree* t);

 private:
  uint8_t m_realValue : 4;
  uint8_t m_imagValue : 4;
};

static_assert(sizeof(Sign) == sizeof(uint8_t));
static_assert(sizeof(ComplexSign) == sizeof(uint8_t));

static_assert(ComplexSign(ComplexSign::RealInteger().getValue()) ==
              ComplexSign::RealInteger());
static_assert(ComplexSign(ComplexSign::RealInteger().getValue()) ==
              ComplexSign::RealInteger());
static_assert(ComplexSign(ComplexSign::RealUnknown().getValue()) ==
              ComplexSign::RealUnknown());
static_assert(ComplexSign(ComplexSign::ComplexUnknown().getValue()) ==
              ComplexSign::ComplexUnknown());
static_assert(ComplexSign::ComplexUnknown().isUnknown());
static_assert(ComplexSign::RealUnknown().isReal());
static_assert(ComplexSign::RealInteger().isReal() &&
              ComplexSign::RealInteger().isInteger());

}  // namespace PoincareJ

#endif
