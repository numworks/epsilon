#include <poincare/ceiling.h>
#include <poincare/constant.h>
#include <poincare/ceiling_layout.h>
#include <poincare/serialization_helper.h>

#include <poincare/symbol.h>
#include <poincare/rational.h>
#include <cmath>
#include <ion.h>
#include <assert.h>

namespace Poincare {

constexpr Expression::FunctionHelper Ceiling::s_functionHelper;

int CeilingNode::numberOfChildren() const { return Ceiling::s_functionHelper.numberOfChildren(); }

Layout CeilingNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return CeilingLayout::Builder(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits));
}

int CeilingNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Ceiling::s_functionHelper.name());
}

template<typename T>
Complex<T> CeilingNode::computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit) {
  if (c.imag() != 0) {
    return Complex<T>::RealUndefined();
  }
  return Complex<T>::Builder(std::ceil(c.real()));
}

Expression CeilingNode::shallowReduce(ReductionContext reductionContext) {
  return Ceiling(this).shallowReduce(reductionContext);
}


Expression Ceiling::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = Expression::defaultShallowReduce();
    e = e.defaultHandleUnitsInChildren();
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression c = childAtIndex(0);
  if (c.type() == ExpressionNode::Type::Matrix) {
    return mapOnMatrixFirstChild(reductionContext);
  }
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
