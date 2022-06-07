#include <poincare/vector_cross.h>
#include <poincare/division.h>
#include <poincare/layout_helper.h>
#include <poincare/matrix.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/undefined.h>

namespace Poincare {

int VectorCrossNode::numberOfChildren() const { return VectorCross::s_functionHelper.numberOfChildren(); }

Expression VectorCrossNode::shallowReduce(ReductionContext reductionContext) {
  return VectorCross(this).shallowReduce(reductionContext);
}

Layout VectorCrossNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(VectorCross(this), floatDisplayMode, numberOfSignificantDigits, VectorCross::s_functionHelper.name());
}

int VectorCrossNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, VectorCross::s_functionHelper.name());
}

template<typename T>
Evaluation<T> VectorCrossNode::templatedApproximate(ApproximationContext approximationContext) const {
  if (Poincare::Preferences::sharedPreferences()->vectorProductsAreForbidden()) {
    return Complex<T>::Undefined();
  }
  Evaluation<T> input0 = childAtIndex(0)->approximate(T(), approximationContext);
  if (input0.type() != EvaluationNode<T>::Type::MatrixComplex) {
    return Complex<T>::Undefined();
  }
  Evaluation<T> input1 = childAtIndex(1)->approximate(T(), approximationContext);
  if (input1.type() != EvaluationNode<T>::Type::MatrixComplex) {
    return Complex<T>::Undefined();
  }
  return static_cast<MatrixComplex<T>&>(input0).cross(static_cast<MatrixComplex<T> *>(&input1));
}

Expression VectorCross::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(*this);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  if (Poincare::Preferences::sharedPreferences()->vectorProductsAreForbidden()) {
    return replaceWithUndefinedInPlace();
  }
  Expression c0 = childAtIndex(0);
  Expression c1 = childAtIndex(1);
  if (c0.type() == ExpressionNode::Type::Matrix && c1.type() == ExpressionNode::Type::Matrix) {
    Matrix matrixChild0 = static_cast<Matrix&>(c0);
    Matrix matrixChild1 = static_cast<Matrix&>(c1);
    // Cross product is defined between two vectors of same type and of size 3
    if (matrixChild0.vectorType() == Array::VectorType::None || matrixChild0.vectorType() != matrixChild1.vectorType() || matrixChild0.numberOfChildren() != 3 || matrixChild1.numberOfChildren() != 3) {
      return replaceWithUndefinedInPlace();
    }
    Expression a = matrixChild0.cross(&matrixChild1, reductionContext);
    replaceWithInPlace(a);
    return a.shallowReduce(reductionContext);
  }
  if (c0.deepIsMatrix(reductionContext.context()) && c1.deepIsMatrix(reductionContext.context())) {
    return *this;
  }
  return replaceWithUndefinedInPlace();
}

}
