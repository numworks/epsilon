#include <poincare/hyperbolic_sine.h>

namespace Poincare {

template<typename T>
Complex<T> HyperbolicSineNode::computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  return Complex<T>(Trigonometry::RoundToMeaningfulDigits(std::sinh(c)));
}

template Complex<float> Poincare::HyperbolicSineNode::computeOnComplex<float>(std::complex<float>, Preferences::AngleUnit);
template Complex<double> Poincare::HyperbolicSineNode::computeOnComplex<double>(std::complex<double>, Preferences::AngleUnit);

}
