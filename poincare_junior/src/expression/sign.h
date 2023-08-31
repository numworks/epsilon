#ifndef POINCARE_EXPRESSION_SIGN_H
#define POINCARE_EXPRESSION_SIGN_H

#include <omgpj/enums.h>
#include <poincare_junior/src/memory/tree.h>

namespace PoincareJ {

namespace Sign {

struct Sign {
  bool canBeNull : 1 = false;
  bool canBePositive : 1 = false;
  bool canBeNegative : 1 = false;

  bool operator==(const Sign&) const = default;
};

constexpr Sign Null{.canBeNull = true};
constexpr Sign Positive{.canBePositive = true};
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
