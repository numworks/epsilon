#include <poincare/complex.h>
#include <poincare/cosine.h>
#include <poincare/cotangent.h>
#include <poincare/layout.h>
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
std::complex<T> CotangentNode::computeOnComplex(
    const std::complex<T> c, Preferences::ComplexFormat complexFormat,
    Preferences::AngleUnit angleUnit) {
  std::complex<T> denominator =
      SineNode::computeOnComplex<T>(c, complexFormat, angleUnit);
  std::complex<T> numerator =
      CosineNode::computeOnComplex<T>(c, complexFormat, angleUnit);
  if (denominator == static_cast<T>(0.0)) {
    return complexNAN<T>();
  }
  return numerator / denominator;
}

size_t CotangentNode::serialize(char* buffer, size_t bufferSize,
                                Preferences::PrintFloatMode floatDisplayMode,
                                int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      Cotangent::s_functionHelper.aliasesList().mainAlias());
}

OExpression CotangentNode::shallowReduce(
    const ReductionContext& reductionContext) {
  Cotangent e = Cotangent(this);
  return Trigonometry::ShallowReduceAdvancedFunction(e, reductionContext);
}

}  // namespace Poincare
