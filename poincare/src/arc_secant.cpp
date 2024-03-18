#include <poincare/arc_cosine.h>
#include <poincare/arc_secant.h>
#include <poincare/complex.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/trigonometry.h>

#include <cmath>

namespace Poincare {

int ArcSecantNode::numberOfChildren() const {
  return ArcSecant::s_functionHelper.numberOfChildren();
}

template <typename T>
std::complex<T> ArcSecantNode::computeOnComplex(
    const std::complex<T> c, Preferences::ComplexFormat complexFormat,
    Preferences::AngleUnit angleUnit) {
  if (c == static_cast<T>(0.0)) {
    return complexNAN<T>();
  }
  return ArcCosineNode::computeOnComplex<T>(std::complex<T>(1) / c,
                                            complexFormat, angleUnit);
}

size_t ArcSecantNode::serialize(char* buffer, size_t bufferSize,
                                Preferences::PrintFloatMode floatDisplayMode,
                                int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      ArcSecant::s_functionHelper.aliasesList().mainAlias());
}

Expression ArcSecantNode::shallowReduce(
    const ReductionContext& reductionContext) {
  ArcSecant e = ArcSecant(this);
  return Trigonometry::ShallowReduceInverseAdvancedFunction(e,
                                                            reductionContext);
}

}  // namespace Poincare
