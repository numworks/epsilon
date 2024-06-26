#include <poincare/layout.h>
#include <poincare/old/derivative.h>
#include <poincare/old/hyperbolic_cosine.h>
#include <poincare/old/hyperbolic_sine.h>
#include <poincare/old/serialization_helper.h>

namespace Poincare {

size_t HyperbolicSineNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      HyperbolicSine::s_functionHelper.aliasesList().mainAlias());
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

}  // namespace Poincare
