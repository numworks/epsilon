#include <poincare/layout.h>
#include <poincare/old/approximation_helper.h>
#include <poincare/old/division_quotient.h>
#include <poincare/old/infinity.h>
#include <poincare/old/multiplication.h>
#include <poincare/old/rational.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/undefined.h>

#include <cmath>

namespace Poincare {

int DivisionQuotientNode::numberOfChildren() const {
  return DivisionQuotient::s_functionHelper.numberOfChildren();
}

OMG::Troolean DivisionQuotientNode::isPositive(Context *context) const {
  OMG::Troolean numeratorPositive = childAtIndex(0)->isPositive(context);
  OMG::Troolean denominatorPositive = childAtIndex(1)->isPositive(context);
  if (numeratorPositive == OMG::Troolean::Unknown ||
      denominatorPositive == OMG::Troolean::Unknown) {
    return OMG::Troolean::Unknown;
  }
  return OMG::BoolToTroolean(numeratorPositive == denominatorPositive);
}

OExpression DivisionQuotientNode::shallowReduce(
    const ReductionContext &reductionContext) {
  return DivisionQuotient(this).shallowReduce(reductionContext);
}

size_t DivisionQuotientNode::serialize(
    char *buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      DivisionQuotient::s_functionHelper.aliasesList().mainAlias());
}

template <typename T>
Evaluation<T> DivisionQuotientNode::templatedApproximate(
    const ApproximationContext &approximationContext) const {
  return ApproximationHelper::Map<T>(
      this, approximationContext,
      [](const std::complex<T> *c, int numberOfComplexes,
         Preferences::ComplexFormat complexFormat,
         Preferences::AngleUnit angleUnit, void *ctx) -> std::complex<T> {
        assert(numberOfComplexes == 2);
        T f1 = ComplexNode<T>::ToScalar(c[0]);
        T f2 = ComplexNode<T>::ToScalar(c[1]);
        if (std::isnan(f1) || std::isnan(f2) || f1 != (int)f1 ||
            f2 != (int)f2) {
          return complexRealNAN<T>();
        }
        return DivisionQuotient::TemplatedQuotient(f1, f2);
      });
}

OExpression DivisionQuotient::shallowReduce(ReductionContext reductionContext) {
  {
    OExpression e = SimplificationHelper::defaultShallowReduce(
        *this, &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::BanUnits,
        SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
        SimplificationHelper::ListReduction::DistributeOverLists);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  OExpression c0 = childAtIndex(0);
  OExpression c1 = childAtIndex(1);
  if (c0.otype() == ExpressionNode::Type::Rational) {
    Rational r0 = static_cast<Rational &>(c0);
    if (!r0.isInteger()) {
      return replaceWithUndefinedInPlace();
    }
  }
  if (c1.otype() == ExpressionNode::Type::Rational) {
    Rational r1 = static_cast<Rational &>(c1);
    if (!r1.isInteger()) {
      return replaceWithUndefinedInPlace();
    }
  }
  if (c0.otype() != ExpressionNode::Type::Rational ||
      c1.otype() != ExpressionNode::Type::Rational) {
    return *this;
  }
  Rational r0 = static_cast<Rational &>(c0);
  Rational r1 = static_cast<Rational &>(c1);

  Integer a = r0.signedIntegerNumerator();
  Integer b = r1.signedIntegerNumerator();
  OExpression result = Reduce(a, b);
  replaceWithInPlace(result);
  return result;
}

OExpression DivisionQuotient::Reduce(const Integer &a, const Integer &b) {
  if (b.isZero()) {
    return Infinity::Builder(a.isNegative());
  }
  Integer result = Integer::Division(a, b).quotient;
  assert(!result.isOverflow());
  return Rational::Builder(result);
}

}  // namespace Poincare
