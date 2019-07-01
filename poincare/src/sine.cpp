#include <poincare/sine.h>
#include <poincare/complex.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <cmath>

namespace Poincare {

constexpr Expression::FunctionHelper Sine::s_functionHelper;

int SineNode::numberOfChildren() const { return Sine::s_functionHelper.numberOfChildren(); }

float SineNode::characteristicXRange(Context & context, Preferences::AngleUnit angleUnit) const {
  return Trigonometry::characteristicXRange(Sine(this), context, angleUnit);
}

template<typename T>
Complex<T> SineNode::computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit) {
  std::complex<T> angleInput = Trigonometry::ConvertToRadian(c, angleUnit);
  std::complex<T> res = std::sin(angleInput);
  return Complex<T>::Builder(Trigonometry::RoundToMeaningfulDigits(res, angleInput));
}

Layout SineNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(Sine(this), floatDisplayMode, numberOfSignificantDigits, Sine::s_functionHelper.name());
}

int SineNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Sine::s_functionHelper.name());
}

Expression SineNode::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target, bool symbolicComputation) {
  return Sine(this).shallowReduce(context, complexFormat, angleUnit, target);
}


Expression Sine::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  {
    Expression e = Expression::defaultShallowReduce();
    if (e.isUndefined()) {
      return e;
    }
  }
  if (childAtIndex(0).type() == ExpressionNode::Type::Matrix) {
    return SimplificationHelper::Map(*this, context, angleUnit);
  }
  return Trigonometry::shallowReduceDirectFunction(*this, context, complexFormat, angleUnit, target);
}

}
