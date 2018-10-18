#include <poincare/hyperbolic_sine.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

Layout HyperbolicSineNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(HyperbolicSine(this), floatDisplayMode, numberOfSignificantDigits, HyperbolicSine::Name());
}
int HyperbolicSineNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, HyperbolicSine::Name());
}

template<typename T>
Complex<T> HyperbolicSineNode::computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  return Complex<T>(Trigonometry::RoundToMeaningfulDigits(std::sinh(c), c));
}

template Complex<float> Poincare::HyperbolicSineNode::computeOnComplex<float>(std::complex<float>, Preferences::AngleUnit);
template Complex<double> Poincare::HyperbolicSineNode::computeOnComplex<double>(std::complex<double>, Preferences::AngleUnit);

}
