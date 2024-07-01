#include <poincare/layout.h>
#include <poincare/old/list.h>
#include <poincare/old/list_maximum.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/undefined.h>

namespace Poincare {

int ListMaximumNode::numberOfChildren() const {
  return ListMaximum::s_functionHelper.numberOfChildren();
}

size_t ListMaximumNode::serialize(char* buffer, size_t bufferSize,
                                  Preferences::PrintFloatMode floatDisplayMode,
                                  int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      ListMaximum::s_functionHelper.aliasesList().mainAlias());
}

OExpression ListMaximumNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return ListMaximum(this).shallowReduce(reductionContext);
}

OExpression ListMaximum::shallowReduce(ReductionContext reductionContext) {
  OExpression child = childAtIndex(0);
  if (child.otype() != ExpressionNode::Type::OList ||
      child.numberOfChildren() == 0 ||
      recursivelyMatches(OExpression::IsUndefined, nullptr)) {
    return replaceWithUndefinedInPlace();
  }
  OExpression result =
      static_cast<OList&>(child).extremum(reductionContext, false);
  if (result.isUndefined()) {
    // Let approximation handle this
    return *this;
  }
  replaceWithInPlace(result);
  return result;
}

}  // namespace Poincare
