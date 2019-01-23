#include <poincare/complex_argument.h>
#include <poincare/complex_cartesian.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/rational.h>
#include <poincare/constant.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

constexpr Expression::FunctionHelper ComplexArgument::s_functionHelper;

int ComplexArgumentNode::numberOfChildren() const { return ComplexArgument::s_functionHelper.numberOfChildren(); }

Layout ComplexArgumentNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(ComplexArgument(this), floatDisplayMode, numberOfSignificantDigits, ComplexArgument::s_functionHelper.name());
}

int ComplexArgumentNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, ComplexArgument::s_functionHelper.name());
}

Expression ComplexArgumentNode::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) {
  return ComplexArgument(this).shallowReduce(context, complexFormat, angleUnit, target);
}

template<typename T>
Complex<T> ComplexArgumentNode::computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit) {
  return Complex<T>::Builder(std::arg(c));
}


Expression ComplexArgument::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  {
    Expression e = Expression::defaultShallowReduce();
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
  bool real = c.isReal(context);
  if (real) {
    float app = c.node()->approximate(float(), context, complexFormat, angleUnit).toScalar();
    if (!std::isnan(app) && app >= Expression::Epsilon<float>()) {
      // arg(x) = 0 if x > 0
      Expression result = Rational::Builder(0);
      replaceWithInPlace(result);
      return result;
    } else if (!std::isnan(app) && app <= -Expression::Epsilon<float>()) {
      // arg(x) = Pi if x < 0
      Expression result = Constant::Builder(UCodePointGreekSmallLetterPi);
      replaceWithInPlace(result);
      return result;
    }
  }
  if (real || c.type() == ExpressionNode::Type::ComplexCartesian) {
    ComplexCartesian complexChild = real ? ComplexCartesian::Builder(c, Rational::Builder(0)) : static_cast<ComplexCartesian &>(c);
    Expression childArg = complexChild.argument(context, complexFormat, angleUnit, target);
    replaceWithInPlace(childArg);
    return childArg.shallowReduce(context, complexFormat, angleUnit, target);
  }
  return *this;
}

}
