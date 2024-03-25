#include <poincare/complex.h>
#include <poincare/cosecant.h>
#include <poincare/layout.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/sine.h>
#include <poincare/trigonometry.h>

#include <cmath>

namespace Poincare {

int CosecantNode::numberOfChildren() const {
  return Cosecant::s_functionHelper.numberOfChildren();
}

template <typename T>
std::complex<T> CosecantNode::computeOnComplex(
    const std::complex<T> c, Preferences::ComplexFormat complexFormat,
    Preferences::AngleUnit angleUnit) {
  std::complex<T> denominator =
      SineNode::computeOnComplex<T>(c, complexFormat, angleUnit);
  if (denominator == static_cast<T>(0.0)) {
    return complexNAN<T>();
  }
  return std::complex<T>(1) / denominator;
}

size_t CosecantNode::serialize(char* buffer, size_t bufferSize,
                               Preferences::PrintFloatMode floatDisplayMode,
                               int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      Cosecant::s_functionHelper.aliasesList().mainAlias());
}

OExpression CosecantNode::shallowReduce(
    const ReductionContext& reductionContext) {
  Cosecant e = Cosecant(this);
  return Trigonometry::ShallowReduceAdvancedFunction(e, reductionContext);
}

}  // namespace Poincare
