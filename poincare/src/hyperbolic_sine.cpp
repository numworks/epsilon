#include <poincare/derivative.h>
#include <poincare/hyperbolic_cosine.h>
#include <poincare/hyperbolic_sine.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

size_t HyperbolicSineNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      HyperbolicSine::s_functionHelper.aliasesList().mainAlias());
}

template <typename T>
std::complex<T> HyperbolicSineNode::computeOnComplex(
    const std::complex<T> c, Preferences::ComplexFormat,
    Preferences::AngleUnit angleUnit) {
  /* If c is real and large (over 100.0), the float evaluation of std::sinh
   * will return image = NaN when it should be 0.0. */
  return ApproximationHelper::MakeResultRealIfInputIsReal<T>(
      ApproximationHelper::NeglectRealOrImaginaryPartIfNeglectable(std::sinh(c),
                                                                   c),
      c);
}

bool HyperbolicSineNode::derivate(const ReductionContext& reductionContext,
                                  Symbol symbol, OExpression symbolValue) {
  return HyperbolicSine(this).derivate(reductionContext, symbol, symbolValue);
}

OExpression HyperbolicSineNode::unaryFunctionDifferential(
    const ReductionContext& reductionContext) {
  return HyperbolicSine(this).unaryFunctionDifferential(reductionContext);
}

bool HyperbolicSine::derivate(const ReductionContext& reductionContext,
                              Symbol symbol, OExpression symbolValue) {
  Derivative::DerivateUnaryFunction(*this, symbol, symbolValue,
                                    reductionContext);
  return true;
}

OExpression HyperbolicSine::unaryFunctionDifferential(
    const ReductionContext& reductionContext) {
  return HyperbolicCosine::Builder(childAtIndex(0).clone());
}

template std::complex<float> Poincare::HyperbolicSineNode::computeOnComplex<
    float>(std::complex<float>, Preferences::ComplexFormat,
           Preferences::AngleUnit);
template std::complex<double> Poincare::HyperbolicSineNode::computeOnComplex<
    double>(std::complex<double>, Preferences::ComplexFormat complexFormat,
            Preferences::AngleUnit);

}  // namespace Poincare
