#include <poincare/complex.h>
#include <poincare/cosine.h>
#include <poincare/cotangent.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/sine.h>
#include <poincare/trigonometry.h>

#include <cmath>

namespace Poincare {

int CotangentNode::numberOfChildren() const {
  return Cotangent::s_functionHelper.numberOfChildren();
}

template <typename T>
Complex<T> CotangentNode::computeOnComplex(
    const std::complex<T> c, Preferences::ComplexFormat complexFormat,
    Preferences::AngleUnit angleUnit) {
  std::complex<T> denominator =
      SineNode::computeOnComplex<T>(c, complexFormat, angleUnit)
          .complexAtIndex(0);
  std::complex<T> numerator =
      CosineNode::computeOnComplex<T>(c, complexFormat, angleUnit)
          .complexAtIndex(0);
  if (denominator == static_cast<T>(0.0)) {
    return Complex<T>::Undefined();
  }
  return Complex<T>::Builder(numerator / denominator);
}

Layout CotangentNode::createLayout(Preferences::PrintFloatMode floatDisplayMode,
                                   int numberOfSignificantDigits,
                                   Context* context) const {
  return LayoutHelper::Prefix(
      Cotangent(this), floatDisplayMode, numberOfSignificantDigits,
      Cotangent::s_functionHelper.aliasesList().mainAlias(), context);
}

int CotangentNode::serialize(char* buffer, int bufferSize,
                             Preferences::PrintFloatMode floatDisplayMode,
                             int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      Cotangent::s_functionHelper.aliasesList().mainAlias());
}

Expression CotangentNode::shallowReduce(
    const ReductionContext& reductionContext) {
  Cotangent e = Cotangent(this);
  return Trigonometry::shallowReduceAdvancedFunction(e, reductionContext);
}

}  // namespace Poincare