#include <poincare/hyperbolic_cosine.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

Layout HyperbolicCosineNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(HyperbolicCosine(this), floatDisplayMode, numberOfSignificantDigits, HyperbolicCosine::FunctionHelper()->name());
}
int HyperbolicCosineNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, HyperbolicCosine::FunctionHelper()->name());
}

template<typename T>
Complex<T> HyperbolicCosineNode::computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  return Complex<T>(Trigonometry::RoundToMeaningfulDigits(std::cosh(c), c));
}

template Complex<float> Poincare::HyperbolicCosineNode::computeOnComplex<float>(std::complex<float>, Preferences::AngleUnit);
template Complex<double> Poincare::HyperbolicCosineNode::computeOnComplex<double>(std::complex<double>, Preferences::AngleUnit);

constexpr Expression::FunctionHelper HyperbolicCosine::m_functionHelper = Expression::FunctionHelper("cosh", 1, &HyperbolicCosine::UntypedBuilder);

}
