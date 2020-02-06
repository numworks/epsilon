#include <poincare/determinant.h>
#include <poincare/addition.h>
#include <poincare/matrix.h>
#include <poincare/multiplication.h>
#include <poincare/layout_helper.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/subtraction.h>
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

template<typename T>
Evaluation<T> DeterminantNode::templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> input = childAtIndex(0)->approximate(T(), context, complexFormat, angleUnit);
  return Complex<T>::Builder(input.determinant());
}

Expression DeterminantNode::shallowReduce(ReductionContext reductionContext) {
  return Determinant(this).shallowReduce(reductionContext);
}

Expression Determinant::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = Expression::defaultShallowReduce();
    e = e.defaultHandleUnitsInChildren();
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression c0 = childAtIndex(0);
  // det(A) = A if A is not a matrix
  if (!c0.deepIsMatrix(reductionContext.context())) {
    replaceWithInPlace(c0);
    return c0;
  }
  if (c0.type() == ExpressionNode::Type::Matrix) {
    Matrix m0 = static_cast<Matrix &>(c0);
    bool couldComputeDeterminant = true;
    Expression result = m0.determinant(reductionContext, &couldComputeDeterminant, true);
    if (couldComputeDeterminant) {
      assert(!result.isUninitialized());
      replaceWithInPlace(result);
      return result.shallowReduce(reductionContext);
    }
  }
  return *this;
}

}
