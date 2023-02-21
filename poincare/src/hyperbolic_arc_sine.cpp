#include <poincare/complex.h>
#include <poincare/hyperbolic_arc_sine.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>

#include <cmath>

namespace Poincare {

Layout HyperbolicArcSineNode::createLayout(
    Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits,
    Context* context) const {
  return LayoutHelper::Prefix(
      HyperbolicArcSine(this), floatDisplayMode, numberOfSignificantDigits,
      HyperbolicArcSine::s_functionHelper.aliasesList().mainAlias(), context);
}

int HyperbolicArcSineNode::serialize(
    char* buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      HyperbolicArcSine::s_functionHelper.aliasesList().mainAlias());
}

template <typename T>
Complex<T> HyperbolicArcSineNode::computeOnComplex(
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
  return Complex<T>::Builder(
      ApproximationHelper::NeglectRealOrImaginaryPartIfNeglectable(result, c));
}

template Complex<float> Poincare::HyperbolicArcSineNode::computeOnComplex<
    float>(std::complex<float>, Preferences::ComplexFormat,
           Preferences::AngleUnit);
template Complex<double> Poincare::HyperbolicArcSineNode::computeOnComplex<
    double>(std::complex<double>, Preferences::ComplexFormat complexFormat,
            Preferences::AngleUnit);

}  // namespace Poincare
