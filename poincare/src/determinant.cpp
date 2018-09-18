#include <poincare/determinant.h>
#include <poincare/matrix.h>
#include <poincare/layout_helper.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Layout DeterminantNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(Determinant(this), floatDisplayMode, numberOfSignificantDigits, name());
}

// TODO: handle this exactly in shallowReduce for small dimensions.
template<typename T>
Evaluation<T> DeterminantNode::templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> input = childAtIndex(0)->approximate(T(), context, angleUnit);
  return Complex<T>(input.determinant());
}

Expression DeterminantNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  return Determinant(this).shallowReduce(context, angleUnit);
}

Determinant::Determinant() : Expression(TreePool::sharedPool()->createTreeNode<DeterminantNode>()) {}

Expression Determinant::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  {
    Expression e = Expression::defaultShallowReduce(context, angleUnit);
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression c0 = childAtIndex(0);
#if MATRIX_EXACT_REDUCING
#if 0
  if (!op.recursivelyMatches(Expression::IsMatrix)) {
    return replaceWith(op, true);
  }
  return this;
#endif
#endif
  // det(A) = A if A is not a matrix
  if (!c0.recursivelyMatches(Expression::IsMatrix, context)) {
    replaceWithInPlace(c0);
    return c0;
  }
  return *this;
}

}
