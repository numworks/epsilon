#include <poincare/floor.h>
#include <poincare/constant.h>
#include <poincare/floor_layout.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/symbol.h>
#include <ion.h>
#include <assert.h>
#include <cmath>

namespace Poincare {

constexpr Expression::FunctionHelper Floor::s_functionHelper;

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
    return Complex<T>::Undefined();
  }
  return Complex<T>::Builder(std::floor(c.real()));
}

Expression FloorNode::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target, bool symbolicComputation) {
  return Floor(this).shallowReduce(context, angleUnit);
}

Expression Floor::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  {
    Expression e = Expression::defaultShallowReduce();
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression c = childAtIndex(0);
  if (c.type() == ExpressionNode::Type::Matrix) {
    return SimplificationHelper::Map(*this, context, angleUnit);
  }
  if (c.type() == ExpressionNode::Type::Constant) {
    Constant s = static_cast<Constant &>(c);
    Expression result;
    if (s.isPi()) {
      result = Rational::Builder(3);
    }
    if (s.isExponential()) {
      result = Rational::Builder(2);
    }
    if (!result.isUninitialized()) {
      replaceWithInPlace(result);
      return result;
    }
    return *this;
  }
  if (c.type() != ExpressionNode::Type::Rational) {
    return *this;
  }
  Rational r = static_cast<Rational &>(c);
  IntegerDivision div = Integer::Division(r.signedIntegerNumerator(), r.integerDenominator());
  assert(!div.quotient.isOverflow());
  Expression result = Rational::Builder(div.quotient);
  replaceWithInPlace(result);
  return result;
}

}
