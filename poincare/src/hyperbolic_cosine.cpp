#include <poincare/hyperbolic_cosine.h>
#include <poincare/derivative.h>
#include <poincare/hyperbolic_sine.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

constexpr Expression::FunctionHelper HyperbolicCosine::s_functionHelper;

Layout HyperbolicCosineNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(HyperbolicCosine(this), floatDisplayMode, numberOfSignificantDigits, HyperbolicCosine::s_functionHelper.name());
}

int HyperbolicCosineNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, HyperbolicCosine::s_functionHelper.name());
}

template<typename T>
Complex<T> HyperbolicCosineNode::computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit) {
  return Complex<T>::Builder(ApproximationHelper::NeglectRealOrImaginaryPartIfNeglectable(std::cosh(c), c));
}

bool HyperbolicCosineNode::derivate(ReductionContext reductionContext, Expression symbol, Expression symbolValue) {
  return HyperbolicCosine(this).derivate(reductionContext, symbol, symbolValue);
}

Expression HyperbolicCosineNode::unaryFunctionDifferential() {
  return HyperbolicCosine(this).unaryFunctionDifferential();
}


bool HyperbolicCosine::derivate(ExpressionNode::ReductionContext reductionContext, Expression symbol, Expression symbolValue) {
  Derivative::DerivateUnaryFunction(*this, symbol, symbolValue);
  return true;
}

Expression HyperbolicCosine::unaryFunctionDifferential() {
  return HyperbolicSine::Builder(childAtIndex(0).clone());
}

template Complex<float> Poincare::HyperbolicCosineNode::computeOnComplex<float>(std::complex<float>, Preferences::ComplexFormat, Preferences::AngleUnit);
template Complex<double> Poincare::HyperbolicCosineNode::computeOnComplex<double>(std::complex<double>, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit);

}
