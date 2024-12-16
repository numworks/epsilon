#include "number.h"

#include <poincare/sign.h>

#include <cmath>

#include "approximation.h"
#include "float_helper.h"
#include "k_tree.h"
#include "rational.h"

namespace Poincare::Internal {

bool Number::IsNull(const Tree* e) {
  if (!e->isFloat()) {
    return e->isZero();
  }
  return Approximation::To<double>(e, Approximation::Parameters{}) == 0.0;
}

bool Number::IsOne(const Tree* e) {
  return e->isOne() || e->treeIsIdenticalTo(1_fe) || e->treeIsIdenticalTo(1_de);
}

Tree* Number::Addition(const Tree* e1, const Tree* e2) {
  if (e1->isDoubleFloat() || e2->isDoubleFloat()) {
    return SharedTreeStack->pushDoubleFloat(
        Approximation::To<double>(e1, Approximation::Parameters{}) +
        Approximation::To<double>(e2, Approximation::Parameters{}));
  }
  if (e1->isSingleFloat() || e2->isSingleFloat()) {
    return SharedTreeStack->pushSingleFloat(
        Approximation::To<float>(e1, Approximation::Parameters{}) +
        Approximation::To<float>(e2, Approximation::Parameters{}));
  }
  assert(!e1->isMathematicalConstant() && !e2->isMathematicalConstant());
  Tree* result = Rational::Addition(e1, e2);
  return result;
}
Tree* Number::Multiplication(const Tree* e1, const Tree* e2) {
  if (e1->isDoubleFloat() || e2->isDoubleFloat()) {
    // TODO: approximate the Tree to be consistent with enhanced float *
    return SharedTreeStack->pushDoubleFloat(
        Approximation::To<double>(e1, Approximation::Parameters{}) *
        Approximation::To<double>(e2, Approximation::Parameters{}));
  }
  if (e1->isSingleFloat() || e2->isSingleFloat()) {
    return SharedTreeStack->pushSingleFloat(
        Approximation::To<float>(e1, Approximation::Parameters{}) *
        Approximation::To<float>(e2, Approximation::Parameters{}));
  }
  assert(!e1->isMathematicalConstant() && !e2->isMathematicalConstant());
  Tree* result = Rational::Multiplication(e1, e2);
  return result;
}

Sign Number::Sign(const Tree* e) {
  assert(e->isNumber());
  switch (e->type()) {
    case Type::Pi:
    case Type::EulerE:
      return Sign::FiniteStrictlyPositive();
    case Type::DoubleFloat:
    case Type::SingleFloat: {
      double value = FloatHelper::To(e);
      if (std::isnan(value)) {
        return Sign::Unknown();
      }
      // Floats are not considered integer since they may have been rounded
      return Poincare::Sign(value == 0, value > 0, value < 0, true,
                            std::isinf(value));
    }
    default:
      assert(e->isRational());
      return Rational::Sign(e);
  }
}

bool Number::SetSign(Tree* e, NonStrictSign sign) {
  assert(e->isNumber());
  if (e->isRational()) {
    return Rational::SetSign(e, NonStrictSign::Positive);
  } else if (e->isFloat()) {
    return FloatHelper::SetSign(e, NonStrictSign::Positive);
  }
  assert(Number::Sign(e).isNull() ||
         Number::Sign(e).isPositive() == (sign == NonStrictSign::Positive));
  return false;
}

}  // namespace Poincare::Internal
