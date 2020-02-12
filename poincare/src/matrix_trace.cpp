#include <poincare/matrix_trace.h>
#include <poincare/addition.h>
#include <poincare/layout_helper.h>
#include <poincare/matrix.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/undefined.h>
#include <cmath>
#include <utility>

namespace Poincare {

constexpr Expression::FunctionHelper MatrixTrace::s_functionHelper;

int MatrixTraceNode::numberOfChildren() const { return MatrixTrace::s_functionHelper.numberOfChildren(); }

Expression MatrixTraceNode::shallowReduce(ReductionContext reductionContext) {
  return MatrixTrace(this).shallowReduce(reductionContext);
}

Layout MatrixTraceNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(MatrixTrace(this), floatDisplayMode, numberOfSignificantDigits, MatrixTrace::s_functionHelper.name());
}

int MatrixTraceNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, MatrixTrace::s_functionHelper.name());
}

template<typename T>
Evaluation<T> MatrixTraceNode::templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> input = childAtIndex(0)->approximate(T(), context, complexFormat, angleUnit);
  Complex<T> result = Complex<T>::Builder(input.trace());
  return std::move(result);
}


Expression MatrixTrace::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = Expression::defaultShallowReduce();
    e = e.defaultHandleUnitsInChildren();
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression c = childAtIndex(0);
  if (c.type() == ExpressionNode::Type::Matrix) {
    Matrix matrixChild0 = static_cast<Matrix&>(c);
    if (matrixChild0.numberOfRows() != matrixChild0.numberOfColumns()) {
      return replaceWithUndefinedInPlace();
    }
    int n = matrixChild0.numberOfRows();
    Addition a = Addition::Builder();
    for (int i = 0; i < n; i++) {
      a.addChildAtIndexInPlace(matrixChild0.matrixChild(i,i), i, i); // No need to clone
    }
    replaceWithInPlace(a);
    return a.shallowReduce(reductionContext);
  }
  if (c.deepIsMatrix(reductionContext.context())) {
    return *this;
  }
  replaceWithInPlace(c);
  return c;
}

}
