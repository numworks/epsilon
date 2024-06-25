#include "number.h"

#include "approximation.h"
#include "float_helper.h"
#include "rational.h"

namespace Poincare::Internal {

Tree* Number::Addition(const Tree* i, const Tree* j) {
  if (i->isDoubleFloat() || j->isDoubleFloat()) {
    return SharedTreeStack->pushDoubleFloat(Approximation::To<double>(i) +
                                            Approximation::To<double>(j));
  }
  if (i->isSingleFloat() || j->isSingleFloat()) {
    return SharedTreeStack->pushSingleFloat(Approximation::To<float>(i) +
                                            Approximation::To<float>(j));
  }
  assert(!i->isMathematicalConstant() && !j->isMathematicalConstant());
  Tree* result = Rational::Addition(i, j);
  return result;
}
Tree* Number::Multiplication(const Tree* i, const Tree* j) {
  if (i->isDoubleFloat() || j->isDoubleFloat()) {
    return SharedTreeStack->pushDoubleFloat(Approximation::To<double>(i) *
                                            Approximation::To<double>(j));
  }
  if (i->isSingleFloat() || j->isSingleFloat()) {
    return SharedTreeStack->pushSingleFloat(Approximation::To<float>(i) *
                                            Approximation::To<float>(j));
  }
  assert(!i->isMathematicalConstant() && !j->isMathematicalConstant());
  Tree* result = Rational::Multiplication(i, j);
  return result;
}

Sign Number::Sign(const Tree* node) {
  assert(node->isNumber());
  switch (node->type()) {
    case Type::Pi:
    case Type::EulerE:
      return Sign::StrictlyPositive();
    case Type::DoubleFloat:
    case Type::SingleFloat: {
      double value = FloatHelper::To(node);
      // Floats are not considered integer since they may have been rounded
      return Internal::Sign(value == 0, value > 0, value < 0, true);
    }
    default:
      assert(node->isRational());
      return Rational::Sign(node);
  }
}

bool Number::SetSign(Tree* number, NonStrictSign sign) {
  assert(number->isNumber());
  if (number->isRational()) {
    return Rational::SetSign(number, NonStrictSign::Positive);
  } else if (number->isFloat()) {
    return FloatHelper::SetSign(number, NonStrictSign::Positive);
  }
  assert(Number::Sign(number).isZero() ||
         Number::Sign(number).isPositive() ==
             (sign == NonStrictSign::Positive));
  return false;
}

}  // namespace Poincare::Internal
