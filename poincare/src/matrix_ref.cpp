#include <poincare/matrix_ref.h>
#include <poincare/matrix_complex.h>
#include <poincare/layout_helper.h>
#include <poincare/matrix.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

constexpr Expression::FunctionHelper MatrixRef::s_functionHelper;

int MatrixRefNode::numberOfChildren() const { return MatrixRef::s_functionHelper.numberOfChildren(); }

Expression MatrixRefNode::shallowReduce(ReductionContext reductionContext) {
  return MatrixRef(this).shallowReduce(reductionContext);
}

Layout MatrixRefNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(MatrixRef(this), floatDisplayMode, numberOfSignificantDigits, MatrixRef::s_functionHelper.name());
}

int MatrixRefNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, MatrixRef::s_functionHelper.name());
}

template<typename T>
Evaluation<T> MatrixRefNode::templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> input = childAtIndex(0)->approximate(T(), context, complexFormat, angleUnit);
  Evaluation<T> ref;
  if (input.type() == EvaluationNode<T>::Type::MatrixComplex) {
    ref = static_cast<MatrixComplex<T>&>(input).ref(false);
  } else {
    ref = Complex<T>::Undefined();
  }
  assert(!ref.isUninitialized());
  return ref;
}


Expression MatrixRef::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = Expression::defaultShallowReduce();
    e = e.defaultHandleUnitsInChildren();
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression c = childAtIndex(0);
  if (c.type() == ExpressionNode::Type::Matrix) {
    bool couldComputeRef = false;
    Expression result = static_cast<Matrix&>(c).createRef(reductionContext, &couldComputeRef, false);
    if (couldComputeRef) {
      replaceWithInPlace(result);
      return result;
    }
    // The matrix could not be transformed properly
    return *this;
  }
  return replaceWithUndefinedInPlace();
}

}
