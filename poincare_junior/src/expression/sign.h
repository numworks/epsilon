#ifndef POINCARE_EXPRESSION_SIGN_H
#define POINCARE_EXPRESSION_SIGN_H

#include <stdint.h>

namespace PoincareJ {
class Tree;

namespace Sign {

struct Sign {
  bool canBeNull : 1 = false;
  bool canBePositive : 1 = false;
  bool canBeNegative : 1 = false;
  bool isInteger : 1 = false;  // = !canBeNonIntegral

  // There must be a sign, and zero should must be an integer.
  bool isValid() const {
    return canBePositive || canBeNegative || (canBeNull && isInteger);
  }
  bool isZero() const;
  bool isStrictlyPositive() const;
  bool isStrictlyNegative() const;
  bool isNegative() const;
  bool isPositive() const;
  bool isKnown() const;

  bool operator==(const Sign&) const = default;
};

static_assert(sizeof(Sign) == sizeof(uint8_t));

constexpr Sign Zero{.canBeNull = true, .isInteger = true};
constexpr Sign Positive{.canBePositive = true};
constexpr Sign NonZeroNatural{.canBePositive = true, .isInteger = true};
constexpr Sign PositiveOrNull{.canBeNull = true, .canBePositive = true};
constexpr Sign Negative{.canBeNegative = true};
constexpr Sign NegativeOrNull{.canBeNull = true, .canBeNegative = true};
constexpr Sign Unknown{
    .canBeNull = true, .canBePositive = true, .canBeNegative = true};

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

struct ComplexSign {
  // Real Part
  Sign realSign;
  // Imaginary Part
  Sign imagSign;

  bool isValid() const { return realSign.isValid() && imagSign.isValid(); }
  bool isReal() const;
  bool isZero() const;
  // Anything is possible
  bool isUnknown() const;
  bool canBeNull() const;
  bool isInteger() const;

  bool operator==(const ComplexSign&) const = default;
};

static_assert(sizeof(ComplexSign) == sizeof(uint8_t));

constexpr ComplexSign RealInteger{.realSign = Integer, .imagSign = Zero};
constexpr ComplexSign RealUnknown{.realSign = Unknown, .imagSign = Zero};
constexpr ComplexSign ComplexUnknown{.realSign = Unknown, .imagSign = Unknown};
constexpr ComplexSign ComplexZero{.realSign = Zero, .imagSign = Zero};
constexpr ComplexSign ComplexOne{.realSign = Positive, .imagSign = Zero};

ComplexSign NoIntegers(ComplexSign s);
ComplexSign Add(ComplexSign s1, ComplexSign s2);
ComplexSign Mult(ComplexSign s1, ComplexSign s2);

ComplexSign GetComplexSign(const Tree* t);

Sign GetSign(const Tree* t) {
  assert(GetComplexSign(t).isReal());
  return GetComplexSign(t).realSign();
}

constexpr uint8_t GetValue(ComplexSign s) {
  return s.realSign.canBeNull << 0 | s.realSign.canBePositive << 1 |
         s.realSign.canBeNegative << 2 | s.realSign.isInteger << 3 |
         s.imagSign.canBeNull << 4 | s.imagSign.canBePositive << 5 |
         s.imagSign.canBeNegative << 6 | s.imagSign.isInteger << 7;
}

constexpr ComplexSign GetSign(uint8_t value) {
  return Sign{.realSign.canBeNull = ((value >> 0) & 1) == 1,
              .realSign.canBePositive = ((value >> 1) & 1) == 1,
              .realSign.canBeNegative = ((value >> 2) & 1) == 1,
              .realSign.isInteger = ((value >> 3) & 1) == 1,
              .imagSign.canBeNull = ((value >> 4) & 1) == 1,
              .imagSign.canBePositive = ((value >> 5) & 1) == 1,
              .imagSign.canBeNegative = ((value >> 6) & 1) == 1,
              .imagSign.isInteger = ((value >> 7) & 1) == 1};
}

static_assert(GetSign(GetValue(RealInteger)) == RealInteger);
static_assert(GetSign(GetValue(RealUnknown)) == RealUnknown);
static_assert(GetSign(GetValue(ComplexUnknown)) == ComplexUnknown);
static_assert(ComplexUnknown.isUnknown());
static_assert(RealUnknown.isReal());
static_assert(RealInteger.isReal() && RealInteger.isInteger());

}  // namespace Sign
}  // namespace PoincareJ

#endif
