#include <poincare/hyperbolic_sine.h>
#include <poincare/derivative.h>
#include <poincare/hyperbolic_cosine.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

constexpr Expression::FunctionHelper HyperbolicSine::s_functionHelper;

Layout HyperbolicSineNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(HyperbolicSine(this), floatDisplayMode, numberOfSignificantDigits, HyperbolicSine::s_functionHelper.name());
}

int HyperbolicSineNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, HyperbolicSine::s_functionHelper.name());
}

template<typename T>
Complex<T> HyperbolicSineNode::computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit) {
  return Complex<T>::Builder(ApproximationHelper::NeglectRealOrImaginaryPartIfNeglectable(std::sinh(c), c));
}

bool HyperbolicSineNode::derivate(ReductionContext reductionContext, Expression symbol, Expression symbolValue) {
  return HyperbolicSine(this).derivate(reductionContext, symbol, symbolValue);
}

Expression HyperbolicSineNode::unaryFunctionDifferential() {
  return HyperbolicSine(this).unaryFunctionDifferential();
}

bool HyperbolicSine::derivate(ExpressionNode::ReductionContext reductionContext, Expression symbol, Expression symbolValue) {
  Derivative::DerivateUnaryFunction(*this, symbol, symbolValue);
  return true;
}

Expression HyperbolicSine::unaryFunctionDifferential() {
  return HyperbolicCosine::Builder(childAtIndex(0).clone());
}

template Complex<float> Poincare::HyperbolicSineNode::computeOnComplex<float>(std::complex<float>, Preferences::ComplexFormat, Preferences::AngleUnit);
template Complex<double> Poincare::HyperbolicSineNode::computeOnComplex<double>(std::complex<double>, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit);

}
