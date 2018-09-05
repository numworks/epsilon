#include <poincare/hyperbolic_cosine.h>

namespace Poincare {

template<typename T>
Complex<T> HyperbolicCosineNode::computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  return Complex<T>(Trigonometry::RoundToMeaningfulDigits(std::cosh(c)));
}

template Complex<float> Poincare::HyperbolicCosineNode::computeOnComplex<float>(std::complex<float>, Preferences::AngleUnit);
template Complex<double> Poincare::HyperbolicCosineNode::computeOnComplex<double>(std::complex<double>, Preferences::AngleUnit);

}
