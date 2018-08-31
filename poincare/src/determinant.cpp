#include <poincare/determinant.h>
#include <poincare/matrix.h>
#include <poincare/layout_helper.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

DeterminantNode * DeterminantNode::FailedAllocationStaticNode() {
  static AllocationFailureExpressionNode<DeterminantNode> failure;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failure);
  return &failure;
}

LayoutRef DeterminantNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(Determinant(this), floatDisplayMode, numberOfSignificantDigits, name());
}

// TODO: handle this exactly in shallowReduce for small dimensions.
template<typename T>
Evaluation<T> DeterminantNode::templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> input = childAtIndex(0)->approximate(T(), context, angleUnit);
  return Complex<T>(input.determinant());
}

Expression DeterminantNode::shallowReduce(Context& context, Preferences::AngleUnit angleUnit, const Expression futureParent) {
  return Determinant(this).shallowReduce(context, angleUnit);
}

Expression Determinant::shallowReduce(Context& context, Preferences::AngleUnit angleUnit, const Expression futureParent) {
  Expression e = Expression::defaultShallowReduce(context, angleUnit);
  if (e.isUndefinedOrAllocationFailure()) {
    return e;
  }
  Expression op = childAtIndex(0);
#if MATRIX_EXACT_REDUCING
#if 0
  if (!op.recursivelyMatches(Expression::IsMatrix)) {
    return replaceWith(op, true);
  }
  return this;
#endif
#endif
  // det(A) = A if A is not a matrix
  if (!op.recursivelyMatches(Expression::IsMatrix, context)) {
    return op;
  }
  return *this;
}

}

