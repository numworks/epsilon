#ifndef POINCARE_EXPRESSION_SIGN_H
#define POINCARE_EXPRESSION_SIGN_H

#include <assert.h>
#include <omg/bit_helper.h>
#include <omg/troolean.h>
#include <stdint.h>

#if POINCARE_TREE_LOG
#include <iostream>
#endif

namespace Poincare::Internal {
class Tree;

/* Note: The expressions handled here are assumed to have been systematic
 * reduced beforehand. Otherwise, we would have to deal with unprojected
 * expressions, as well as powers of non-integers and other unreduced trees.
 * TODO: Some logic could be optimized using this constraint. */

class Sign {
 public:
  constexpr Sign(bool canBeNull, bool canBeStrictlyPositive,
                 bool canBeStrictlyNegative, bool canBeNonInteger = true)
      : m_canBeNull(canBeNull),
        m_canBeStrictlyPositive(canBeStrictlyPositive),
        m_canBeStrictlyNegative(canBeStrictlyNegative),
        m_canBeNonInteger(canBeNonInteger &&
                          (canBeStrictlyPositive || canBeStrictlyNegative)) {
    // By ensuring its members can't be modified, a Sign is always valid.
    assert(isValid());
  }
  constexpr Sign(uint8_t value)
      : Sign(OMG::BitHelper::getBitRange(value, 0, 0),
             OMG::BitHelper::getBitRange(value, 1, 1),
             OMG::BitHelper::getBitRange(value, 2, 2),
             OMG::BitHelper::getBitRange(value, 3, 3)) {}

  constexpr bool canBeNull() const { return m_canBeNull; }
  constexpr bool canBeStrictlyPositive() const {
    return m_canBeStrictlyPositive;
  }
  constexpr bool canBeStrictlyNegative() const {
    return m_canBeStrictlyNegative;
  }
  constexpr bool canBeNonInteger() const { return m_canBeNonInteger; }
  constexpr bool canBeNonNull() const {
    return m_canBeStrictlyPositive || m_canBeStrictlyNegative;
  }
  constexpr bool isNull() const { return !canBeNonNull(); }
  constexpr bool isInteger() const { return !canBeNonInteger(); }
  constexpr bool isStrictlyPositive() const {
    return !(m_canBeNull || m_canBeStrictlyNegative);
  }
  constexpr bool isStrictlyNegative() const {
    return !(m_canBeNull || m_canBeStrictlyPositive);
  }
  constexpr bool isNegative() const { return !m_canBeStrictlyPositive; }
  constexpr bool isPositive() const { return !m_canBeStrictlyNegative; }
  // It can be positive, negative and null
  constexpr bool isUnknown() const {
    return m_canBeNull && m_canBeStrictlyPositive && m_canBeStrictlyNegative;
  }
  // It's either strictly positive, strictly negative or null.
  constexpr bool isKnown() const {
    return isNull() || isStrictlyPositive() || isStrictlyNegative();
  }
  constexpr OMG::Troolean trooleanIsNull() const {
    return !canBeNull() ? OMG::Troolean::False
           : isNull()   ? OMG::Troolean::True
                        : OMG::Troolean::Unknown;
  }
  constexpr OMG::Troolean trooleanIsStrictlyPositive() const {
    return !canBeStrictlyPositive() ? OMG::Troolean::False
           : isStrictlyPositive()   ? OMG::Troolean::True
                                    : OMG::Troolean::Unknown;
  }
  constexpr OMG::Troolean trooleanIsStrictlyNegative() const {
    return !canBeStrictlyNegative() ? OMG::Troolean::False
           : isStrictlyNegative()   ? OMG::Troolean::True
                                    : OMG::Troolean::Unknown;
  }

  bool operator==(const Sign&) const = default;
  Sign operator||(const Sign& other) const {
    return Sign(m_canBeNull || other.canBeNull(),
                m_canBeStrictlyPositive || other.canBeStrictlyPositive(),
                m_canBeStrictlyNegative || other.canBeStrictlyNegative(),
                m_canBeNonInteger || other.canBeNonInteger());
  }
  Sign operator&&(const Sign& other) const {
    return Sign(m_canBeNull && other.canBeNull(),
                m_canBeStrictlyPositive && other.canBeStrictlyPositive(),
                m_canBeStrictlyNegative && other.canBeStrictlyNegative(),
                m_canBeNonInteger && other.canBeNonInteger());
  }

  constexpr uint8_t getValue() {
    // Cannot use bit_cast because it doesn't handle bitfields.
    return m_canBeNull << 0 | m_canBeStrictlyPositive << 1 |
           m_canBeStrictlyNegative << 2 | m_canBeNonInteger << 3;
  }

