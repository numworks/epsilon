#include <poincare/multiplication_implicite.h>
#include <poincare/multiplication_explicite.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/rational.h>
#include <cmath>
#include <ion.h>
#include <assert.h>

namespace Poincare {

Layout MultiplicationImpliciteNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Infix(MultiplicationImplicite(this), floatDisplayMode, numberOfSignificantDigits, "");
}

int MultiplicationImpliciteNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Infix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "");
}

Expression MultiplicationImpliciteNode::shallowReduce(ReductionContext reductionContext) {
  return MultiplicationImplicite(this).shallowReduce(reductionContext);
}

bool MultiplicationImpliciteNode::childNeedsSystemParenthesesAtSerialization(const TreeNode * child) const {
  /*  2
   * ---i --> [2/3]i
   *  3
   */
  if (static_cast<const ExpressionNode *>(child)->type() == Type::Rational && !static_cast<const RationalNode *>(child)->isInteger()) {
    return true;
  }
  // 2^{3}i --> [2^3]i
  Type types[] = {Type::Power, Type::Opposite, Type::MultiplicationExplicite, Type::Division, Type::Factorial};
  return static_cast<const ExpressionNode *>(child)->isOfType(types, 5);
}

/* Multiplication */

Expression MultiplicationImplicite::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = Expression::defaultShallowReduce();
    if (e.isUndefined()) {
      return e;
    }
  }
  assert(numberOfChildren() == 2);
  MultiplicationExplicite m = MultiplicationExplicite::Builder();
  for (int i = 0; i < numberOfChildren(); i++) {
    m.addChildAtIndexInPlace(childAtIndex(i), i, i);
  }
  replaceWithInPlace(m);
  return m.shallowReduce(reductionContext);
}

}
