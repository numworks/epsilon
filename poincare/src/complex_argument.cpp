#include <poincare/complex_argument.h>
#include <poincare/complex_cartesian.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>

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

Expression ComplexArgumentNode::shallowReduce(ReductionContext reductionContext) {
  return ComplexArgument(this).shallowReduce(reductionContext);
}

template<typename T>
Complex<T> ComplexArgumentNode::computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit) {
  return Complex<T>::Builder(std::arg(c));
}


Expression ComplexArgument::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
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
  if (c.deepIsMatrix(reductionContext.context())) {
    return *this;
  }
  bool real = c.isReal(reductionContext.context());
  if (real) {
    float app = c.node()->approximate(float(), reductionContext.context(), reductionContext.complexFormat(), reductionContext.angleUnit()).toScalar();
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
    Expression childArg = complexChild.argument(reductionContext);
    replaceWithInPlace(childArg);
    return childArg.shallowReduce(reductionContext);
  }
  return *this;
}

}
