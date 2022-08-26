#include <poincare/parenthesis.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>

namespace Poincare {

int ParenthesisNode::polynomialDegree(Context * context, const char * symbolName) const {
  return childAtIndex(0)->polynomialDegree(context, symbolName);
}

Layout ParenthesisNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  return LayoutHelper::Parentheses(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits, context), false);
}

int ParenthesisNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "");
}

Expression ParenthesisNode::shallowReduce(const ReductionContext& reductionContext) {
  return Parenthesis(this).shallowReduce(reductionContext);
}

template<typename T>
Evaluation<T> ParenthesisNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  return childAtIndex(0)->approximate(T(), approximationContext);
}

Expression Parenthesis::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  Expression e = SimplificationHelper::defaultShallowReduce(*this, &reductionContext);
  if (!e.isUninitialized()) {
    return e;
  }
  Expression c = childAtIndex(0);
  replaceWithInPlace(c);
  return c;
}

}
