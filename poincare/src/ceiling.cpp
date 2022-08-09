#include <poincare/ceiling.h>
#include <poincare/constant.h>
#include <poincare/ceiling_layout.h>
#include <poincare/float.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/symbol.h>
#include <cmath>
#include <ion.h>
#include <assert.h>
#include <float.h>

namespace Poincare {

int CeilingNode::numberOfChildren() const { return Ceiling::s_functionHelper.numberOfChildren(); }

Layout CeilingNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  return CeilingLayout::Builder(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits, context));
}

int CeilingNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Ceiling::s_functionHelper.aliasesList().mainAlias());
}

template<typename T>
Complex<T> CeilingNode::computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit) {
  if (c.imag() != 0) {
    return Complex<T>::RealUndefined();
  }
  /* Assume low deviation from natural numbers are errors */
  T delta = std::fabs((std::round(c.real()) - c.real()) / c.real());
  if (delta <= Float<T>::Epsilon()) {
    return Complex<T>::Builder(std::round(c.real()));
  }
  return Complex<T>::Builder(std::ceil(c.real()));
}

Expression CeilingNode::shallowReduce(const ReductionContext& reductionContext) {
  return Ceiling(this).shallowReduce(reductionContext);
}


Expression Ceiling::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(
        *this,
        &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::ExtractUnitsOfFirstChild,
        SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
        SimplificationHelper::ListReduction::DistributeOverLists
    );
    if (!e.isUninitialized()) {
      return e;
    }
  }

  Expression c = childAtIndex(0);
  if (c.type() == ExpressionNode::Type::Rational) {
    Rational r = c.convert<Rational>();
    IntegerDivision div = Integer::Division(r.signedIntegerNumerator(), r.integerDenominator());
    assert(!div.remainder.isOverflow());
    if (div.remainder.isZero()) {
      Expression result = Rational::Builder(div.quotient);
      replaceWithInPlace(result);
      return result;
    }
    Integer result = Integer::Addition(div.quotient, Integer(1));
    if (result.isOverflow()) {
      return *this;
    }
    Expression rationalResult = Rational::Builder(result);
    replaceWithInPlace(rationalResult);
    return rationalResult;
  }
  return shallowReduceUsingApproximation(reductionContext);
}

}
