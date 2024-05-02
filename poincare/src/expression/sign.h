#ifndef POINCARE_EXPRESSION_SIGN_H
#define POINCARE_EXPRESSION_SIGN_H

#include <assert.h>
#include <omg/bit_helper.h>
#include <stdint.h>

namespace Poincare::Internal {
class Tree;

/* Note: The expressions handled here are assumed to have been systematic
 * reduced beforehand. Otherwise, we would have to deal with unprojected
 * expressions, as well as powers of non-integers and other unreduced trees.
 * TODO: Some logic could be optimized using this constraint. */

class Sign {
 public:
  constexpr Sign(bool canBeNull, bool canBeStriclyPositive,
                 bool canBeStriclyNegative, bool canBeNonInteger = true)
      : m_canBeNull(canBeNull),
        m_canBeStriclyPositive(canBeStriclyPositive),
        m_canBeStriclyNegative(canBeStriclyNegative),
        m_canBeNonInteger(canBeNonInteger &&
                          (canBeStriclyPositive || canBeStriclyNegative)) {
    // By ensuring its members can't be modified, a Sign is always valid.
    assert(isValid());
  }
  constexpr Sign(uint8_t value)
      : Sign(OMG::BitHelper::getBitRange(value, 0, 0),
             OMG::BitHelper::getBitRange(value, 1, 1),
             OMG::BitHelper::getBitRange(value, 2, 2),
             OMG::BitHelper::getBitRange(value, 3, 3)) {}

  constexpr bool canBeNull() const { return m_canBeNull; }
  constexpr bool canBeStriclyPositive() const { return m_canBeStriclyPositive; }
  constexpr bool canBeStriclyNegative() const { return m_canBeStriclyNegative; }
  constexpr bool canBeNonInteger() const { return m_canBeNonInteger; }
  constexpr bool canBeNonNull() const {
    return m_canBeStriclyPositive || m_canBeStriclyNegative;
  }
  constexpr bool isZero() const { return !canBeNonNull(); }
  constexpr bool isStrictlyPositive() const {
    return !(m_canBeNull || m_canBeStriclyNegative);
  }
  constexpr bool isStrictlyNegative() const {
    return !(m_canBeNull || m_canBeStriclyPositive);
  }
  constexpr bool isNegative() const { return !m_canBeStriclyPositive; }
  constexpr bool isPositive() const { return !m_canBeStriclyNegative; }
  // It can be positive, negative and null
  constexpr bool isUnknown() const {
    return m_canBeNull && m_canBeStriclyPositive && m_canBeStriclyNegative;
  }
  // It's either strictly positive, strictly negative or null.
  constexpr bool isKnown() const {
    return isZero() || isStrictlyPositive() || isStrictlyNegative();
  }

  bool operator==(const Sign&) const = default;
  Sign operator||(const Sign& other) const {
    return Sign(m_canBeNull || other.canBeNull(),
                m_canBeStriclyPositive || other.canBeStriclyPositive(),
                m_canBeStriclyNegative || other.canBeStriclyNegative(),
                m_canBeNonInteger || other.canBeNonInteger());
  }

  constexpr uint8_t getValue() {
    // Cannot use bit_cast because it doesn't handle bitfields.
    return m_canBeNull << 0 | m_canBeStriclyPositive << 1 |
           m_canBeStriclyNegative << 2 | m_canBeNonInteger << 3;
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

  static Sign Get(const Tree* t);

#if POINCARE_TREE_LOG
  void log(bool endOfLine = true) const;
#endif

 private:
  constexpr bool isValid() const {
    return m_canBeStriclyPositive || m_canBeStriclyNegative ||
           (m_canBeNull && !m_canBeNonInteger);
  }

  bool m_canBeNull : 1;
  bool m_canBeStriclyPositive : 1;
  bool m_canBeStriclyNegative : 1;
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

  constexpr bool isReal() const { return imagSign().isZero(); }
  constexpr bool isPureIm() const { return realSign().isZero(); }
  constexpr bool isComplex() const { return !imagSign().canBeNull(); }
  constexpr bool isZero() const { return isReal() && isPureIm(); }
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

  bool operator==(const ComplexSign&) const = default;

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

  static ComplexSign Get(const Tree* t);

  /* Sign of a - b so that a < b <=> SignOfDifference(a, b) < 0 and so on.
   * Beware that the difference may be real while the trees were complexes. */
  static ComplexSign SignOfDifference(const Tree* a, const Tree* b);

#if POINCARE_TREE_LOG
  void log() const;
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
