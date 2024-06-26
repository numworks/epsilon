#include <poincare/layout.h>
#include <poincare/old/derivative.h>
#include <poincare/old/hyperbolic_cosine.h>
#include <poincare/old/hyperbolic_tangent.h>
#include <poincare/old/power.h>
#include <poincare/old/serialization_helper.h>

namespace Poincare {

size_t HyperbolicTangentNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      HyperbolicTangent::s_functionHelper.aliasesList().mainAlias());
}

bool HyperbolicTangentNode::derivate(const ReductionContext& reductionContext,
                                     Symbol symbol, OExpression symbolValue) {
  return HyperbolicTangent(this).derivate(reductionContext, symbol,
                                          symbolValue);
}

OExpression HyperbolicTangentNode::unaryFunctionDifferential(
    const ReductionContext& reductionContext) {
  return HyperbolicTangent(this).unaryFunctionDifferential(reductionContext);
}

bool HyperbolicTangent::derivate(const ReductionContext& reductionContext,
                                 Symbol symbol, OExpression symbolValue) {
  Derivative::DerivateUnaryFunction(*this, symbol, symbolValue,
                                    reductionContext);
  return true;
}

OExpression HyperbolicTangent::unaryFunctionDifferential(
    const ReductionContext& reductionContext) {
  return Power::Builder(HyperbolicCosine::Builder(childAtIndex(0).clone()),
                        Rational::Builder(-2));
}

}  // namespace Poincare
