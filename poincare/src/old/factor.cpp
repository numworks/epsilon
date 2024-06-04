#include <poincare/layout.h>
#include <poincare/old/arithmetic.h>
#include <poincare/old/division.h>
#include <poincare/old/factor.h>
#include <poincare/old/opposite.h>
#include <poincare/old/power.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/undefined.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <cmath>
#include <utility>

namespace Poincare {

int FactorNode::numberOfChildren() const {
  return Factor::s_functionHelper.numberOfChildren();
}

size_t FactorNode::serialize(char* buffer, size_t bufferSize,
                             Preferences::PrintFloatMode floatDisplayMode,
                             int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      Factor::s_functionHelper.aliasesList().mainAlias());
}

OExpression FactorNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return Factor(this).shallowReduce(reductionContext);
}

OExpression FactorNode::shallowBeautify(
    const ReductionContext& reductionContext) {
  return Factor(this).shallowBeautify(reductionContext);
}

Multiplication Factor::createMultiplicationOfIntegerPrimeDecomposition(
    Integer i) const {
  assert(!i.isZero());
  assert(!i.isNegative());
  Multiplication m = Multiplication::Builder();
  Arithmetic arithmetic;
  int numberOfPrimeFactors = arithmetic.PrimeFactorization(i);
  if (numberOfPrimeFactors == 0) {
    m.addChildAtIndexInPlace(Rational::Builder(i), 0, 0);
    return m;
  }
  if (numberOfPrimeFactors < 0) {
    // Exception: the decomposition failed
    return m;
  }
  for (int index = 0; index < numberOfPrimeFactors; index++) {
    OExpression factor = Rational::Builder(*arithmetic.factorAtIndex(index));
    if (!arithmetic.coefficientAtIndex(index)->isOne()) {
      factor = Power::Builder(
          factor, Rational::Builder(*arithmetic.coefficientAtIndex(index)));
    }
    m.addChildAtIndexInPlace(factor, m.numberOfChildren(),
                             m.numberOfChildren());
  }
  return m;
}

OExpression Factor::shallowReduce(ReductionContext reductionContext) {
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
  return *this;
}

OExpression Factor::shallowBeautify(const ReductionContext& reductionContext) {
  OExpression c = childAtIndex(0);
  if (c.otype() != ExpressionNode::Type::Rational) {
    return replaceWithUndefinedInPlace();
  }
  Rational r = static_cast<Rational&>(c);
  if (r.isZero()) {
    replaceWithInPlace(r);
    return std::move(r);
  }
  Multiplication numeratorDecomp =
      createMultiplicationOfIntegerPrimeDecomposition(
          r.unsignedIntegerNumerator());
  if (numeratorDecomp.numberOfChildren() == 0) {
    return replaceWithUndefinedInPlace();
  }
  OExpression result = numeratorDecomp.squashUnaryHierarchyInPlace();
  if (!r.isInteger()) {
    Multiplication denominatorDecomp =
        createMultiplicationOfIntegerPrimeDecomposition(r.integerDenominator());
    if (denominatorDecomp.numberOfChildren() == 0) {
      return replaceWithUndefinedInPlace();
    }
    result = Division::Builder(result,
                               denominatorDecomp.squashUnaryHierarchyInPlace());
  }
  if (r.isPositive() == OMG::Troolean::False) {
    result = Opposite::Builder(result);
  }
  replaceWithInPlace(result);
  return result;
}

}  // namespace Poincare
