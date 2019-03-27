#include <poincare/tangent.h>
#include <poincare/cosine.h>
#include <poincare/division.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/sine.h>
#include <poincare/trigonometry.h>
#include <cmath>

namespace Poincare {

constexpr Expression::FunctionHelper Tangent::s_functionHelper;

int TangentNode::numberOfChildren() const { return Tangent::s_functionHelper.numberOfChildren(); }

float TangentNode::characteristicXRange(Context & context, Preferences::AngleUnit angleUnit) const {
  return Trigonometry::characteristicXRange(Tangent(this), context, angleUnit);
}

Layout TangentNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(Tangent(this), floatDisplayMode, numberOfSignificantDigits, Tangent::s_functionHelper.name());
}

int TangentNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Tangent::s_functionHelper.name());
}

template<typename T>
Complex<T> TangentNode::computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit) {
  std::complex<T> angleInput = Trigonometry::ConvertToRadian(c, angleUnit);
  std::complex<T> res = std::tan(angleInput);
  return Complex<T>::Builder(Trigonometry::RoundToMeaningfulDigits(res, angleInput));
}

Expression TangentNode::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target, bool symbolicComputation) {
  return Tangent(this).shallowReduce(context, complexFormat, angleUnit, target);
}


Expression Tangent::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  {
    Expression e = Expression::defaultShallowReduce();
    if (e.isUndefined()) {
      return e;
    }
  }
#if MATRIX_EXACT_REDUCING
  Expression op = childAtIndex(0);
  if (op.type() == ExpressionNode::Type::Matrix) {
    return SimplificationHelper::Map(*this, context, angleUnit);
  }
#endif
  Expression newExpression = Trigonometry::shallowReduceDirectFunction(*this, context, complexFormat, angleUnit, target);
  if (newExpression.type() == ExpressionNode::Type::Tangent) {
    Sine s = Sine::Builder(newExpression.childAtIndex(0).clone());
    Cosine c = Cosine::Builder(newExpression.childAtIndex(0));
    Division d = Division::Builder(s, c);
    s.shallowReduce(context, complexFormat, angleUnit, target);
    c.shallowReduce(context, complexFormat, angleUnit, target);
    newExpression.replaceWithInPlace(d);
    return d.shallowReduce(context, complexFormat, angleUnit, target);
  }
  return newExpression;
}

}
