#include <poincare/frac_part.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/rational.h>
#include <cmath>

namespace Poincare {

Layout FracPartNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(FracPart(this), floatDisplayMode, numberOfSignificantDigits, name());
}

int FracPartNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, name());
}

Expression FracPartNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  return FracPart(this).shallowReduce(context, angleUnit);
}

template<typename T>
Complex<T> FracPartNode::computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  if (c.imag() != 0) {
    return Complex<T>::Undefined();
  }
  return Complex<T>(c.real()-std::floor(c.real()));
}

FracPart::FracPart() : Expression(TreePool::sharedPool()->createTreeNode<FracPartNode>()) {}

Expression FracPart::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  {
    Expression e = Expression::defaultShallowReduce(context, angleUnit);
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression c = childAtIndex(0);
#if MATRIX_EXACT_REDUCING
  if (c.type() == ExpressionNode::Type::Matrix) {
    return SimplificationHelper::Map(*this, context, angleUnit);
  }
#endif
  if (c.type() != ExpressionNode::Type::Rational) {
    return *this;
  }
  Rational r = static_cast<Rational &>(c);
  IntegerDivision div = Integer::Division(r.signedIntegerNumerator(), r.integerDenominator());
  assert(!div.remainder.isInfinity());
  Integer rDenominator = r.integerDenominator();
  Expression result = Rational(div.remainder, rDenominator);
  replaceWithInPlace(result);
  return result;
}

}
