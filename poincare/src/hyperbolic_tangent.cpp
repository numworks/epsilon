#include <poincare/hyperbolic_tangent.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

Layout HyperbolicTangentNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(HyperbolicTangent(this), floatDisplayMode, numberOfSignificantDigits, HyperbolicTangent::FunctionHelper()->name());
}
int HyperbolicTangentNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, HyperbolicTangent::FunctionHelper()->name());
}

template<typename T>
Complex<T> HyperbolicTangentNode::computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  return Complex<T>(Trigonometry::RoundToMeaningfulDigits(std::tanh(c), c));
}

template Complex<float> Poincare::HyperbolicTangentNode::computeOnComplex<float>(std::complex<float>, Preferences::AngleUnit);
template Complex<double> Poincare::HyperbolicTangentNode::computeOnComplex<double>(std::complex<double>, Preferences::AngleUnit);

constexpr Expression::FunctionHelper HyperbolicTangent::m_functionHelper = Expression::FunctionHelper("tanh", 1, &HyperbolicTangent::UntypedBuilder);

}
