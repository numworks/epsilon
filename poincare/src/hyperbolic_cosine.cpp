#include <poincare/derivative.h>
#include <poincare/hyperbolic_cosine.h>
#include <poincare/hyperbolic_sine.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

size_t HyperbolicCosineNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      HyperbolicCosine::s_functionHelper.aliasesList().mainAlias());
}

template <typename T>
std::complex<T> HyperbolicCosineNode::computeOnComplex(
    const std::complex<T> c, Preferences::ComplexFormat,
    Preferences::AngleUnit angleUnit) {
  /* If c is real and large (over 100.0), the float evaluation of std::cosh
   * will return image = NaN when it should be 0.0. */
  return ApproximationHelper::MakeResultRealIfInputIsReal<T>(
      ApproximationHelper::NeglectRealOrImaginaryPartIfNeglectable(std::cosh(c),
                                                                   c),
      c);
}

bool HyperbolicCosineNode::derivate(const ReductionContext& reductionContext,
                                    Symbol symbol, OExpression symbolValue) {
  return HyperbolicCosine(this).derivate(reductionContext, symbol, symbolValue);
}

OExpression HyperbolicCosineNode::unaryFunctionDifferential(
    const ReductionContext& reductionContext) {
  return HyperbolicCosine(this).unaryFunctionDifferential(reductionContext);
}

bool HyperbolicCosine::derivate(const ReductionContext& reductionContext,
                                Symbol symbol, OExpression symbolValue) {
  Derivative::DerivateUnaryFunction(*this, symbol, symbolValue,
                                    reductionContext);
  return true;
}

OExpression HyperbolicCosine::unaryFunctionDifferential(
    const ReductionContext& reductionContext) {
  return HyperbolicSine::Builder(childAtIndex(0).clone());
}

template std::complex<float> Poincare::HyperbolicCosineNode::computeOnComplex<
    float>(std::complex<float>, Preferences::ComplexFormat,
           Preferences::AngleUnit);
template std::complex<double> Poincare::HyperbolicCosineNode::computeOnComplex<
    double>(std::complex<double>, Preferences::ComplexFormat complexFormat,
            Preferences::AngleUnit);

}  // namespace Poincare
