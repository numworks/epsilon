#include <poincare/absolute_value.h>
#include <poincare/simplification_helper.h>
#include <poincare/absolute_value_layout.h>
#include <assert.h>
#include <cmath>

namespace Poincare {

Expression AbsoluteValueNode::setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit) {
  return AbsoluteValue(this).setSign(s, context, angleUnit);
}

Layout AbsoluteValueNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return AbsoluteValueLayout(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits));
}

Expression AbsoluteValueNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  return AbsoluteValue(this).shallowReduce(context, angleUnit);
}

Expression AbsoluteValue::setSign(ExpressionNode::Sign s, Context & context, Preferences::AngleUnit angleUnit) {
  assert(s == ExpressionNode::Sign::Positive);
  return *this;
}

AbsoluteValue::AbsoluteValue() : Expression(TreePool::sharedPool()->createTreeNode<AbsoluteValueNode>()) {}

Expression AbsoluteValue::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  Expression e = Expression::defaultShallowReduce(context, angleUnit);
  if (e.isUndefined()) {
    return e;
  }
  Expression c = childAtIndex(0);
#if MATRIX_EXACT_REDUCING
#if 0
  if (c->type() == Type::Matrix) {
    return SimplificationHelper::Map(this, context, angleUnit);
  }
#endif
#endif
  if (c.sign() == ExpressionNode::Sign::Positive) {
    replaceWithInPlace(c);
    return c;
  }
  if (c.sign() == ExpressionNode::Sign::Negative) {
    Expression result = c.setSign(ExpressionNode::Sign::Positive, context, angleUnit);
    replaceWithInPlace(result);
    return result;
  }
  return *this;
}


}
