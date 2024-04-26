#include <poincare/old/addition.h>
#include <poincare/old/layout.h>
#include <poincare/old/matrix.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/undefined.h>
#include <poincare/old/vector_dot.h>

namespace Poincare {

int VectorDotNode::numberOfChildren() const {
  return VectorDot::s_functionHelper.numberOfChildren();
}

OExpression VectorDotNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return VectorDot(this).shallowReduce(reductionContext);
}

size_t VectorDotNode::serialize(char* buffer, size_t bufferSize,
                                Preferences::PrintFloatMode floatDisplayMode,
                                int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      VectorDot::s_functionHelper.aliasesList().mainAlias());
}

template <typename T>
Evaluation<T> VectorDotNode::templatedApproximate(
    const ApproximationContext& approximationContext) const {
  if (Poincare::Preferences::SharedPreferences()
          ->examMode()
          .forbidVectorProduct()) {
    return Complex<T>::Undefined();
  }
  Evaluation<T> input0 =
      childAtIndex(0)->approximate(T(), approximationContext);
  if (input0.otype() != EvaluationNode<T>::Type::MatrixComplex) {
    return Complex<T>::Undefined();
  }
  Evaluation<T> input1 =
      childAtIndex(1)->approximate(T(), approximationContext);
  if (input1.otype() != EvaluationNode<T>::Type::MatrixComplex) {
    return Complex<T>::Undefined();
  }
  return Complex<T>::Builder(static_cast<MatrixComplex<T>&>(input0).dot(
      static_cast<MatrixComplex<T>*>(&input1)));
}

OExpression VectorDot::shallowReduce(ReductionContext reductionContext) {
  {
    OExpression e = SimplificationHelper::defaultShallowReduce(
        *this, &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::BanUnits);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  if (Poincare::Preferences::SharedPreferences()
          ->examMode()
          .forbidVectorProduct()) {
    return replaceWithUndefinedInPlace();
  }
  OExpression c0 = childAtIndex(0);
  OExpression c1 = childAtIndex(1);
  if (c0.otype() == ExpressionNode::Type::OMatrix &&
      c1.otype() == ExpressionNode::Type::OMatrix) {
    OMatrix matrixChild0 = static_cast<OMatrix&>(c0);
    OMatrix matrixChild1 = static_cast<OMatrix&>(c1);
    // Dot product is defined between two vectors of the same dimension and type
    if (!matrixChild0.isVector() ||
        matrixChild0.vectorType() != matrixChild1.vectorType() ||
        matrixChild0.numberOfChildren() != matrixChild1.numberOfChildren()) {
      return replaceWithUndefinedInPlace();
    }
    OExpression a = matrixChild0.dot(&matrixChild1, reductionContext);
    replaceWithInPlace(a);
    return a.shallowReduce(reductionContext);
  }
  if (c0.deepIsMatrix(reductionContext.context(),
                      reductionContext.shouldCheckMatrices()) &&
      c1.deepIsMatrix(reductionContext.context(),
                      reductionContext.shouldCheckMatrices())) {
    return *this;
  }
  return replaceWithUndefinedInPlace();
}

}  // namespace Poincare
