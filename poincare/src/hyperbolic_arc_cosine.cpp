#include <poincare/hyperbolic_arc_cosine.h>
#include <poincare/complex.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <cmath>

namespace Poincare {

Layout HyperbolicArcCosineNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(HyperbolicArcCosine(this), floatDisplayMode, numberOfSignificantDigits, HyperbolicArcCosine::s_functionHelper.name());
}

int HyperbolicArcCosineNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, HyperbolicArcCosine::s_functionHelper.name());
}

template<typename T>
Complex<T> HyperbolicArcCosineNode::computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit) {
  std::complex<T> result = std::acosh(c);
  /* asinh has a branch cut on ]-inf, 1]: it is then multivalued
   * on this cut. We followed the convention chosen by the lib c++ of llvm on
   * ]-inf+0i, 1+0i] (warning: atanh takes the other side of the cut values on
   * ]-inf-0i, 1-0i[).*/
  return Complex<T>::Builder(ApproximationHelper::NeglectRealOrImaginaryPartIfNeglectable(result, c));
}

template Complex<float> Poincare::HyperbolicArcCosineNode::computeOnComplex<float>(std::complex<float>, Preferences::ComplexFormat, Preferences::AngleUnit);
template Complex<double> Poincare::HyperbolicArcCosineNode::computeOnComplex<double>(std::complex<double>, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit);

}
