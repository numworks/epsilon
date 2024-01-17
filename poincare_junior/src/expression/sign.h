#ifndef POINCARE_EXPRESSION_SIGN_H
#define POINCARE_EXPRESSION_SIGN_H

#include <assert.h>
#include <stdint.h>

namespace PoincareJ {
class Tree;

namespace Sign {

/* Note: The expressions handled here are assumed to have been systematic
 * reduced beforehand. Otherwise, we would have to deal with unprojected
 * expressions, as well as powers of non-integers.
 * TODO: Some logic could be optimized using this constraint. */

struct Sign {
  bool canBeNull : 1 = false;
  bool canBePositive : 1 = false;
  bool canBeNegative : 1 = false;
  bool isInteger : 1 = false;  // = !canBeNonIntegral, not always true when zero

  constexpr bool isValid() const {
    return canBePositive || canBeNegative || canBeNull;
  }
  bool isZero() const;
  bool isStrictlyPositive() const;
  bool isStrictlyNegative() const;
  bool isNegative() const;
  bool isPositive() const;
  // It can be positive, negative and null
  bool isUnknown() const;
  // It's either strictly positive, strictly negative or null.
  bool isKnown() const;
  bool isIntegerOrNull() const;
  // Return true if sign is equal or more restrictive than other sign.
  constexpr bool isSameOrMoreRestrictiveThan(Sign other) {
    assert(isValid() && other.isValid());
    return (other.canBeNegative || !canBeNegative) &&
           (other.canBeNull || !canBeNull) &&
           (other.canBePositive || !canBePositive) &&
           (isInteger || !other.isInteger);
  }
};

static_assert(sizeof(Sign) == sizeof(uint8_t));

/* Warning : These representations are not unique because of optional isInteger.
 * We could either ensure isInteger is always true when isZero, or allow both
 * representations to exist and limit == and value comparisons to strict
 * minimum. */
constexpr Sign Zero{.canBeNull = true, .isInteger = true};
constexpr Sign Positive{.canBePositive = true};
constexpr Sign PositiveOrNull{.canBeNull = true, .canBePositive = true};
constexpr Sign Negative{.canBeNegative = true};
constexpr Sign NegativeOrNull{.canBeNull = true, .canBeNegative = true};
constexpr Sign Unknown{
    .canBeNull = true, .canBePositive = true, .canBeNegative = true};
// These representation are unique
constexpr Sign NonZeroNatural{.canBePositive = true, .isInteger = true};
constexpr Sign PositiveInteger{.canBePositive = true, .isInteger = true};
constexpr Sign NegativeInteger{.canBeNegative = true, .isInteger = true};
constexpr Sign Integer{.canBeNull = true,
                       .canBePositive = true,
                       .canBeNegative = true,
                       .isInteger = true};

// Set isInteger to false unless sign isZero
Sign NoIntegers(Sign s);
Sign Oppose(Sign s);
Sign Add(Sign s1, Sign s2);
Sign Mult(Sign s1, Sign s2);

constexpr uint8_t GetValue(Sign s) {
  return s.canBeNull << 0 | s.canBePositive << 1 | s.canBeNegative << 2 |
         s.isInteger << 3;
}

constexpr Sign GetSign(uint8_t value) {
  return Sign{.canBeNull = ((value >> 0) & 1) == 1,
              .canBePositive = ((value >> 1) & 1) == 1,
              .canBeNegative = ((value >> 2) & 1) == 1,
              .isInteger = ((value >> 3) & 1) == 1};
}

struct ComplexSign {
  uint8_t realValue : 4;
  uint8_t imagValue : 4;

  constexpr ComplexSign(Sign realSign, Sign imagSign)
      : realValue(GetValue(realSign)), imagValue(GetValue(imagSign)) {
    assert(isValid());
  }
  constexpr Sign realSign() const { return GetSign(realValue); }
  constexpr Sign imagSign() const { return GetSign(imagValue); }

  constexpr bool isValid() const {
    return realSign().isValid() && imagSign().isValid();
  }
  bool isReal() const;
  bool isZero() const;
  // Anything is possible
  bool isUnknown() const;
  bool canBeNull() const;
  // Also return true if zero
  bool isInteger() const;
};

static_assert(sizeof(ComplexSign) == sizeof(uint8_t));

// Warning : These representations are not unique
constexpr ComplexSign RealInteger(Integer, Zero);
constexpr ComplexSign RealUnknown(Unknown, Zero);
constexpr ComplexSign ComplexUnknown(Unknown, Unknown);
constexpr ComplexSign ComplexZero(Zero, Zero);
constexpr ComplexSign ComplexOne(PositiveInteger, Zero);

ComplexSign NoIntegers(ComplexSign s);
ComplexSign Add(ComplexSign s1, ComplexSign s2);
ComplexSign Mult(ComplexSign s1, ComplexSign s2);

ComplexSign GetComplexSign(const Tree* t);

Sign GetSign(const Tree* t);

constexpr uint8_t GetValue(ComplexSign s) {
  return (s.realValue & 15) | s.imagValue << 4;
}

constexpr ComplexSign GetComplexSign(uint8_t value) {
  return ComplexSign(GetSign(value), GetSign(value >> 4));
}

// static_assert(GetComplexSign(GetValue(RealInteger)) == RealInteger);
// static_assert(GetComplexSign(GetValue(RealInteger)) == RealInteger);
// static_assert(GetComplexSign(GetValue(RealUnknown)) == RealUnknown);
// static_assert(GetComplexSign(GetValue(ComplexUnknown)) == ComplexUnknown);
// static_assert(ComplexUnknown.isUnknown());
// static_assert(RealUnknown.isReal());
// static_assert(RealInteger.isReal() && RealInteger.isInteger());

}  // namespace Sign
}  // namespace PoincareJ

#endif
