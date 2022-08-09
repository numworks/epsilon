#include <poincare/vector_dot.h>
#include <poincare/addition.h>
#include <poincare/layout_helper.h>
#include <poincare/matrix.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/undefined.h>

namespace Poincare {

int VectorDotNode::numberOfChildren() const { return VectorDot::s_functionHelper.numberOfChildren(); }

Expression VectorDotNode::shallowReduce(const ReductionContext& reductionContext) {
  return VectorDot(this).shallowReduce(reductionContext);
}

Layout VectorDotNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  return LayoutHelper::Prefix(VectorDot(this), floatDisplayMode, numberOfSignificantDigits, VectorDot::s_functionHelper.aliasesList().mainAlias(), context);
}

int VectorDotNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, VectorDot::s_functionHelper.aliasesList().mainAlias());
}

template<typename T>
Evaluation<T> VectorDotNode::templatedApproximate(const ApproximationContext& approximationContext) const {
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
  return Complex<T>::Builder(static_cast<MatrixComplex<T>&>(input0).dot(static_cast<MatrixComplex<T> *>(&input1)));
}


Expression VectorDot::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(
        *this,
        &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::BanUnits
    );
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
    // Dot product is defined between two vectors of the same dimension and type
    if (matrixChild0.vectorType() == Array::VectorType::None || matrixChild0.vectorType() != matrixChild1.vectorType() || matrixChild0.numberOfChildren() != matrixChild1.numberOfChildren()) {
      return replaceWithUndefinedInPlace();
    }
    Expression a = matrixChild0.dot(&matrixChild1, reductionContext);
    replaceWithInPlace(a);
    return a.shallowReduce(reductionContext);
  }
  if (c0.deepIsMatrix(reductionContext.context(), reductionContext.shouldCheckMatrices()) && c1.deepIsMatrix(reductionContext.context(), reductionContext.shouldCheckMatrices())) {
    return *this;
  }
  return replaceWithUndefinedInPlace();
}

}
