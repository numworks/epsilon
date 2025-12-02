#pragma once

#include "sign.h"

#if POINCARE_TREE_LOG
#include <iostream>
#endif

namespace Poincare {

/* Note: The expressions handled here are assumed to have been systematic
 * reduced beforehand. Otherwise, we would have to deal with unprojected
 * expressions, as well as powers of non-integers and other unreduced trees.
 * TODO: Some logic could be optimized using this constraint. */

class Properties {
 public:
  constexpr Properties(Sign sign, bool canBeNonInteger,
                       bool canBeInfinite = true)
      : Properties(sign.canBeNull(), sign.canBeStrictlyPositive(),
                   sign.canBeStrictlyNegative(), canBeNonInteger,
                   canBeInfinite) {}

  constexpr explicit Properties(Sign sign) : Properties(sign, true, true) {}

  constexpr Properties(bool canBeNull, bool canBeStrictlyPositive,
                       bool canBeStrictlyNegative, bool canBeNonInteger,
                       bool canBeInfinite = true)
      : m_canBeNull(canBeNull),
        m_canBeStrictlyPositive(canBeStrictlyPositive),
        m_canBeStrictlyNegative(canBeStrictlyNegative),
        m_canBeNonInteger(canBeNonInteger &&
                          (canBeStrictlyPositive || canBeStrictlyNegative)),
        m_canBeInfinite(canBeInfinite &&
                        (canBeStrictlyPositive || canBeStrictlyNegative)) {
    /* By ensuring its members can't be modified, Properties always remain
     * valid. */
    assert(isValid());
  }

  /* sign() is often used to retrieve a property such as canBeNull(), so it is
   * worth to inline it to allow the member to be accessed directly. */
  [[gnu::always_inline]] constexpr Sign sign() const {
    return Sign(m_canBeNull, m_canBeStrictlyPositive, m_canBeStrictlyNegative);
  }

  constexpr static Properties FromValue(uint8_t value) {
    return Properties(Sign(OMG::BitHelper::getBitRange(value, 0, 0),
                           OMG::BitHelper::getBitRange(value, 1, 1),
                           OMG::BitHelper::getBitRange(value, 2, 2)),
                      OMG::BitHelper::getBitRange(value, 3, 3),
                      OMG::BitHelper::getBitRange(value, 4, 4));
  }

  constexpr bool canBeNonInteger() const { return m_canBeNonInteger; }
  constexpr bool canBeInfinite() const { return m_canBeInfinite; }
  constexpr bool isInteger() const { return !m_canBeNonInteger; }
  constexpr bool isFinite() const { return !m_canBeInfinite; }

  // No information
  constexpr bool isUnknown() const {
    return m_canBeNull && m_canBeStrictlyPositive && m_canBeStrictlyNegative &&
           m_canBeNonInteger && m_canBeInfinite;
  }

  bool operator==(const Properties&) const = default;

  /* OR operator on each property:
   * PositiveInteger() || NegativeInteger() = Integer() */
  Properties operator||(const Properties& other) const {
    return Properties(sign() || other.sign(),
                      m_canBeNonInteger || other.canBeNonInteger(),
                      m_canBeInfinite || other.canBeInfinite());
  }
  /* AND operator on each property:
   * PositiveInteger() && NegativeInteger() = Zero() */
  Properties operator&&(const Properties& other) const {
    return Properties(sign() && other.sign(),
                      m_canBeNonInteger && other.canBeNonInteger(),
                      m_canBeInfinite && other.canBeInfinite());
  }

  constexpr uint8_t getValue() {
    // Cannot use bit_cast because it doesn't handle bitfields.
    return m_canBeNull << 0 | m_canBeStrictlyPositive << 1 |
           m_canBeStrictlyNegative << 2 | m_canBeNonInteger << 3 |
           m_canBeInfinite << 4;
  }

  constexpr static Properties Unknown() {
    return Properties(Sign::Unknown(), true, true);
  }
  constexpr static Properties Zero() {
    return Properties(Sign::Zero(), false, false);
  }
  constexpr static Properties NonNull() {
    return Properties(Sign::NonNull(), true, true);
  }
  constexpr static Properties StrictlyPositive() {
    return Properties(Sign::StrictlyPositive(), true, true);
  }
  constexpr static Properties Positive() {
    return Properties(Sign::Positive(), true, true);
  }
  constexpr static Properties StrictlyNegative() {
    return Properties(Sign::StrictlyNegative(), true, true);
  }
  constexpr static Properties Negative() {
    return Properties(Sign::Negative(), true, true);
  }
  constexpr static Properties NonNullFinite() {
    return Properties(Sign::NonNull(), true, false);
  }
  constexpr static Properties StrictlyPositiveInteger() {
    return Properties(Sign::StrictlyPositive(), false);
  }
  constexpr static Properties PositiveInteger() {
    return Properties(Sign::Positive(), false);
  }
  constexpr static Properties StrictlyNegativeInteger() {
    return Properties(Sign::StrictlyNegative(), false);
  }
  constexpr static Properties NegativeInteger() {
    return Properties(Sign::Negative(), false);
  }
  constexpr static Properties NonNullInteger() {
    return Properties(Sign::NonNull(), false);
  }
  constexpr static Properties Integer() {
    return Properties(Sign::Unknown(), false);
  }
  constexpr static Properties Finite() {
    return Properties(Sign::Unknown(), true, false);
  }
  constexpr static Properties FinitePositive() {
    return Properties(Sign::Positive(), true, false);
  }
  constexpr static Properties FiniteNegative() {
    return Properties(Sign::Negative(), true, false);
  }
  constexpr static Properties FiniteStrictlyPositive() {
    return Properties(Sign::StrictlyPositive(), true, false);
  }
  constexpr static Properties FiniteStrictlyNegative() {
    return Properties(Sign::StrictlyNegative(), true, false);
  }
  constexpr static Properties FiniteInteger() {
    return Properties(Sign::Unknown(), false, false);
  }
  constexpr static Properties FinitePositiveInteger() {
    return Properties(Sign::Positive(), false, false);
  }
  constexpr static Properties FiniteNegativeInteger() {
    return Properties(Sign::Negative(), false, false);
  }
  constexpr static Properties FiniteStrictlyPositiveInteger() {
    return Properties(Sign::StrictlyPositive(), false, false);
  }
  constexpr static Properties FiniteStrictlyNegativeInteger() {
    return Properties(Sign::StrictlyNegative(), false, false);
  }

#if POINCARE_TREE_LOG
  __attribute__((__used__)) void log() const { log(std::cout, true); }
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
  bool m_canBeInfinite : 1;
};

class ComplexProperties {
 public:
  constexpr ComplexProperties(Properties realProperties,
                              Properties imagProperties)
      : m_realValue(realProperties.getValue()),
        m_imagValue(imagProperties.getValue()) {}
  constexpr static ComplexProperties FromValue(uint8_t realValue,
                                               uint8_t imagValue) {
    return ComplexProperties(Properties::FromValue(realValue),
                             Properties::FromValue(imagValue));
  }

