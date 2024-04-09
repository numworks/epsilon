#include <poincare/layout.h>
#include <poincare/old/complex.h>
#include <poincare/old/hyperbolic_arc_cosine.h>
#include <poincare/old/serialization_helper.h>

#include <cmath>

namespace Poincare {

size_t HyperbolicArcCosineNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      HyperbolicArcCosine::s_functionHelper.aliasesList().mainAlias());
}

template <typename T>
std::complex<T> HyperbolicArcCosineNode::computeOnComplex(
    const std::complex<T> c, Preferences::ComplexFormat,
    Preferences::AngleUnit angleUnit) {
  std::complex<T> result = std::acosh(c);
  /* asinh has a branch cut on ]-inf, 1]: it is then multivalued
   * on this cut. We followed the convention chosen by the lib c++ of llvm on
   * ]-inf+0i, 1+0i] (warning: atanh takes the other side of the cut values on
   * ]-inf-0i, 1-0i[).*/
  return ApproximationHelper::NeglectRealOrImaginaryPartIfNeglectable(result,
                                                                      c);
}

template std::complex<float>
    Poincare::HyperbolicArcCosineNode::computeOnComplex<float>(
        std::complex<float>, Preferences::ComplexFormat,
        Preferences::AngleUnit);
template std::complex<double>
Poincare::HyperbolicArcCosineNode::computeOnComplex<double>(
    std::complex<double>, Preferences::ComplexFormat complexFormat,
    Preferences::AngleUnit);

}  // namespace Poincare
