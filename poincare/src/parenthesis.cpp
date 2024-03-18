#include <poincare/layout_helper.h>
#include <poincare/parenthesis.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>

namespace Poincare {

size_t ParenthesisNode::serialize(char* buffer, size_t bufferSize,
                                  Preferences::PrintFloatMode floatDisplayMode,
                                  int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode,
                                     numberOfSignificantDigits, "");
}

Expression ParenthesisNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return Parenthesis(this).shallowReduce(reductionContext);
}

template <typename T>
Evaluation<T> ParenthesisNode::templatedApproximate(
    const ApproximationContext& approximationContext) const {
  return childAtIndex(0)->approximate(T(), approximationContext);
}

Expression Parenthesis::shallowReduce(ReductionContext reductionContext) {
  Expression e =
      SimplificationHelper::defaultShallowReduce(*this, &reductionContext);
  if (!e.isUninitialized()) {
    return e;
  }
  Expression c = childAtIndex(0);
  replaceWithInPlace(c);
  return c;
}

}  // namespace Poincare
