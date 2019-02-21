#include <poincare/determinant.h>
#include <poincare/matrix.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

constexpr Expression::FunctionHelper Determinant::s_functionHelper;

int DeterminantNode::numberOfChildren() const { return Determinant::s_functionHelper.numberOfChildren(); }

Layout DeterminantNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(Determinant(this), floatDisplayMode, numberOfSignificantDigits, Determinant::s_functionHelper.name());
}

int DeterminantNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Determinant::s_functionHelper.name());
}

// TODO: handle this exactly in shallowReduce for small dimensions.
template<typename T>
Evaluation<T> DeterminantNode::templatedApproximate(Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> input = childAtIndex(0)->approximate(T(), context, complexFormat, angleUnit);
  return Complex<T>::Builder(input.determinant());
}

Expression DeterminantNode::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) {
  return Determinant(this).shallowReduce(context);
}


Expression Determinant::shallowReduce(Context & context) {
  {
    Expression e = Expression::defaultShallowReduce();
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
  if (!c0.recursivelyMatches(Expression::IsMatrix, context, true)) {
    replaceWithInPlace(c0);
    return c0;
  }
  return *this;
}

}
