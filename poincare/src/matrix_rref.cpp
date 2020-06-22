#include <poincare/matrix_rref.h>
#include <poincare/matrix_complex.h>
#include <poincare/layout_helper.h>
#include <poincare/matrix.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

constexpr Expression::FunctionHelper MatrixRref::s_functionHelper;

int MatrixRrefNode::numberOfChildren() const { return MatrixRref::s_functionHelper.numberOfChildren(); }

Expression MatrixRrefNode::shallowReduce(ReductionContext reductionContext) {
  return MatrixRref(this).shallowReduce(reductionContext);
}

Layout MatrixRrefNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(MatrixRref(this), floatDisplayMode, numberOfSignificantDigits, MatrixRref::s_functionHelper.name());
}

int MatrixRrefNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, MatrixRref::s_functionHelper.name());
}

template<typename T>
Evaluation<T> MatrixRrefNode::templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> input = childAtIndex(0)->approximate(T(), context, complexFormat, angleUnit);
  Evaluation<T> rref;
  if (input.type() == EvaluationNode<T>::Type::MatrixComplex) {
    rref = static_cast<MatrixComplex<T>&>(input).ref(true);
  } else {
    rref = Complex<T>::Undefined();
  }
  assert(!rref.isUninitialized());
  return rref;
}


Expression MatrixRref::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = Expression::defaultShallowReduce();
    e = e.defaultHandleUnitsInChildren();
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression c = childAtIndex(0);
  if (c.type() == ExpressionNode::Type::Matrix) {
    bool couldComputeRref = false;
    Expression result = static_cast<Matrix&>(c).createRef(reductionContext, &couldComputeRref, true);
    if (couldComputeRref) {
      replaceWithInPlace(result);
      return result;
    }
    // The matrix could not be transformed properly
    return *this;
  }
  return replaceWithUndefinedInPlace();
}

}
