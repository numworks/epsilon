#include <poincare/frac_part.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>

#include <poincare/rational.h>
#include <cmath>

namespace Poincare {

constexpr Expression::FunctionHelper FracPart::s_functionHelper;

int FracPartNode::numberOfChildren() const { return FracPart::s_functionHelper.numberOfChildren(); }

Layout FracPartNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(FracPart(this), floatDisplayMode, numberOfSignificantDigits, FracPart::s_functionHelper.name());
}

int FracPartNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, FracPart::s_functionHelper.name());
}

Expression FracPartNode::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target, bool symbolicComputation) {
  return FracPart(this).shallowReduce(context, complexFormat, angleUnit, target, symbolicComputation);
}

template<typename T>
Complex<T> FracPartNode::computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit) {
  if (c.imag() != 0) {
    return Complex<T>::Undefined();
  }
  return Complex<T>::Builder(c.real()-std::floor(c.real()));
}


Expression FracPart::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target, bool symbolicComputation) {
  {
    Expression e = Expression::defaultShallowReduce();
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression c = childAtIndex(0);
  if (c.type() == ExpressionNode::Type::Matrix) {
    return mapOnMatrixChild(context, complexFormat, angleUnit, target, symbolicComputation);
  }
  if (c.type() != ExpressionNode::Type::Rational) {
    return *this;
  }
  Rational r = static_cast<Rational &>(c);
  IntegerDivision div = Integer::Division(r.signedIntegerNumerator(), r.integerDenominator());
  assert(!div.remainder.isOverflow());
  Integer rDenominator = r.integerDenominator();
  Expression result = Rational::Builder(div.remainder, rDenominator);
  replaceWithInPlace(result);
  return result;
}

}
