#pragma once

#include <assert.h>
#include <omg/bit_helper.h>
#include <omg/troolean.h>
#include <stdint.h>

#if POINCARE_TREE_LOG
#include <iostream>
#endif

namespace Poincare {

/* Note: The expressions handled here are assumed to have been systematic
 * reduced beforehand. Otherwise, we would have to deal with unprojected
 * expressions, as well as powers of non-integers and other unreduced trees.
 * TODO: Some logic could be optimized using this constraint. */

class Sign {
  friend class Properties;

 public:
  constexpr Sign(bool canBeNull, bool canBeStrictlyPositive,
                 bool canBeStrictlyNegative)
      : m_canBeNull(canBeNull),
        m_canBeStrictlyPositive(canBeStrictlyPositive),
        m_canBeStrictlyNegative(canBeStrictlyNegative) {
    // By ensuring its members can't be modified, a Sign always remains valid.
    assert(isValid());
  }
  constexpr static Sign FromValue(uint8_t value) {
    return Sign(OMG::BitHelper::getBitRange(value, 0, 0),
                OMG::BitHelper::getBitRange(value, 1, 1),
                OMG::BitHelper::getBitRange(value, 2, 2));
  }

  constexpr bool canBeNull() const { return m_canBeNull; }
  constexpr bool canBeStrictlyPositive() const {
    return m_canBeStrictlyPositive;
  }
  constexpr bool canBeStrictlyNegative() const {
    return m_canBeStrictlyNegative;
  }
  constexpr bool canBeNonNull() const {
    return m_canBeStrictlyPositive || m_canBeStrictlyNegative;
  }
  constexpr bool isNull() const { return !canBeNonNull(); }
  constexpr bool isStrictlyPositive() const {
    return !(m_canBeNull || m_canBeStrictlyNegative);
  }
  constexpr bool isStrictlyNegative() const {
    return !(m_canBeNull || m_canBeStrictlyPositive);
  }
  constexpr bool isNegative() const { return !m_canBeStrictlyPositive; }
  constexpr bool isPositive() const { return !m_canBeStrictlyNegative; }

  // No information
  constexpr bool isUnknown() const {
    return m_canBeNull && m_canBeStrictlyPositive && m_canBeStrictlyNegative;
  }
  // Is either positive or negative.
  constexpr bool hasKnownSign() const { return isPositive() || isNegative(); }
  // Is either strictly positive, strictly negative or null.
  constexpr bool hasKnownStrictSign() const {
    return isNull() || isStrictlyPositive() || isStrictlyNegative();
  }
  constexpr OMG::Troolean trooleanIsNull() const {
    return !canBeNull() ? OMG::Troolean::False
           : isNull()   ? OMG::Troolean::True
                        : OMG::Troolean::Unknown;
  }
  constexpr OMG::Troolean trooleanIsPositive() const {
    return isPositive()           ? OMG::Troolean::True
           : isStrictlyNegative() ? OMG::Troolean::False
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
  /* OR operator on each of sign's properties:
   * PositiveInteger() || NegativeInteger() = Integer() */
  Sign operator||(const Sign& other) const {
    return Sign(m_canBeNull || other.canBeNull(),
                m_canBeStrictlyPositive || other.canBeStrictlyPositive(),
                m_canBeStrictlyNegative || other.canBeStrictlyNegative());
  }
  /* AND operator on each of sign's properties:
   * PositiveInteger() && NegativeInteger() = Zero() */
  Sign operator&&(const Sign& other) const {
    return Sign(m_canBeNull && other.canBeNull(),
                m_canBeStrictlyPositive && other.canBeStrictlyPositive(),
                m_canBeStrictlyNegative && other.canBeStrictlyNegative());
  }

  constexpr uint8_t getValue() {
    // Cannot use bit_cast because it doesn't handle bitfields.
    return m_canBeNull << 0 | m_canBeStrictlyPositive << 1 |
           m_canBeStrictlyNegative << 2;
  }

  constexpr static Sign Zero() { return Sign(true, false, false); }
  constexpr static Sign NonNull() { return Sign(false, true, true); }
  constexpr static Sign StrictlyPositive() { return Sign(false, true, false); }
  constexpr static Sign Positive() { return Sign(true, true, false); }
  constexpr static Sign StrictlyNegative() { return Sign(false, false, true); }
  constexpr static Sign Negative() { return Sign(true, false, true); }
  constexpr static Sign Unknown() { return Sign(true, true, true); }

#if POINCARE_TREE_LOG
  __attribute__((__used__)) void log() const { log(std::cout, true); }
  void log(std::ostream& stream, bool endOfLine = true) const;
#endif

 private:
  constexpr bool isValid() const {
    return m_canBeStrictlyPositive || m_canBeStrictlyNegative || m_canBeNull;
  }

  bool m_canBeNull : 1;
  bool m_canBeStrictlyPositive : 1;
  bool m_canBeStrictlyNegative : 1;
};

static_assert(sizeof(Sign) == sizeof(uint8_t));

}  // namespace Poincare
