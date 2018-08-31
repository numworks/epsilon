#include <poincare/absolute_value.h>
#include <poincare/simplification_helper.h>
#include <poincare/absolute_value_layout_node.h>
#include <poincare/allocation_failure_layout_node.h>

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

AbsoluteValueNode * AbsoluteValueNode::FailedAllocationStaticNode() {
  static AllocationFailureExpressionNode<AbsoluteValueNode> failure;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failure);
  return &failure;
}

Expression AbsoluteValueNode::setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit) {
  return AbsoluteValue(this).setSign(s, context, angleUnit);
}

template<typename T>
Complex<T> AbsoluteValueNode::computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  return Complex<T>(std::abs(c));
}

LayoutRef AbsoluteValueNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return AbsoluteValueLayoutRef(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits));
}

Expression AbsoluteValueNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  return AbsoluteValue(this).shallowReduce(context, angleUnit);
}

Expression AbsoluteValue::setSign(ExpressionNode::Sign s, Context & context, Preferences::AngleUnit angleUnit) {
  assert(s == ExpressionNode::Sign::Positive);
  return *this;
}

Expression AbsoluteValue::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  Expression e = Expression::defaultShallowReduce(context, angleUnit);
  if (e.isUndefinedOrAllocationFailure()) {
    return e;
  }
  Expression op = childAtIndex(0);
#if MATRIX_EXACT_REDUCING
#if 0
  if (op->type() == Type::Matrix) {
    return SimplificationHelper::Map(this, context, angleUnit);
  }
#endif
#endif
  if (op.sign() == ExpressionNode::Sign::Positive) {
    return op;
  }
  if (op.sign() == ExpressionNode::Sign::Negative) {
    return op.setSign(ExpressionNode::Sign::Positive, context, angleUnit);
  }
  return *this;
}


}
