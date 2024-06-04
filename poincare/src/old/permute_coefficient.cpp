#include <poincare/layout.h>
#include <poincare/old/approximation_helper.h>
#include <poincare/old/permute_coefficient.h>
#include <poincare/old/rational.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/undefined.h>
#include <poincare/preferences.h>

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

int PermuteCoefficientNode::numberOfChildren() const {
  return PermuteCoefficient::s_functionHelper.numberOfChildren();
}

size_t PermuteCoefficientNode::serialize(
    char *buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      PermuteCoefficient::s_functionHelper.aliasesList().mainAlias());
}

OExpression PermuteCoefficientNode::shallowReduce(
    const ReductionContext &reductionContext) {
  return PermuteCoefficient(this).shallowReduce(reductionContext);
}

OExpression PermuteCoefficient::shallowReduce(
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
    if (!r0.isInteger() || r0.isPositive() == OMG::Troolean::False) {
      return replaceWithUndefinedInPlace();
    }
  }
  if (c1.otype() == ExpressionNode::Type::Rational) {
    Rational r1 = static_cast<Rational &>(c1);
    if (!r1.isInteger() || r1.isPositive() == OMG::Troolean::False) {
      return replaceWithUndefinedInPlace();
    }
  }
  if (c0.otype() != ExpressionNode::Type::Rational ||
      c1.otype() != ExpressionNode::Type::Rational) {
    return *this;
  }
  Rational r0 = static_cast<Rational &>(c0);
  Rational r1 = static_cast<Rational &>(c1);

  Integer n = r0.unsignedIntegerNumerator();
  Integer k = r1.unsignedIntegerNumerator();
  if (n.isLowerThan(k)) {
    OExpression result = Rational::Builder(0);
    replaceWithInPlace(result);
    return result;
  }
  /* if n is too big, we do not reduce to avoid too long computation.
   * The permute coefficient will be evaluate approximatively later */
  if (Integer(k_maxNValue).isLowerThan(n)) {
    return *this;
  }
  Integer result(1);
  // Authorized because k < n < k_maxNValue
  int clippedK = k.extractedInt();
  for (int i = 0; i < clippedK; i++) {
    Integer factor = Integer::Subtraction(n, Integer(i));
    result = Integer::Multiplication(result, factor);
  }
  assert(!result.isOverflow());  // < permute(k_maxNValue, k_maxNValue-1)~10^158
  OExpression rationalResult = Rational::Builder(result);
  replaceWithInPlace(rationalResult);
  return rationalResult;
}

}  // namespace Poincare
