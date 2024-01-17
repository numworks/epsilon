#include "sign.h"

#include "dimension.h"
#include "number.h"
#include "variables.h"

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
bool Sign::isUnknown() const {
  assert(isValid());
  return canBeNull && canBePositive && canBeNegative;
}
bool Sign::isKnown() const {
  assert(isValid());
  return isZero() || isStrictlyPositive() || isStrictlyNegative();
}
bool Sign::isIntegerOrNull() const {
  assert(isValid());
  return isInteger || isZero();
}

Sign NoIntegers(Sign s) {
  return {
      .canBeNull = s.canBeNull,
      .canBePositive = s.canBeNegative,
      .canBeNegative = s.canBePositive,
      .isInteger = false,
  };
}

Sign Oppose(Sign s) {
  return {
      .canBeNull = s.canBeNull,
      .canBePositive = s.canBeNegative,
      .canBeNegative = s.canBePositive,
      .isInteger = s.isInteger,
  };
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
  assert(GetComplexSign(t).isReal());
  return GetComplexSign(t).realSign();
}

bool ComplexSign::isReal() const {
  assert(isValid());
  return imagSign().isZero();
}
bool ComplexSign::isZero() const {
  assert(isValid());
  return realSign().isZero() && imagSign().isZero();
}
bool ComplexSign::isUnknown() const {
  assert(isValid());
  return realSign().isUnknown() && imagSign().isUnknown();
}
bool ComplexSign::canBeNull() const {
  assert(isValid());
  return realSign().canBeNull && imagSign().canBeNull;
}
bool ComplexSign::isInteger() const {
  assert(isValid());
  return realSign().isIntegerOrNull() && imagSign().isIntegerOrNull();
}

ComplexSign NoIntegers(ComplexSign s) {
  return ComplexSign(NoIntegers(s.realSign()), NoIntegers(s.imagSign()));
}

ComplexSign Mult(ComplexSign s1, ComplexSign s2) {
  Sign a = Add(Mult(s1.realSign(), s2.realSign()),
               Oppose(Mult(s1.imagSign(), s2.imagSign())));
  Sign b = Add(Mult(s1.realSign(), s2.imagSign()),
               Mult(s1.imagSign(), s2.realSign()));
  return ComplexSign(a, b);
}

ComplexSign Add(ComplexSign s1, ComplexSign s2) {
  return ComplexSign(Add(s1.realSign(), s2.realSign()),
                     Add(s1.imagSign(), s2.imagSign()));
}

// Note: A complex function plotter can be used to fill in these methods.
ComplexSign GetComplexSign(const Tree* t) {
  assert(Dimension::GetDimension(t).isScalar());
  if (t->isNumber()) {
    return ComplexSign(Number::Sign(t), Zero);
  }
  switch (t->type()) {
    case BlockType::Multiplication: {
      ComplexSign s = ComplexOne;
      for (const Tree* c : t->children()) {
        s = Mult(s, GetComplexSign(c));
        if (s.isUnknown() || s.isZero()) {
          break;
        }
      }
      return s;
    }
    case BlockType::Addition: {
      ComplexSign s = ComplexZero;
      for (const Tree* c : t->children()) {
        s = Add(s, GetComplexSign(c));
        if (s.isUnknown()) {
          break;
        }
      }
      return s;
    }
    case BlockType::Power: {
      ComplexSign base = GetComplexSign(t->firstChild());
      ComplexSign exp = GetComplexSign(t->child(1));
      // If this assert can;t be maintained, escape with Unknown.
      assert(exp.isReal() && exp.isInteger());
      if (base.isZero()) {
        return ComplexZero;
      }
      if (exp.isZero()) {
        return ComplexOne;
      }
      bool isInteger = (base.isInteger() && !exp.realSign().isPositive());
      bool baseIsReal = base.isReal();
      return ComplexSign(
          {.canBeNull = base.realSign().canBeNull,
           .canBePositive = true,
           .canBeNegative = !(baseIsReal && base.realSign().isPositive()),
           .isInteger = isInteger},
          {.canBeNull = base.imagSign().canBeNull,
           .canBePositive = !baseIsReal,
           .canBeNegative = !baseIsReal,
           .isInteger = isInteger});
    }
    case BlockType::Norm:
      // Child isn't a scalar
      return ComplexSign(PositiveOrNull, Zero);
    case BlockType::Abs: {
      ComplexSign s = GetComplexSign(t->firstChild());
      return ComplexSign(
          {.canBeNull = s.canBeNull(),
           .canBePositive = !s.isZero(),
           .canBeNegative = false,
           .isInteger = s.isInteger() && (s.isReal() || s.realSign().isZero())},
          Zero);
    }
    case BlockType::ArcSine:
    case BlockType::ArcTangent:
      // Both real and imaginary part keep the same sign
      return NoIntegers(GetComplexSign(t->firstChild()));
    case BlockType::ArcCosine: {
      ComplexSign s = GetComplexSign(t->firstChild());
      return ComplexSign({.canBeNull = s.realSign().canBePositive,
                          .canBePositive = true,
                          .canBeNegative = false,
                          .isInteger = false},
                         {.canBeNull = s.imagSign().canBeNull,
                          .canBePositive = s.imagSign().canBeNegative ||
                                           (s.imagSign().canBeNull &&
                                            s.realSign().canBePositive),
                          .canBeNegative = s.imagSign().canBePositive ||
                                           (s.imagSign().canBeNull &&
                                            s.realSign().canBeNegative),
                          .isInteger = false});
    }
    case BlockType::Exponential: {
      ComplexSign s = GetComplexSign(t->firstChild());
      bool childIsReal = s.isReal();
      return ComplexSign({.canBeNull = !childIsReal,
                          .canBePositive = childIsReal,
                          .canBeNegative = !childIsReal,
                          .isInteger = false},
                         {.canBeNull = true,
                          .canBePositive = !childIsReal,
                          .canBeNegative = !childIsReal,
                          .isInteger = false});
    }
    case BlockType::Ln: {
      ComplexSign s = GetComplexSign(t->firstChild());
      bool lnIsReal = s.isReal() && s.realSign().isPositive();
      return ComplexSign(
          Unknown, {.canBeNull = lnIsReal,
                    .canBePositive = s.imagSign().isPositive() && !lnIsReal,
                    .canBeNegative = s.imagSign().isStrictlyNegative(),
                    .isInteger = false});
    }
    case BlockType::Factorial:
      assert(GetComplexSign(t->firstChild()).isReal() &&
             GetComplexSign(t->firstChild()).isInteger());
      return ComplexOne;
    case BlockType::RealPart:
    case BlockType::ImaginaryPart: {
      ComplexSign childSign = GetComplexSign(t->firstChild());
      return ComplexSign(
          t->isRealPart() ? childSign.realSign() : childSign.imagSign(), Zero);
    }
    case BlockType::Variable:
      return Variables::GetComplexSign(t);
    default:
      return ComplexUnknown;
  }
}

}  // namespace Sign
}  // namespace PoincareJ
