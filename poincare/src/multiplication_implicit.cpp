#include <poincare/multiplication_implicit.h>
#include <poincare/multiplication_explicit.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/rational.h>
#include <cmath>
#include <ion.h>
#include <assert.h>

namespace Poincare {

Layout MultiplicationImplicitNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Infix(MultiplicationImplicit(this), floatDisplayMode, numberOfSignificantDigits, "");
}

int MultiplicationImplicitNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Infix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "");
}

Expression MultiplicationImplicitNode::shallowReduce(ReductionContext reductionContext) {
  return MultiplicationImplicit(this).shallowReduce(reductionContext);
}

bool MultiplicationImplicitNode::childNeedsSystemParenthesesAtSerialization(const TreeNode * child) const {
  /*  2
   * ---i --> [2/3]i
   *  3
   */
  if (static_cast<const ExpressionNode *>(child)->type() == Type::Rational && !static_cast<const RationalNode *>(child)->isInteger()) {
    return true;
  }
  if (childAtIndex(0) == child && static_cast<const ExpressionNode *>(child)->type() == Type::Power) {
    // 2^{3}2 --> [2^3]2
    return true;
  }
  /*  2
   * ---i --> [2/π]i
   *  π
   */
  return static_cast<const ExpressionNode *>(child)->type() == Type::Division;
}

/* Multiplication */

Expression MultiplicationImplicit::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = Expression::defaultShallowReduce();
    if (e.isUndefined()) {
      return e;
    }
  }
  assert(numberOfChildren() == 2);
  MultiplicationExplicit m = MultiplicationExplicit::Builder();
  for (int i = 0; i < numberOfChildren(); i++) {
    m.addChildAtIndexInPlace(childAtIndex(i), i, i);
  }
  replaceWithInPlace(m);
  return m.shallowReduce(reductionContext);
}

}
