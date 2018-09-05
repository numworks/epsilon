#include <poincare/hyperbolic_arc_cosine.h>
#include <poincare/complex.h>
#include <cmath>

namespace Poincare {

template<typename T>
Complex<T> HyperbolicArcCosineNode::computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  std::complex<T> result = std::acosh(c);
  /* asinh has a branch cut on ]-inf, 1]: it is then multivalued
   * on this cut. We followed the convention chosen by the lib c++ of llvm on
   * ]-inf+0i, 1+0i] (warning: atanh takes the other side of the cut values on
   * ]-inf-0i, 1-0i[).*/
  return Complex<T>(Trigonometry::RoundToMeaningfulDigits(result));
}

template Complex<float> Poincare::HyperbolicArcCosineNode::computeOnComplex<float>(std::complex<float>, Preferences::AngleUnit);
template Complex<double> Poincare::HyperbolicArcCosineNode::computeOnComplex<double>(std::complex<double>, Preferences::AngleUnit);

}