  constexpr static Sign Zero() { return Sign(true, false, false); }
  constexpr static Sign NonNull() { return Sign(false, true, true); }
  constexpr static Sign StrictlyPositive() { return Sign(false, true, false); }
  constexpr static Sign Positive() { return Sign(true, true, false); }
  constexpr static Sign StrictlyNegative() { return Sign(false, false, true); }
  constexpr static Sign Negative() { return Sign(true, false, true); }
  constexpr static Sign Unknown() { return Sign(true, true, true); }
  constexpr static Sign StrictlyPositiveInteger() {
    return Sign(false, true, false, false);
  }
  constexpr static Sign PositiveInteger() {
    return Sign(true, true, false, false);
  }
  constexpr static Sign StrictlyNegativeInteger() {
    return Sign(false, false, true, false);
  }
  constexpr static Sign NegativeInteger() {
    return Sign(true, false, true, false);
  }
  constexpr static Sign NonNullInteger() {
    return Sign(false, true, true, false);
  }
  constexpr static Sign Integer() { return Sign(true, true, true, false); }

  static Sign Get(const Tree* e);

#if POINCARE_TREE_LOG
  __attribute__((__used__)) void log(bool endOfLine = true) const {
    log(std::cout, endOfLine);
  }
  void log(std::ostream& stream, bool endOfLine = true) const;
#endif

 private:
  constexpr bool isValid() const {
    return m_canBeStrictlyPositive || m_canBeStrictlyNegative ||
           (m_canBeNull && !m_canBeNonInteger);
  }

  bool m_canBeNull : 1;
  bool m_canBeStrictlyPositive : 1;
  bool m_canBeStrictlyNegative : 1;
  bool m_canBeNonInteger : 1;
};

class ComplexSign {
 public:
  constexpr ComplexSign(Sign realSign, Sign imagSign)
      : m_realValue(realSign.getValue()), m_imagValue(imagSign.getValue()) {}
  constexpr ComplexSign(uint8_t value)
      : m_realValue(OMG::BitHelper::getBitRange(value, 3, 0)),
        m_imagValue(OMG::BitHelper::getBitRange(value, 7, 4)) {}

  constexpr uint8_t getValue() const { return m_realValue | m_imagValue << 4; }

  constexpr Sign realSign() const { return Sign(m_realValue); }
  constexpr Sign imagSign() const { return Sign(m_imagValue); }

  constexpr bool isReal() const { return imagSign().isNull(); }
  constexpr bool isPureIm() const { return realSign().isNull(); }
  constexpr bool isNonReal() const { return !imagSign().canBeNull(); }
  constexpr bool canBeNonReal() const { return imagSign().canBeNonNull(); }
  constexpr bool isNull() const { return isReal() && isPureIm(); }
  constexpr bool isPure() const { return isReal() || isPureIm(); }
  // Anything is possible
  constexpr bool isUnknown() const {
    return realSign().isUnknown() && imagSign().isUnknown();
  }
  constexpr bool canBeNull() const {
    return realSign().canBeNull() && imagSign().canBeNull();
  }
  constexpr bool canBeNonInteger() const {
    return realSign().canBeNonInteger() || imagSign().canBeNonInteger();
  }
  constexpr bool isInteger() const { return !canBeNonInteger(); }

  bool operator==(const ComplexSign&) const = default;
  ComplexSign operator&&(const ComplexSign& other) const {
    return ComplexSign(realSign() && other.realSign(),
                       imagSign() && other.imagSign());
  }

  static constexpr ComplexSign RealInteger() {
    return ComplexSign(Sign::Integer(), Sign::Zero());
  }
  static constexpr ComplexSign RealUnknown() {
    return ComplexSign(Sign::Unknown(), Sign::Zero());
  }
  static constexpr ComplexSign Unknown() {
    return ComplexSign(Sign::Unknown(), Sign::Unknown());
  }
  static constexpr ComplexSign Zero() {
    return ComplexSign(Sign::Zero(), Sign::Zero());
  }
  static constexpr ComplexSign RealStrictlyPositive() {
    return ComplexSign(Sign::StrictlyPositive(), Sign::Zero());
  }
  static constexpr ComplexSign RealStrictlyPositiveInteger() {
    return ComplexSign(Sign::StrictlyPositiveInteger(), Sign::Zero());
  }

  static ComplexSign Get(const Tree* e);

  /* Sign of e1 - e2 so that e1 < e2 <=> SignOfDifference(e1, e2) < 0 and so on.
   * Beware that the difference may be real while the trees were complexes. */
  static ComplexSign SignOfDifference(const Tree* e1, const Tree* e2);

#if POINCARE_TREE_LOG
  __attribute__((__used__)) void log(bool endOfLine = true) const {
    log(std::cout, endOfLine);
  }
  void log(std::ostream& stream, bool endOfLine = true) const;
#endif

 private:
  uint8_t m_realValue : 4;
  uint8_t m_imagValue : 4;
};

static_assert(sizeof(Sign) == sizeof(uint8_t));
static_assert(sizeof(ComplexSign) == sizeof(uint8_t));

// TODO : Sign could be used here instead.

enum class NonStrictSign : int8_t { Positive = 1, Negative = -1 };

enum class StrictSign : int8_t { Positive = 1, Null = 0, Negative = -1 };

inline StrictSign InvertSign(StrictSign sign) {
  return static_cast<StrictSign>(-static_cast<int8_t>(sign));
}

inline NonStrictSign InvertSign(NonStrictSign sign) {
  return static_cast<NonStrictSign>(-static_cast<int8_t>(sign));
}

}  // namespace Poincare::Internal

#endif
