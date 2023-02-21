#include <poincare/arithmetic.h>
#include <poincare/division.h>
#include <poincare/factor.h>
#include <poincare/layout_helper.h>
#include <poincare/opposite.h>
#include <poincare/power.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/undefined.h>
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

Layout FactorNode::createLayout(Preferences::PrintFloatMode floatDisplayMode,
                                int numberOfSignificantDigits,
                                Context* context) const {
  return LayoutHelper::Prefix(
      Factor(this), floatDisplayMode, numberOfSignificantDigits,
      Factor::s_functionHelper.aliasesList().mainAlias(), context);
}

int FactorNode::serialize(char* buffer, int bufferSize,
                          Preferences::PrintFloatMode floatDisplayMode,
                          int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      Factor::s_functionHelper.aliasesList().mainAlias());
}

Expression FactorNode::shallowReduce(const ReductionContext& reductionContext) {
  return Factor(this).shallowReduce(reductionContext);
}

Expression FactorNode::shallowBeautify(
    const ReductionContext& reductionContext) {
  return Factor(this).shallowBeautify(reductionContext);
}

// Add tests :)
template <typename T>
Evaluation<T> FactorNode::templatedApproximate(
    const ApproximationContext& approximationContext) const {
  return ApproximationHelper::MapOneChild<T>(
      this, approximationContext,
      [](const std::complex<T> c, Preferences::ComplexFormat complexFormat,
         Preferences::AngleUnit angleUnit) {
        if (std::isnan(ComplexNode<T>::ToScalar(c))) {
          return Complex<T>::Undefined();
        }
        return Complex<T>::Builder(c);
      });
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
    Expression factor = Rational::Builder(*arithmetic.factorAtIndex(index));
    if (!arithmetic.coefficientAtIndex(index)->isOne()) {
      factor = Power::Builder(
          factor, Rational::Builder(*arithmetic.coefficientAtIndex(index)));
    }
    m.addChildAtIndexInPlace(factor, m.numberOfChildren(),
                             m.numberOfChildren());
  }
  return m;
}

Expression Factor::shallowReduce(ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(
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

Expression Factor::shallowBeautify(const ReductionContext& reductionContext) {
  Expression c = childAtIndex(0);
  if (c.type() != ExpressionNode::Type::Rational) {
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
  Expression result = numeratorDecomp.squashUnaryHierarchyInPlace();
  if (!r.isInteger()) {
    Multiplication denominatorDecomp =
        createMultiplicationOfIntegerPrimeDecomposition(r.integerDenominator());
    if (denominatorDecomp.numberOfChildren() == 0) {
      return replaceWithUndefinedInPlace();
    }
    result = Division::Builder(result,
                               denominatorDecomp.squashUnaryHierarchyInPlace());
  }
  if (r.isPositive() == TrinaryBoolean::False) {
    result = Opposite::Builder(result);
  }
  replaceWithInPlace(result);
  return result;
}

}  // namespace Poincare
