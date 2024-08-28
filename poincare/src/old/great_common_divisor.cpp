#include <poincare/layout.h>
#include <poincare/old/arithmetic.h>
#include <poincare/old/great_common_divisor.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>

namespace Poincare {

constexpr OExpression::FunctionHelper GreatCommonDivisor::s_functionHelper;

size_t GreatCommonDivisorNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      GreatCommonDivisor::s_functionHelper.aliasesList().mainAlias());
}

OExpression GreatCommonDivisorNode::shallowBeautify(
    const ReductionContext& reductionContext) {
  return GreatCommonDivisor(this).shallowBeautify(reductionContext.context());
}

OExpression GreatCommonDivisor::shallowBeautify(Context* context) {
  /* Sort children in decreasing order:
   * gcd(1,x,x^2) --> gcd(x^2,x,1)
   * gcd(1,R(2)) --> gcd(R(2),1) */
  sortChildrenInPlace(
      [](const ExpressionNode* e1, const ExpressionNode* e2) {
        return ExpressionNode::SimplificationOrder(e1, e2, false);
      },
      context, true, false);
  return *this;
}

}  // namespace Poincare
