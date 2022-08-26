#include <poincare/matrix_trace.h>
#include <poincare/addition.h>
#include <poincare/layout_helper.h>
#include <poincare/matrix.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/undefined.h>
#include <cmath>
#include <utility>

namespace Poincare {

int MatrixTraceNode::numberOfChildren() const { return MatrixTrace::s_functionHelper.numberOfChildren(); }

Expression MatrixTraceNode::shallowReduce(const ReductionContext& reductionContext) {
  return MatrixTrace(this).shallowReduce(reductionContext);
}

Layout MatrixTraceNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  return LayoutHelper::Prefix(MatrixTrace(this), floatDisplayMode, numberOfSignificantDigits, MatrixTrace::s_functionHelper.aliasesList().mainAlias(), context);
}

int MatrixTraceNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, MatrixTrace::s_functionHelper.aliasesList().mainAlias());
}

template<typename T>
Evaluation<T> MatrixTraceNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  Evaluation<T> input = childAtIndex(0)->approximate(T(), approximationContext);
  if (input.type() != EvaluationNode<T>::Type::MatrixComplex) {
    return input;
  }
  Complex<T> result = Complex<T>::Builder(static_cast<MatrixComplex<T>&>(input).trace());
  return std::move(result);
}

Expression MatrixTrace::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(
        *this,
        &reductionContext,
        SimplificationHelper::UnitReduction::BanUnits
    );
    if (!e.isUninitialized()) {
      return e;
    }
  }
  Expression c = childAtIndex(0);
  if (c.type() == ExpressionNode::Type::Matrix) {
    Matrix matrixChild0 = static_cast<Matrix&>(c);
    if (matrixChild0.numberOfRows() != matrixChild0.numberOfColumns()) {
      return replaceWithUndefinedInPlace();
    }
    Expression a = matrixChild0.createTrace();
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
