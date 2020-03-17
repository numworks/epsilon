#include <poincare/cosine.h>
#include <poincare/complex.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>

#include <cmath>

namespace Poincare {

constexpr Expression::FunctionHelper Cosine::s_functionHelper;

int CosineNode::numberOfChildren() const { return Cosine::s_functionHelper.numberOfChildren(); }

float CosineNode::characteristicXRange(Context * context, Preferences::AngleUnit angleUnit) const {
  return Trigonometry::characteristicXRange(Cosine(this), context, angleUnit);
}

template<typename T>
Complex<T> CosineNode::computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit) {
  std::complex<T> angleInput = Trigonometry::ConvertToRadian(c, angleUnit);
  std::complex<T> res = std::cos(angleInput);
  return Complex<T>::Builder(ApproximationHelper::NeglectRealOrImaginaryPartIfNeglectable(res, angleInput));
}

Layout CosineNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(Cosine(this), floatDisplayMode, numberOfSignificantDigits, Cosine::s_functionHelper.name());
}

int CosineNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Cosine::s_functionHelper.name());
}

Expression CosineNode::shallowReduce(ReductionContext reductionContext) {
  return Cosine(this).shallowReduce(reductionContext);
}

Expression Cosine::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
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
