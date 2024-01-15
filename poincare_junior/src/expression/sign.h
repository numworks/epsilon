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

  bool isValid() const { return canBeNull || canBePositive || canBeNegative; }
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

Sign Add(Sign s1, Sign s2);
Sign Mult(Sign s1, Sign s2);

Sign GetSign(const Tree* t);

constexpr uint8_t GetValue(Sign sign) {
  return sign.canBeNull << 0 | sign.canBePositive << 1 |
         sign.canBeNegative << 2 | sign.isInteger << 3;
}

constexpr Sign GetSign(uint8_t value) {
  return Sign{.canBeNull = ((value >> 0) & 1) == 1,
              .canBePositive = ((value >> 1) & 1) == 1,
              .canBeNegative = ((value >> 2) & 1) == 1,
              .isInteger = ((value >> 3) & 1) == 1};
}

static_assert(GetSign(GetValue(Integer)) == Integer);
static_assert(GetSign(GetValue(Positive)) == Positive);
static_assert(GetSign(GetValue(NegativeOrNull)) == NegativeOrNull);

}  // namespace Sign
}  // namespace PoincareJ

#endif
