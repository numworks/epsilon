#include "sign.h"

#include "dimension.h"
#include "number.h"

namespace PoincareJ {

namespace Sign {

Sign Mult(Sign s1, Sign s2) {
  return {.canBeNull = s1.canBeNull || s2.canBeNull,
          .canBePositive = (s1.canBePositive && s2.canBePositive) ||
                           (s1.canBeNegative && s2.canBeNegative),
          .canBeNegative = (s1.canBePositive && s2.canBeNegative) ||
                           (s1.canBeNegative && s2.canBePositive)};
}

Sign Add(Sign s1, Sign s2) {
  return {.canBeNull = (s1.canBeNull && s2.canBeNull) ||
                       (s1.canBePositive && s2.canBeNegative) ||
                       (s1.canBeNegative && s2.canBePositive),
          .canBePositive = s1.canBePositive || s2.canBePositive,
          .canBeNegative = s1.canBeNegative || s2.canBeNegative};
}

Sign GetSign(const Tree* t) {
  assert(Dimension::GetDimension(t).isScalar());
  if (t->block()->isNumber()) {
    StrictSign s = Number::StrictSign(t);
    return {.canBeNull = s == StrictSign::Null,
            .canBePositive = s == StrictSign::Positive,
            .canBeNegative = s == StrictSign::Negative};
  }
  switch (t->type()) {
    case BlockType::Multiplication: {
      Sign s = Positive;
      for (const Tree* c : t->children()) {
        s = Mult(s, GetSign(c));
        if (s == Unknown || s == Null) {
          break;
        }
      }
      return s;
    }
    case BlockType::Addition: {
      Sign s = Null;
      for (const Tree* c : t->children()) {
        s = Add(s, GetSign(c));
        if (s == Unknown) {
          break;
        }
      }
      return s;
    }
    case BlockType::Power: {
      Sign s = GetSign(t->firstChild());
      return {.canBeNull = s.canBeNull,
              .canBePositive = true,
              .canBeNegative = s.canBeNegative};
    }
    case BlockType::Abs:
    case BlockType::Norm: {
      Sign s = GetSign(t->firstChild());
      return {.canBeNull = s.canBeNull,
              .canBePositive = s.canBePositive || s.canBeNegative,
              .canBeNegative = false};
    }
    case BlockType::ArcSine:
    case BlockType::ArcTangent:
    case BlockType::ArcCosine:
      return PositiveOrNull;
    case BlockType::Exponential:
    case BlockType::Factorial:
      return Positive;
    default:
      return Unknown;
  }
}

}  // namespace Sign
}  // namespace PoincareJ
