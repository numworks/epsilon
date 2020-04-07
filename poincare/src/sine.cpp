#include <poincare/sine.h>
#include <poincare/complex.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>

#include <cmath>

namespace Poincare {

constexpr Expression::FunctionHelper Sine::s_functionHelper;

int SineNode::numberOfChildren() const { return Sine::s_functionHelper.numberOfChildren(); }

float SineNode::characteristicXRange(Context * context, Preferences::AngleUnit angleUnit) const {
  return Trigonometry::characteristicXRange(Sine(this), context, angleUnit);
}

template<typename T>
Complex<T> SineNode::computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit) {
  std::complex<T> angleInput = Trigonometry::ConvertToRadian(c, angleUnit);
  std::complex<T> res = std::sin(angleInput);
  return Complex<T>::Builder(ApproximationHelper::NeglectRealOrImaginaryPartIfNeglectable(res, angleInput));
}

Layout SineNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(Sine(this), floatDisplayMode, numberOfSignificantDigits, Sine::s_functionHelper.name());
}

int SineNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Sine::s_functionHelper.name());
}

Expression SineNode::shallowReduce(ReductionContext reductionContext) {
  return Sine(this).shallowReduce(reductionContext);
}


Expression Sine::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = Expression::defaultShallowReduce();
    e = e.defaultHandleUnitsInChildren();
    if (e.isUndefined()) {
      return e;
    }
  }
  return Trigonometry::shallowReduceDirectFunction(*this, reductionContext);
}

}
