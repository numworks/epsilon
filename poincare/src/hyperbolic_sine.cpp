#include <poincare/derivative.h>
#include <poincare/hyperbolic_cosine.h>
#include <poincare/hyperbolic_sine.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

Layout HyperbolicSineNode::createLayout(
    Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits,
    Context* context) const {
  return LayoutHelper::Prefix(
      HyperbolicSine(this), floatDisplayMode, numberOfSignificantDigits,
      HyperbolicSine::s_functionHelper.aliasesList().mainAlias(), context);
}

int HyperbolicSineNode::serialize(char* buffer, int bufferSize,
                                  Preferences::PrintFloatMode floatDisplayMode,
                                  int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      HyperbolicSine::s_functionHelper.aliasesList().mainAlias());
}

template <typename T>
Complex<T> HyperbolicSineNode::computeOnComplex(
    const std::complex<T> c, Preferences::ComplexFormat,
    Preferences::AngleUnit angleUnit) {
  /* If c is real and large (over 100.0), the float evaluation of std::sinh
   * will return image = NaN when it should be 0.0. */
  return Complex<T>::Builder(
      ApproximationHelper::MakeResultRealIfInputIsReal<T>(
          ApproximationHelper::NeglectRealOrImaginaryPartIfNeglectable(
              std::sinh(c), c),
          c));
}

bool HyperbolicSineNode::derivate(const ReductionContext& reductionContext,
                                  Symbol symbol, Expression symbolValue) {
  return HyperbolicSine(this).derivate(reductionContext, symbol, symbolValue);
}

Expression HyperbolicSineNode::unaryFunctionDifferential(
    const ReductionContext& reductionContext) {
  return HyperbolicSine(this).unaryFunctionDifferential(reductionContext);
}

bool HyperbolicSine::derivate(const ReductionContext& reductionContext,
                              Symbol symbol, Expression symbolValue) {
  Derivative::DerivateUnaryFunction(*this, symbol, symbolValue,
                                    reductionContext);
  return true;
}

Expression HyperbolicSine::unaryFunctionDifferential(
    const ReductionContext& reductionContext) {
  return HyperbolicCosine::Builder(childAtIndex(0).clone());
}

template Complex<float> Poincare::HyperbolicSineNode::computeOnComplex<float>(
    std::complex<float>, Preferences::ComplexFormat, Preferences::AngleUnit);
template Complex<double> Poincare::HyperbolicSineNode::computeOnComplex<double>(
    std::complex<double>, Preferences::ComplexFormat complexFormat,
    Preferences::AngleUnit);

}  // namespace Poincare
