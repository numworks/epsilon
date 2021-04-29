#include <poincare/factor.h>
#include <poincare/undefined.h>
#include <poincare/arithmetic.h>
#include <poincare/power.h>
#include <poincare/division.h>
#include <poincare/opposite.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/exception_checkpoint.h>
extern "C" {
#include <stdlib.h>
#include <assert.h>
}
#include <cmath>
#include <utility>

namespace Poincare {

constexpr Expression::FunctionHelper Factor::s_functionHelper;

int FactorNode::numberOfChildren() const { return Factor::s_functionHelper.numberOfChildren(); }

Expression FactorNode::setSign(Sign s, ReductionContext reductionContext) {
  return Factor(this).setSign(s, reductionContext);
}

Layout FactorNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(Factor(this), floatDisplayMode, numberOfSignificantDigits, Factor::s_functionHelper.name());
}

int FactorNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Factor::s_functionHelper.name());
}

Expression FactorNode::shallowReduce(ReductionContext reductionContext) {
  return Factor(this).shallowReduce(reductionContext.context());
}

Expression FactorNode::shallowBeautify(ReductionContext * reductionContext) {
  return Factor(this).shallowBeautify(reductionContext);
}

// Add tests :)
template<typename T>
Evaluation<T> FactorNode::templatedApproximate(ApproximationContext approximationContext) const {
  Evaluation<T> e = childAtIndex(0)->approximate(T(), approximationContext);
  if (std::isnan(e.toScalar())) {
    return Complex<T>::Undefined();
  }
  return e;
}


Multiplication Factor::createMultiplicationOfIntegerPrimeDecomposition(Integer i) const {
  assert(!i.isZero());
  assert(!i.isNegative());
  Multiplication m = Multiplication::Builder();
  {
    // See comment in Arithmetic::resetPrimeFactorization()
    ExceptionCheckpoint tempEcp;
    if (ExceptionRun(tempEcp)) {
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
        Expression factor = Rational::Builder(*arithmetic.factorizationFactorAtIndex(index));
        if (!arithmetic.factorizationCoefficientAtIndex(index)->isOne()) {
          factor = Power::Builder(factor, Rational::Builder(*arithmetic.factorizationCoefficientAtIndex(index)));
        }
        m.addChildAtIndexInPlace(factor, m.numberOfChildren(), m.numberOfChildren());
      }
      return m;
    } else {
      // Reset factorization
      Arithmetic::resetPrimeFactorization();
    }
  }
  // As tempEcp has been destroyed, fall back on parent exception checkpoint
  ExceptionCheckpoint::Raise();
  // Return to silence warnings
  return Multiplication::Builder();
}

Expression Factor::setSign(ExpressionNode::Sign s, ExpressionNode::ReductionContext reductionContext) {
  return defaultOddFunctionSetSign(s, reductionContext);
}

Expression Factor::shallowReduce(Context * context) {
  {
    Expression e = Expression::defaultShallowReduce();
    e = e.defaultHandleUnitsInChildren();
    if (e.isUndefined()) {
      return e;
    }
  }
  if (childAtIndex(0).deepIsMatrix(context)) {
    return replaceWithUndefinedInPlace();
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
