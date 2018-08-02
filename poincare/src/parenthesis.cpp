#include <poincare/parenthesis.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

int ParenthesisNode::polynomialDegree(char symbolName) const {
  return childAtIndex(0)->polynomialDegree(symbolName);
}

LayoutRef ParenthesisNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutEngine::createParenthesedLayout(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits), false);
}

ExpressionReference ParenthesisNode::shallowReduce(Context& context, Preferences::AngleUnit angleUnit) {
  ExpressionReference e = ExpressionNode::shallowReduce(context, angleUnit);
  if (e.node() != this) {
    return e;
  }
  return ExpressionReference(childAtIndex(0));
}

template<typename T>
EvaluationReference<T> ParenthesisNode::templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const {
  return childAtIndex(0)->approximate(T(), context, angleUnit);
}

}
