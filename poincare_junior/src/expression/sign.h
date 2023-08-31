#ifndef POINCARE_EXPRESSION_SIGN_H
#define POINCARE_EXPRESSION_SIGN_H


namespace PoincareJ {
class Tree;

namespace Sign {

struct Sign {
  bool canBeNull : 1 = false;
  bool canBePositive : 1 = false;
  bool canBeNegative : 1 = false;
  bool isInteger : 1 = false;  // = !canBeNonIntegral

  bool isZero() const { return !(canBePositive || canBeNegative); }
  bool isStrictlyPositive() const { return !(canBeNull || canBeNegative); }
  bool isStrictlyNegative() const { return !(canBeNull || canBePositive); }

  bool operator==(const Sign&) const = default;
};

constexpr Sign Zero{.canBeNull = true, .isInteger = true};
constexpr Sign Positive{.canBePositive = true};
constexpr Sign NonZeroNatural{.canBePositive = true, .isInteger = true};
constexpr Sign PositiveOrNull{.canBeNull = true, .canBePositive = true};
constexpr Sign Negative{.canBeNegative = true};
constexpr Sign NegativeOrNull{.canBeNull = true, .canBeNegative = true};
constexpr Sign Unknown{
    .canBeNull = true, .canBePositive = true, .canBeNegative = true};

Sign Add(Sign s1, Sign s2);
Sign Mult(Sign s1, Sign s2);

Sign GetSign(const Tree* t);

}  // namespace Sign
}  // namespace PoincareJ

#endif
