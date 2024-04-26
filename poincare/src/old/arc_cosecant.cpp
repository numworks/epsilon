#include <poincare/old/arc_cosecant.h>
#include <poincare/old/arc_sine.h>
#include <poincare/old/complex.h>
#include <poincare/old/layout.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/trigonometry.h>

#include <cmath>

namespace Poincare {

int ArcCosecantNode::numberOfChildren() const {
  return ArcCosecant::s_functionHelper.numberOfChildren();
}

template <typename T>
std::complex<T> ArcCosecantNode::computeOnComplex(
    const std::complex<T> c, Preferences::ComplexFormat complexFormat,
    Preferences::AngleUnit angleUnit) {
  if (c == static_cast<T>(0.0)) {
    return complexNAN<T>();
  }
  return ArcSineNode::computeOnComplex<T>(std::complex<T>(1) / c, complexFormat,
                                          angleUnit);
}

size_t ArcCosecantNode::serialize(char* buffer, size_t bufferSize,
                                  Preferences::PrintFloatMode floatDisplayMode,
                                  int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      ArcCosecant::s_functionHelper.aliasesList().mainAlias());
}

OExpression ArcCosecantNode::shallowReduce(
    const ReductionContext& reductionContext) {
  ArcCosecant e = ArcCosecant(this);
  return Trigonometry::ShallowReduceInverseAdvancedFunction(e,
                                                            reductionContext);
}

}  // namespace Poincare
