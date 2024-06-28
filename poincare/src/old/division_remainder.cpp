#include <poincare/layout.h>
#include <poincare/old/approximation_helper.h>
#include <poincare/old/division_quotient.h>
#include <poincare/old/division_remainder.h>
#include <poincare/old/infinity.h>
#include <poincare/old/rational.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/undefined.h>

#include <cmath>

namespace Poincare {

int DivisionRemainderNode::numberOfChildren() const {
  return DivisionRemainder::s_functionHelper.numberOfChildren();
}

size_t DivisionRemainderNode::serialize(
    char *buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      DivisionRemainder::s_functionHelper.aliasesList().mainAlias());
}

OExpression DivisionRemainderNode::shallowReduce(
    const ReductionContext &reductionContext) {
  return DivisionRemainder(this).shallowReduce(reductionContext);
}

OExpression DivisionRemainder::shallowReduce(
    ReductionContext reductionContext) {
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

OExpression DivisionRemainder::Reduce(const Integer &a, const Integer &b) {
  if (b.isZero()) {
    return Undefined::Builder();
  }
  Integer result = Integer::Division(a, b).remainder;
  assert(!result.isOverflow());
  return Rational::Builder(result);
}

}  // namespace Poincare