  constexpr uint8_t getRealValue() const { return m_realValue; }
  constexpr uint8_t getImagValue() const { return m_imagValue; }

  constexpr Properties realProperties() const {
    return Properties::FromValue(m_realValue);
  }
  constexpr Properties imagProperties() const {
    return Properties::FromValue(m_imagValue);
  }

  constexpr Sign realSign() const { return realProperties().sign(); }
  constexpr Sign imagSign() const { return imagProperties().sign(); }

  constexpr bool isReal() const { return imagSign().isNull(); }
  constexpr bool isPureIm() const { return realSign().isNull(); }
  constexpr bool isNonReal() const { return !imagSign().canBeNull(); }
  constexpr bool canBeNonReal() const { return imagSign().canBeNonNull(); }
  constexpr bool isNull() const { return isReal() && isPureIm(); }
  constexpr bool isPure() const { return isReal() || isPureIm(); }
  // Anything is possible
  constexpr bool isUnknown() const {
    return realProperties().isUnknown() && imagProperties().isUnknown();
  }
  constexpr bool canBeNull() const {
    return realSign().canBeNull() && imagSign().canBeNull();
  }
  constexpr bool canBeNonInteger() const {
    return realProperties().canBeNonInteger() ||
           imagProperties().canBeNonInteger();
  }
  constexpr bool isInteger() const { return !canBeNonInteger(); }
  constexpr bool isFinite() const {
    return realProperties().isFinite() && imagProperties().isFinite();
  }
  constexpr bool canBeInfinite() const {
    return realProperties().canBeInfinite() || imagProperties().canBeInfinite();
  }

  bool operator==(const ComplexProperties&) const = default;
  /* OR operator on both real and imaginary signs. See Sign operators.
   * Real() || Zero() = Real() */
  ComplexProperties operator||(const ComplexProperties& other) const {
    return ComplexProperties(realProperties() || other.realProperties(),
                             imagProperties() || other.imagProperties());
  }
  /* AND operator on both real and imaginary signs. See Sign operators.
   * Real() && Zero() = Zero() */
  ComplexProperties operator&&(const ComplexProperties& other) const {
    return ComplexProperties(realProperties() && other.realProperties(),
                             imagProperties() && other.imagProperties());
  }

  constexpr static ComplexProperties RealInteger() {
    return ComplexProperties(Properties::Integer(), Properties::Zero());
  }
  constexpr static ComplexProperties Real() {
    return ComplexProperties(Properties(Sign::Unknown()), Properties::Zero());
  }
  constexpr static ComplexProperties RealFinite() {
    return ComplexProperties(Properties::Finite(),
                             Properties(Properties::Zero()));
  }
  constexpr static ComplexProperties Unknown() {
    return ComplexProperties(Properties(Sign::Unknown()),
                             Properties(Sign::Unknown()));
  }
  constexpr static ComplexProperties Finite() {
    return ComplexProperties(Properties::Finite(), Properties::Finite());
  }
  constexpr static ComplexProperties Zero() {
    return ComplexProperties(Properties::Zero(), Properties::Zero());
  }
  constexpr static ComplexProperties RealPositive() {
    return ComplexProperties(Properties::Positive(), Properties::Zero());
  }
  constexpr static ComplexProperties RealStrictlyPositive() {
    return ComplexProperties(Properties::StrictlyPositive(),
                             Properties::Zero());
  }
  constexpr static ComplexProperties RealFiniteStrictlyPositive() {
    return ComplexProperties(Properties::FiniteStrictlyPositive(),
                             Properties::Zero());
  }
  constexpr static ComplexProperties RealStrictlyPositiveInteger() {
    return ComplexProperties(Properties::StrictlyPositiveInteger(),
                             Properties::Zero());
  }
  constexpr static ComplexProperties RealFiniteStrictlyPositiveInteger() {
    return ComplexProperties(Properties::FiniteStrictlyPositiveInteger(),
                             Properties::Zero());
  }

#if POINCARE_TREE_LOG
  __attribute__((__used__)) void log() const { return log(std::cout, true); }
  void log(std::ostream& stream, bool endOfLine = true) const;
#endif

 private:
  uint8_t m_realValue;
  uint8_t m_imagValue;
};

static_assert(sizeof(Properties) == sizeof(uint8_t));
static_assert(sizeof(ComplexProperties) == 2 * sizeof(uint8_t));

}  // namespace Poincare
