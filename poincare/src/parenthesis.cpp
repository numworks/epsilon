#include <poincare/parenthesis.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

int ParenthesisNode::polynomialDegree(char symbolName) const {
  return childAtIndex(0)->polynomialDegree(symbolName);
}

Layout ParenthesisNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Parentheses(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits), false);
}

int ParenthesisNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "");
}

Expression ParenthesisNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  return Parenthesis(this).shallowReduce(context, angleUnit);
}

template<typename T>
Evaluation<T> ParenthesisNode::templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const {
  return childAtIndex(0)->approximate(T(), context, angleUnit);
}

Expression Parenthesis::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  Expression e = Expression::defaultShallowReduce(context, angleUnit);
  if (e.isUndefined()) {
    return e;
  }
  Expression c = childAtIndex(0);
  replaceWithInPlace(c);
  return c;
}

}
