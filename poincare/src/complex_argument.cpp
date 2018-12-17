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

Expression ComplexArgumentNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, ReductionTarget target) {
  return ComplexArgument(this).shallowReduce(context, angleUnit, target);
}

template<typename T>
Complex<T> ComplexArgumentNode::computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  return Complex<T>(std::arg(c));
}

Expression ComplexArgument::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
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
  if (c.isReal(context, angleUnit)) {
    float app = c.approximateToScalar<float>(context, angleUnit);
    if (std::isnan(app)) {
      ComplexCartesian complexChild = ComplexCartesian::Builder(c, Rational(0));
      Expression arg = complexChild.argument(context, angleUnit, target);
      replaceWithInPlace(arg);
      return arg.shallowReduce(context, angleUnit, target);
    } else if (app >= 0) {
      Expression result = Rational(0);
      replaceWithInPlace(result);
      return result;
    }
    assert(app < 0);
    Expression result = Constant(Ion::Charset::SmallPi);
    replaceWithInPlace(result);
    return result;
  }
  if (c.type() == ExpressionNode::Type::ComplexCartesian) {
    ComplexCartesian complexChild = static_cast<ComplexCartesian &>(c);
    Expression childArg = complexChild.argument(context, angleUnit, target);
    replaceWithInPlace(childArg);
    return childArg.shallowReduce(context, angleUnit, target);
  }
  return *this;
}

}
