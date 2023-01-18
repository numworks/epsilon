#include <poincare/hyperbolic_tangent.h>
#include <poincare/derivative.h>
#include <poincare/hyperbolic_cosine.h>
#include <poincare/layout_helper.h>
#include <poincare/power.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

Layout HyperbolicTangentNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  return LayoutHelper::Prefix(HyperbolicTangent(this), floatDisplayMode, numberOfSignificantDigits, HyperbolicTangent::s_functionHelper.aliasesList().mainAlias(), context);
}

int HyperbolicTangentNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, HyperbolicTangent::s_functionHelper.aliasesList().mainAlias());
}

template<typename T>
Complex<T> HyperbolicTangentNode::computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit) {
  return Complex<T>::Builder(ApproximationHelper::NeglectRealOrImaginaryPartIfNeglectable(std::tanh(c), c));
}

bool HyperbolicTangentNode::derivate(const ReductionContext& reductionContext, Symbol symbol, Expression symbolValue) {
  return HyperbolicTangent(this).derivate(reductionContext, symbol, symbolValue);
}

Expression HyperbolicTangentNode::unaryFunctionDifferential(const ReductionContext& reductionContext) {
  return HyperbolicTangent(this).unaryFunctionDifferential(reductionContext);
}

bool HyperbolicTangent::derivate(const ReductionContext& reductionContext, Symbol symbol, Expression symbolValue) {
  Derivative::DerivateUnaryFunction(*this, symbol, symbolValue, reductionContext);
  return true;
}

Expression HyperbolicTangent::unaryFunctionDifferential(const ReductionContext& reductionContext) {
  return Power::Builder(HyperbolicCosine::Builder(childAtIndex(0).clone()), Rational::Builder(-2));
}

template Complex<float> Poincare::HyperbolicTangentNode::computeOnComplex<float>(std::complex<float>, Preferences::ComplexFormat, Preferences::AngleUnit);
template Complex<double> Poincare::HyperbolicTangentNode::computeOnComplex<double>(std::complex<double>, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit);

}
