#include <poincare/layout.h>
#include <poincare/old/derivative.h>
#include <poincare/old/hyperbolic_cosine.h>
#include <poincare/old/hyperbolic_sine.h>
#include <poincare/old/serialization_helper.h>

namespace Poincare {

size_t HyperbolicCosineNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      HyperbolicCosine::s_functionHelper.aliasesList().mainAlias());
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

}  // namespace Poincare
