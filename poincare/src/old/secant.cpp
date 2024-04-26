#include <poincare/old/complex.h>
#include <poincare/old/cosine.h>
#include <poincare/old/layout.h>
#include <poincare/old/secant.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/trigonometry.h>

#include <cmath>

namespace Poincare {

int SecantNode::numberOfChildren() const {
  return Secant::s_functionHelper.numberOfChildren();
}

template <typename T>
std::complex<T> SecantNode::computeOnComplex(
    const std::complex<T> c, Preferences::ComplexFormat complexFormat,
    Preferences::AngleUnit angleUnit) {
  std::complex<T> denominator =
      CosineNode::computeOnComplex<T>(c, complexFormat, angleUnit);
  if (denominator == static_cast<T>(0.0)) {
    return complexNAN<T>();
  }
  return std::complex<T>(1) / denominator;
}

size_t SecantNode::serialize(char* buffer, size_t bufferSize,
                             Preferences::PrintFloatMode floatDisplayMode,
                             int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      Secant::s_functionHelper.aliasesList().mainAlias());
}

OExpression SecantNode::shallowReduce(
    const ReductionContext& reductionContext) {
  Secant e = Secant(this);
  return Trigonometry::ShallowReduceAdvancedFunction(e, reductionContext);
}

}  // namespace Poincare
