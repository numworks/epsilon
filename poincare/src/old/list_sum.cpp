#include <poincare/layout.h>
#include <poincare/old/addition.h>
#include <poincare/old/list_sum.h>
#include <poincare/old/serialization_helper.h>

namespace Poincare {

int ListSumNode::numberOfChildren() const {
  return ListSum::s_functionHelper.numberOfChildren();
}

size_t ListSumNode::serialize(char* buffer, size_t bufferSize,
                              Preferences::PrintFloatMode floatDisplayMode,
                              int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      ListSum::s_functionHelper.aliasesList().mainAlias());
}

OExpression ListSumNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return ListSum(this).shallowReduce(reductionContext);
}

OExpression ListSum::shallowReduce(ReductionContext reductionContext) {
  OExpression child = childAtIndex(0);
  if (child.otype() != ExpressionNode::Type::OList) {
    return replaceWithUndefinedInPlace();
  }

  int n = child.numberOfChildren();
  Addition sum = Addition::Builder(Rational::Builder(0));
  for (int i = 0; i < n; i++) {
    sum.addChildAtIndexInPlace(child.childAtIndex(i), i, i);
  }
  replaceWithInPlace(sum);
  return sum.shallowReduce(reductionContext);
}

}  // namespace Poincare
