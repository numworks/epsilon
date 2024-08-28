#include <poincare/layout.h>
#include <poincare/old/arithmetic.h>
#include <poincare/old/least_common_multiple.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>

namespace Poincare {

constexpr OExpression::FunctionHelper LeastCommonMultiple::s_functionHelper;

size_t LeastCommonMultipleNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      LeastCommonMultiple::s_functionHelper.aliasesList().mainAlias());
}

OExpression LeastCommonMultipleNode::shallowBeautify(
    const ReductionContext& reductionContext) {
  return LeastCommonMultiple(this).shallowBeautify(reductionContext.context());
}

OExpression LeastCommonMultiple::shallowBeautify(Context* context) {
  /* Sort children in decreasing order:
   * lcm(1,x,x^2) --> lcm(x^2,x,1)
   * lcm(1,R(2)) --> lcm(R(2),1) */
  sortChildrenInPlace(
      [](const ExpressionNode* e1, const ExpressionNode* e2) {
        return ExpressionNode::SimplificationOrder(e1, e2, false);
      },
      context, true, false);
  return *this;
}

OExpression LeastCommonMultiple::shallowReduce(
    ReductionContext reductionContext) {
  return OExpression();
}

}  // namespace Poincare
