#include <poincare/floor.h>
#include <poincare/decimal.h>
#include <poincare/float.h>
#include <poincare/floor_layout.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/symbol.h>
#include <ion.h>
#include <assert.h>
#include <cmath>
#include <float.h>

namespace Poincare {

int FloorNode::numberOfChildren() const { return Floor::s_functionHelper.numberOfChildren(); }

Layout FloorNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return FloorLayout::Builder(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits));
}

int FloorNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Floor::s_functionHelper.name());
}

template<typename T>
Complex<T> FloorNode::computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit) {
  if (c.imag() != 0) {
    return Complex<T>::RealUndefined();
  }
  /* Assume low deviation from natural numbers are errors */
  T delta = std::fabs((std::round(c.real()) - c.real()) / c.real());
  if (delta <= Float<T>::Epsilon()) {
    return Complex<T>::Builder(std::round(c.real()));
  }
  return Complex<T>::Builder(std::floor(c.real()));
}

Expression FloorNode::shallowReduce(const ReductionContext& reductionContext) {
  return Floor(this).shallowReduce(reductionContext);
}

Expression Floor::shallowReduce(const ExpressionNode::ReductionContext& reductionContext) {
  {
    Expression e = SimplificationHelper::shallowReduceUndefinedKeepingUnitsFromFirstChild(*this, reductionContext);
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
  Expression c = childAtIndex(0);
  if (c.type() == ExpressionNode::Type::Rational) {
    Rational r = static_cast<Rational &>(c);
    IntegerDivision div = Integer::Division(r.signedIntegerNumerator(), r.integerDenominator());
    assert(!div.quotient.isOverflow());
    Expression result = Rational::Builder(div.quotient);
    replaceWithInPlace(result);
    return result;
  }
  return shallowReduceUsingApproximation(reductionContext);
}

}
