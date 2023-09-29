#include "sign.h"

#include "dimension.h"
#include "number.h"

namespace PoincareJ {

namespace Sign {

bool Sign::isZero() const {
  assert(isValid());
  return !(canBePositive || canBeNegative);
}
bool Sign::isStrictlyPositive() const {
  assert(isValid());
  return !(canBeNull || canBeNegative);
}
bool Sign::isStrictlyNegative() const {
  assert(isValid());
  return !(canBeNull || canBePositive);
}
bool Sign::isNegative() const {
  assert(isValid());
  return !canBePositive;
}
bool Sign::isPositive() const {
  assert(isValid());
  return !canBeNegative;
}

Sign Mult(Sign s1, Sign s2) {
  return {
      .canBeNull = s1.canBeNull || s2.canBeNull,
      .canBePositive = (s1.canBePositive && s2.canBePositive) ||
                       (s1.canBeNegative && s2.canBeNegative),
      .canBeNegative = (s1.canBePositive && s2.canBeNegative) ||
                       (s1.canBeNegative && s2.canBePositive),
      .isInteger = s1.isInteger && s2.isInteger,
  };
}

Sign Add(Sign s1, Sign s2) {
  return {
      .canBeNull = (s1.canBeNull && s2.canBeNull) ||
                   (s1.canBePositive && s2.canBeNegative) ||
                   (s1.canBeNegative && s2.canBePositive),
      .canBePositive = s1.canBePositive || s2.canBePositive,
      .canBeNegative = s1.canBeNegative || s2.canBeNegative,
      .isInteger = s1.isInteger && s2.isInteger,
  };
}

Sign GetSign(const Tree* t) {
  assert(Dimension::GetDimension(t).isScalar());
  if (t->type().isNumber()) {
    return Number::Sign(t);
  }
  switch (t->type()) {
    case BlockType::Multiplication: {
      Sign s = NonZeroNatural;  // 1
      for (const Tree* c : t->children()) {
        s = Mult(s, GetSign(c));
        if (s == Unknown || s.isZero()) {
          break;
        }
      }
      return s;
    }
    case BlockType::Addition: {
      Sign s = Zero;
      for (const Tree* c : t->children()) {
        s = Add(s, GetSign(c));
        if (s == Unknown) {
          break;
        }
      }
      return s;
    }
    case BlockType::Power: {
      Sign base = GetSign(t->firstChild());
      Sign exp = GetSign(t->child(1));
      return {
          .canBeNull = base.canBeNull,
          .canBePositive = true,
          .canBeNegative = base.canBeNegative,
          .isInteger =
              (base.isInteger && exp.isInteger && !exp.canBeNegative) ||
              exp.isZero(),
      };
    }
    case BlockType::Abs:
    case BlockType::Norm: {
      Sign s = GetSign(t->firstChild());
      return {
          .canBeNull = s.canBeNull,
          .canBePositive = s.canBePositive || s.canBeNegative,
          .canBeNegative = false,
          .isInteger = s.isInteger,
      };
    }
    case BlockType::ArcSine:
    case BlockType::ArcTangent:
      return GetSign(t->firstChild());
    case BlockType::ArcCosine:
      return PositiveOrNull;
    case BlockType::Exponential:
      return Positive;
    case BlockType::Factorial:
      return NonZeroNatural;
    default:
      return Unknown;
  }
}

}  // namespace Sign
}  // namespace PoincareJ
