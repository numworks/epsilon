#include <poincare/layout.h>
#include <poincare/old/complex.h>
#include <poincare/old/hyperbolic_arc_tangent.h>
#include <poincare/old/serialization_helper.h>

#include <cmath>

namespace Poincare {

size_t HyperbolicArcTangentNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      HyperbolicArcTangent::s_functionHelper.aliasesList().mainAlias());
}

template <typename T>
std::complex<T> HyperbolicArcTangentNode::computeOnComplex(
    const std::complex<T> c, Preferences::ComplexFormat,
    Preferences::AngleUnit angleUnit) {
  std::complex<T> result = std::atanh(c);
  /* atanh has a branch cut on ]-inf, -1[U]1, +inf[: it is then multivalued on
   * this cut. We followed the convention chosen by the lib c++ of llvm on
   * ]-inf+0i, -1+0i[ (warning: atanh takes the other side of the cut values on
   * ]-inf-0i, -1-0i[) and choose the values on ]1+0i, +inf+0i[ to comply with
   * atanh(-x) = -atanh(x) and sin(artanh(x)) = x/sqrt(1-x^2). */
  if (c.imag() == 0 && c.real() > 1) {
    result.imag(-result.imag());  // other side of the cut
  }
  return ApproximationHelper::NeglectRealOrImaginaryPartIfNeglectable(result,
                                                                      c);
}

template std::complex<float>
    Poincare::HyperbolicArcTangentNode::computeOnComplex<float>(
        std::complex<float>, Preferences::ComplexFormat,
        Preferences::AngleUnit);
template std::complex<double>
Poincare::HyperbolicArcTangentNode::computeOnComplex<double>(
    std::complex<double>, Preferences::ComplexFormat complexFormat,
    Preferences::AngleUnit);

}  // namespace Poincare
