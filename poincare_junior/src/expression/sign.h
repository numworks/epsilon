#ifndef POINCARE_EXPRESSION_SIGN_H
#define POINCARE_EXPRESSION_SIGN_H

#include <assert.h>
#include <omgpj/bit.h>
#include <stdint.h>

namespace PoincareJ {
class Tree;

namespace Sign {

/* Note: The expressions handled here are assumed to have been systematic
 * reduced beforehand. Otherwise, we would have to deal with unprojected
 * expressions, as well as powers of non-integers and other unreduced trees.
 * TODO: Some logic could be optimized using this constraint. */

struct Sign {
  bool canBeNull : 1 = false;
  bool canBePositive : 1 = false;
  bool canBeNegative : 1 = false;
  bool isInteger : 1 = false;  // = !canBeNonIntegral

  constexpr Sign(bool canBeNull, bool canBePositive, bool canBeNegative,
                 bool isInteger = false)
      : canBeNull(canBeNull),
        canBePositive(canBePositive),
        canBeNegative(canBeNegative),
        isInteger(isInteger || !(canBePositive || canBeNegative)) {
    assert(isValid());
  }
  constexpr bool isValid() const {
    return canBePositive || canBeNegative || (canBeNull && isInteger);
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

  bool operator==(const Sign&) const = default;
};

static_assert(sizeof(Sign) == sizeof(uint8_t));

constexpr Sign Zero(true, false, false);
constexpr Sign Positive(false, true, false);
constexpr Sign PositiveOrNull(true, true, false);
constexpr Sign Negative(false, false, true);
constexpr Sign NegativeOrNull(true, false, true);
constexpr Sign Unknown(true, true, true);
constexpr Sign PositiveInteger(false, true, false, true);
constexpr Sign NegativeInteger(false, false, true, true);
constexpr Sign Integer(true, true, true, true);

Sign NoIntegers(Sign s);
Sign Oppose(Sign s);
Sign Add(Sign s1, Sign s2);
Sign Mult(Sign s1, Sign s2);

constexpr uint8_t GetValue(Sign s) {
  // Cannot use bit_cast because it doesn't handle bitfields.
  return s.canBeNull << 0 | s.canBePositive << 1 | s.canBeNegative << 2 |
         s.isInteger << 3;
}

constexpr Sign GetSign(uint8_t value) {
  return Sign(Bit::getBitRange(value, 0, 0), Bit::getBitRange(value, 1, 1),
              Bit::getBitRange(value, 2, 2), Bit::getBitRange(value, 3, 3));
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
  bool isInteger() const;

  bool operator==(const ComplexSign&) const = default;
};

static_assert(sizeof(ComplexSign) == sizeof(uint8_t));

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
  return Bit::getBitRange(s.realValue, 3, 0) | s.imagValue << 4;
}

constexpr ComplexSign GetComplexSign(uint8_t value) {
  return ComplexSign(GetSign(Bit::getBitRange(value, 3, 0)),
                     GetSign(Bit::getBitRange(value, 7, 4)));
}

static_assert(GetComplexSign(GetValue(RealInteger)) == RealInteger);
static_assert(GetComplexSign(GetValue(RealInteger)) == RealInteger);
static_assert(GetComplexSign(GetValue(RealUnknown)) == RealUnknown);
static_assert(GetComplexSign(GetValue(ComplexUnknown)) == ComplexUnknown);
// static_assert(ComplexUnknown.isUnknown());
// static_assert(RealUnknown.isReal());
// static_assert(RealInteger.isReal() && RealInteger.isInteger());

}  // namespace Sign
}  // namespace PoincareJ

#endif
