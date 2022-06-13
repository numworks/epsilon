#include <poincare/factor.h>
#include <poincare/undefined.h>
#include <poincare/arithmetic.h>
#include <poincare/power.h>
#include <poincare/division.h>
#include <poincare/opposite.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
extern "C" {
#include <stdlib.h>
#include <assert.h>
}
#include <cmath>
#include <utility>

namespace Poincare {

int FactorNode::numberOfChildren() const { return Factor::s_functionHelper.numberOfChildren(); }

Layout FactorNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(Factor(this), floatDisplayMode, numberOfSignificantDigits, Factor::s_functionHelper.name());
}

int FactorNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Factor::s_functionHelper.name());
}

Expression FactorNode::shallowReduce(const ReductionContext& reductionContext) {
  return Factor(this).shallowReduce(reductionContext);
}

Expression FactorNode::shallowBeautify(ReductionContext * reductionContext) {
  return Factor(this).shallowBeautify(reductionContext);
}

// Add tests :)
template<typename T>
Evaluation<T> FactorNode::templatedApproximate(ApproximationContext approximationContext) const {
  return ApproximationHelper::MapOneChild<T>(this,
      approximationContext,
      [] (const std::complex<T> c, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) {
        if (std::isnan(ComplexNode<T>::ToScalar(c))) {
          return Complex<T>::Undefined();
        }
        return Complex<T>::Builder(c);
      });
}


Multiplication Factor::createMultiplicationOfIntegerPrimeDecomposition(Integer i) const {
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
      factor = Power::Builder(factor, Rational::Builder(*arithmetic.coefficientAtIndex(index)));
    }
    m.addChildAtIndexInPlace(factor, m.numberOfChildren(), m.numberOfChildren());
  }
  return m;
}

Expression Factor::shallowReduce(const ExpressionNode::ReductionContext& reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(*this);
    if (!e.isUninitialized()) {
      return e;
    }
    e = SimplificationHelper::undefinedOnMatrix(*this, reductionContext);
    if (!e.isUninitialized()) {
      return e;
    }
    e = SimplificationHelper::distributeReductionOverLists(*this, reductionContext);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  return *this;
}

Expression Factor::shallowBeautify(ExpressionNode::ReductionContext * reductionContext) {
  Expression c = childAtIndex(0);
  if (c.type() != ExpressionNode::Type::Rational) {
    return replaceWithUndefinedInPlace();
  }
  Rational r = static_cast<Rational &>(c);
  if (r.isZero()) {
    replaceWithInPlace(r);
    return std::move(r);
  }
  Multiplication numeratorDecomp = createMultiplicationOfIntegerPrimeDecomposition(r.unsignedIntegerNumerator());
  if (numeratorDecomp.numberOfChildren() == 0) {
    return replaceWithUndefinedInPlace();
  }
  Expression result = numeratorDecomp.squashUnaryHierarchyInPlace();
  if (!r.isInteger()) {
    Multiplication denominatorDecomp = createMultiplicationOfIntegerPrimeDecomposition(r.integerDenominator());
    if (denominatorDecomp.numberOfChildren() == 0) {
      return replaceWithUndefinedInPlace();
    }
    result = Division::Builder(result, denominatorDecomp.squashUnaryHierarchyInPlace());
  }
  if (r.sign() == ExpressionNode::Sign::Negative) {
    result = Opposite::Builder(result);
  }
  replaceWithInPlace(result);
  return result;
}

}
