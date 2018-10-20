#include <poincare/real_part.h>
#include <poincare/simplification_helper.h>
#include <assert.h>
#include <cmath>

namespace Poincare {

Layout RealPartNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(RealPart(this), floatDisplayMode, numberOfSignificantDigits, name());
}

Expression RealPartNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  return RealPart(this).shallowReduce(context, angleUnit);
}

RealPart::RealPart() : Expression(TreePool::sharedPool()->createTreeNode<RealPartNode>()) {}

Expression RealPart::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  {
    Expression e = Expression::defaultShallowReduce(context, angleUnit);
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression c = childAtIndex(0);
#if MATRIX_EXACT_REDUCING
  if (c.type() == ExpressionNode::Type::Matrix) {
    return SimplificationHelper::Map(*this, context, angleUnit);
  }
#endif
  if (c.type() == ExpressionNode::Type::Rational) {
    replaceWithInPlace(c);
    return c;
  }
  return *this;
}

}
