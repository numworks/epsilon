#include <poincare/old/complex.h>
#include <poincare/old/hyperbolic_arc_sine.h>
#include <poincare/old/layout.h>
#include <poincare/old/serialization_helper.h>

#include <cmath>

namespace Poincare {

size_t HyperbolicArcSineNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      HyperbolicArcSine::s_functionHelper.aliasesList().mainAlias());
}

template <typename T>
std::complex<T> HyperbolicArcSineNode::computeOnComplex(
    const std::complex<T> c, Preferences::ComplexFormat,
    Preferences::AngleUnit angleUnit) {
  std::complex<T> result = std::asinh(c);
  /* asinh has a branch cut on ]-inf*i, -i[U]i, +inf*i[: it is then multivalued
   * on this cut. We followed the convention chosen by the lib c++ of llvm on
   * ]+i+0, +i*inf+0[ (warning: atanh takes the other side of the cut values on
   * ]+i-0, +i*inf+0[) and choose the values on ]-inf*i, -i[ to comply with
   * asinh(-x) = -asinh(x). */
  if (c.real() == 0 && c.imag() < 1) {
    result.real(-result.real());  // other side of the cut
  }
  return ApproximationHelper::NeglectRealOrImaginaryPartIfNeglectable(result,
                                                                      c);
}

template std::complex<float> Poincare::HyperbolicArcSineNode::computeOnComplex<
    float>(std::complex<float>, Preferences::ComplexFormat,
           Preferences::AngleUnit);
template std::complex<double> Poincare::HyperbolicArcSineNode::computeOnComplex<
    double>(std::complex<double>, Preferences::ComplexFormat complexFormat,
            Preferences::AngleUnit);

}  // namespace Poincare
