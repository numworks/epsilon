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
bool Sign::isKnown() const {
  assert(isValid());
  return isZero() || isStrictlyPositive() || isStrictlyNegative();
}

Sign NoIntegers(Sign s) {
  return {
      .canBeNull = s.canBeNull,
      .canBePositive = s.canBeNegative,
      .canBeNegative = s.canBePositive,
      .isInteger = s.isZero(),
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

bool ComplexSign::isReal() const {
  assert(isValid());
  return imagSign.isZero();
}
bool ComplexSign::isZero() const {
  assert(isValid());
  return this == ComplexZero;
}
bool ComplexSign::isUnknown() const {
  assert(isValid());
  return this == ComplexUnknown;
}
bool ComplexSign::canBeNull() const {
  assert(isValid());
  return realSign.canBeNull && imagSign.canBeNull;
}
bool ComplexSign::isInteger() const {
  assert(isValid());
  return realSign.isInteger && imagSign.isInteger;
}

ComplexSign NoIntegers(ComplexSign s) {
  return {.realSign = NoIntegers(s.realSign),
          .imagSign = NoIntegers(s.imagSign)};
}

ComplexSign Mult(ComplexSign s1, ComplexSign s2) {
  return {.realSign = Add(Mult(s1.realSign, s2.realSign),
                          Oppose(Mult(s1.imagSign, s2.imagSign))),
          .imagSign = Add(Mult(s1.realSign, s2.imagSign),
                          Mult(s1.imagSign, s2.realSign))};
}

ComplexSign Add(ComplexSign s1, ComplexSign s2) {
  return {.realSign = Add(s1.realSign, s2.realSign),
          .imagSign = Add(s1.imagSign, s2.imagSign)};
}

// Note: A complex function plotter can be used to fill in these methods.
ComplexSign GetComplexSign(const Tree* t) {
  assert(Dimension::GetDimension(t).isScalar());
  if (t->isNumber()) {
    return {.realSign = Number::Sign(t), .imagSign = Zero};
  }
  switch (t->type()) {
    case BlockType::Multiplication: {
      ComplexSign s = ComplexOne;
      for (const Tree* c : t->children()) {
        s = Mult(s, GetSign(c));
        if (s.isUnknown() || s.isZero()) {
          break;
        }
      }
      return s;
    }
    case BlockType::Addition: {
      ComplexSign s = ComplexZero;
      for (const Tree* c : t->children()) {
        s = Add(s, GetSign(c));
        if (s.isUnknown()) {
          break;
        }
      }
      return s;
    }
    case BlockType::Power: {
      ComplexSign base = GetSign(t->firstChild());
      ComplexSign exp = GetSign(t->child(1));
      // If this assert can;t be maintained, escape with Unknown.
      assert(exp.isReal() && exp.isInteger());
      if (base.isZero()) {
        return ComplexZero;
      }
      if (exp.isZero()) {
        return ComplexOne;
      }
      bool isInteger = (base.isInteger() && !exp.isPositive());
      bool baseIsReal = base.isReal();
      return {.realSign = {.canBeNull = base.realSign.canBeNull,
                           .canBePositive = true,
                           .canBeNegative =
                               !(baseIsReal && base.realSign.isPositive()),
                           .isInteger = isInteger},
              .imagSign = {.canBeNull = base.imagSign.canBeNull,
                           .canBePositive = !baseIsReal,
                           .canBeNegative = !baseIsReal,
                           .isInteger = isInteger || baseIsReal}};
    }
    case BlockType::Norm:
      // Child isn't a scalar
      return {.realSign = PositiveOrNull, .imagSign = Zero};
    case BlockType::Abs: {
      ComplexSign s = GetSign(t->firstChild());
      return {
          .canBeNull = s.canBeNull(),
          .canBePositive = !s.isZero(),
          .canBeNegative = false,
          .isInteger = s.isInteger() && (s.isReal() || s.realSign.isZero())};
    }
    case BlockType::ArcSine:
    case BlockType::ArcTangent:
      // Both real and imaginary part keep the same sign
      return NoIntegers(GetSign(t->firstChild()));
    case BlockType::ArcCosine:
      ComplexSign s = GetSign(t->firstChild());
      return {.realSign = {.canBeNull = s.realSign.canBePositive,
                           .canBePositive = true,
                           .canBeNegative = false,
                           .isInteger = false},
              .imagSign = {.canBeNull = s.imagSign.canBeNull,
                           .canBePositive = s.imagSign.canBeNegative ||
                                            (s.imagSign.canBeNull &&
                                             s.realSign.canBePositive),
                           .canBeNegative = s.imagSign.canBePositive ||
                                            (s.imagSign.canBeNull &&
                                             s.realSign.canBeNegative),
                           .isInteger = false}};
    case BlockType::Exponential:
      ComplexSign s = GetSign(t->firstChild());
      bool childIsReal = s.isReal();
      return {.realSign = {.canBeNull = !childIsReal,
                           .canBePositive = childIsReal,
                           .canBeNegative = !childIsReal,
                           .isInteger = false},
              .imagSign = {.canBeNull = true,
                           .canBePositive = !childIsReal,
                           .canBeNegative = !childIsReal,
                           .isInteger = childIsReal}};
    case BlockType::Factorial:
      assert(GetSign(t->firstChild()).isReal() &&
             GetSign(t->firstChild()).isInteger());
      return ComplexOne;
    case BlockType::Variable:
      return Variables::GetSign(t);
    default:
      return ComplexUnknown;
  }
}

}  // namespace Sign
}  // namespace PoincareJ
