#include <poincare/vector_dot.h>
#include <poincare/addition.h>
#include <poincare/layout_helper.h>
#include <poincare/matrix.h>
#include <poincare/serialization_helper.h>
#include <poincare/undefined.h>

namespace Poincare {

constexpr Expression::FunctionHelper VectorDot::s_functionHelper;

int VectorDotNode::numberOfChildren() const { return VectorDot::s_functionHelper.numberOfChildren(); }

Expression VectorDotNode::shallowReduce(ReductionContext reductionContext) {
  return VectorDot(this).shallowReduce(reductionContext);
}

Layout VectorDotNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(VectorDot(this), floatDisplayMode, numberOfSignificantDigits, VectorDot::s_functionHelper.name());
}

int VectorDotNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, VectorDot::s_functionHelper.name());
}

template<typename T>
Evaluation<T> VectorDotNode::templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> input0 = childAtIndex(0)->approximate(T(), context, complexFormat, angleUnit);
  Evaluation<T> input1 = childAtIndex(1)->approximate(T(), context, complexFormat, angleUnit);
  return Complex<T>::Builder(input0.dot(&input1));
}


Expression VectorDot::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = Expression::defaultShallowReduce();
    e = e.defaultHandleUnitsInChildren();
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression c0 = childAtIndex(0);
  Expression c1 = childAtIndex(1);
  if (c0.type() == ExpressionNode::Type::Matrix && c1.type() == ExpressionNode::Type::Matrix) {
    Matrix matrixChild0 = static_cast<Matrix&>(c0);
    Matrix matrixChild1 = static_cast<Matrix&>(c1);
    // Dot product is defined between two column matrices of the same dimensions
    if (matrixChild0.numberOfColumns() != 1 || matrixChild1.numberOfColumns() != 1 || matrixChild0.numberOfRows() != matrixChild1.numberOfRows()) {
      return replaceWithUndefinedInPlace();
    }
    Expression a = matrixChild0.dot(&matrixChild1, reductionContext);
    replaceWithInPlace(a);
    return a.shallowReduce(reductionContext);
  }
  if (c0.deepIsMatrix(reductionContext.context()) && c1.deepIsMatrix(reductionContext.context())) {
    return *this;
  }
  return replaceWithUndefinedInPlace();
}

}
