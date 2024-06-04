#include <assert.h>
#include <poincare/layout.h>
#include <poincare/old/approximation_helper.h>
#include <poincare/old/binomial_coefficient.h>
#include <poincare/old/rational.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/undefined.h>
#include <poincare/preferences.h>
#include <stdlib.h>

#include <cmath>
#include <utility>

namespace Poincare {

int BinomialCoefficientNode::numberOfChildren() const {
  return BinomialCoefficient::s_functionHelper.numberOfChildren();
}

OExpression BinomialCoefficientNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return BinomialCoefficient(this).shallowReduce(reductionContext);
}

size_t BinomialCoefficientNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      BinomialCoefficient::s_functionHelper.aliasesList().mainAlias());
}

OExpression BinomialCoefficient::shallowReduce(
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

  if (c0.otype() != ExpressionNode::Type::Rational ||
      c1.otype() != ExpressionNode::Type::Rational) {
    return *this;
  }

  Rational r0 = static_cast<Rational&>(c0);
  Rational r1 = static_cast<Rational&>(c1);

  if (!r1.isInteger()) {
    // Our factorial is only defined on integers.
    return replaceWithUndefinedInPlace();
  }

  if (r1.isNegative() || r1.isZero()) {
    // By convention, reduce to 0 if k is strictly negative, 1 if k is null.
    Rational result = r1.isZero() ? Rational::Builder(1) : Rational::Builder(0);
    replaceWithInPlace(result);
    return std::move(result);
  }

  if (!r0.isInteger()) {
    // Generalized binomial coefficient (n is not an integer)
    return *this;
  }

  Integer n = r0.signedIntegerNumerator();
  Integer k = r1.signedIntegerNumerator();
  /* Check for situations where there should be no reduction in order to avoid
   * too long computation and a huge result. The binomial coefficient will be
   * approximatively evaluated later. */
  if (n.isLowerThan(k)) {
    // Generalized binomial coefficient (n < k)
    if (!n.isNegative()) {
      // When n is an integer and 0 <= n < k, binomial(n,k) is 0.
      OExpression res = Rational::Builder(0);
      replaceWithInPlace(res);
      return res;
    }
    if (Integer(k_maxNValue).isLowerThan(Integer::Subtraction(k, n))) {
      return *this;
    }
  } else if (Integer(k_maxNValue).isLowerThan(n)) {
    return *this;
  }
  Rational result = Rational::Builder(1);
  Integer kBis = Integer::Subtraction(n, k);
  // Take advantage of symmetry if n >= k
  k = !n.isLowerThan(k) && kBis.isLowerThan(k) ? kBis : k;
  // Authorized because k < k_maxNValue
  int clippedK = k.extractedInt();
  for (int i = 0; i < clippedK; i++) {
    Integer nMinusI = Integer::Subtraction(n, Integer(i));
    Integer kMinusI = Integer::Subtraction(k, Integer(i));
    Rational factor = Rational::Builder(nMinusI, kMinusI);
    result = Rational::Multiplication(result, factor);
  }
  /* As we cap the n < k_maxNValue = 300, result < binomial(300, 150) ~10^89
   * If n was negative, k - n < k_maxNValue, result < binomial(-150,150) ~10^88
   */
  assert(!result.numeratorOrDenominatorIsInfinity());
  replaceWithInPlace(result);
  return std::move(result);
}

}  // namespace Poincare
