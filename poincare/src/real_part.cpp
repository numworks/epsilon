#include <poincare/real_part.h>
#include <poincare/simplification_helper.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

RealPartNode * RealPartNode::FailedAllocationStaticNode() {
  static AllocationFailureExpressionNode<RealPartNode> failure;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failure);
  return &failure;
}

LayoutReference RealPartNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(RealPart(this), floatDisplayMode, numberOfSignificantDigits, name());
}

template<typename T>
Complex<T> RealPartNode::computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  return Complex<T>(std::real(c));
}

Expression RealPartNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  return RealPart(this).shallowReduce(context, angleUnit, futureParent);
}

Expression RealPart::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  Expression e = Expression::defaultShallowReduce(context, angleUnit);
  if (e.isUndefinedOrAllocationFailure()) {
    return e;
  }
  Expression op = childAtIndex(0);
#if MATRIX_EXACT_REDUCING
  if (op.type() == ExpressionNode::Type::Matrix) {
    return SimplificationHelper::Map(*this, context, angleUnit);
  }
#endif
  if (op.type() == ExpressionNode::Type::Rational) {
    return op;
  }
  return *this;
}

}
