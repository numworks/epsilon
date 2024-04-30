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
  constexpr Sign(bool canBeNull, bool canBePositive, bool canBeNegative,
                 bool canBeNonInteger = true)
      : m_canBeNull(canBeNull),
        m_canBePositive(canBePositive),
        m_canBeNegative(canBeNegative),
        m_canBeNonInteger(canBeNonInteger && (canBePositive || canBeNegative)) {
    // By ensuring its members can't be modified, a Sign is always valid.
    assert(isValid());
  }
  constexpr Sign(uint8_t value)
      : Sign(OMG::BitHelper::getBitRange(value, 0, 0),
             OMG::BitHelper::getBitRange(value, 1, 1),
             OMG::BitHelper::getBitRange(value, 2, 2),
             OMG::BitHelper::getBitRange(value, 3, 3)) {}

  constexpr bool canBeNull() const { return m_canBeNull; }
  constexpr bool canBePositive() const { return m_canBePositive; }
  constexpr bool canBeNegative() const { return m_canBeNegative; }
  constexpr bool canBeNonInteger() const { return m_canBeNonInteger; }
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
           m_canBeNonInteger << 3;
  }

  constexpr static Sign Zero() { return Sign(true, false, false); }
  constexpr static Sign NonNull() { return Sign(false, true, true); }
  constexpr static Sign Positive() { return Sign(false, true, false); }
  constexpr static Sign PositiveOrNull() { return Sign(true, true, false); }
  constexpr static Sign Negative() { return Sign(false, false, true); }
  constexpr static Sign NegativeOrNull() { return Sign(true, false, true); }
  constexpr static Sign Unknown() { return Sign(true, true, true); }
  constexpr static Sign PositiveInteger() {
    return Sign(false, true, false, false);
  }
  constexpr static Sign PositiveOrNullInteger() {
    return Sign(true, true, false, false);
  }
  constexpr static Sign NegativeInteger() {
    return Sign(false, false, true, false);
  }
  constexpr static Sign NegativeOrNullInteger() {
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
    return m_canBePositive || m_canBeNegative ||
           (m_canBeNull && !m_canBeNonInteger);
  }

  bool m_canBeNull : 1;
  bool m_canBePositive : 1;
  bool m_canBeNegative : 1;
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
  constexpr bool isComplex() const { return !imagSign().canBeNull(); }
  constexpr bool isZero() const {
    return realSign().isZero() && imagSign().isZero();
  }
  // Is either zero, real or imaginary pure.
  constexpr bool isPure() const {
    return imagSign().isZero() || realSign().isZero();
  }
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
  static constexpr ComplexSign RealPositiveInteger() {
    return ComplexSign(Sign::PositiveInteger(), Sign::Zero());
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
