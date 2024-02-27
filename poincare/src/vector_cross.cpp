#include <poincare/division.h>
#include <poincare/layout_helper.h>
#include <poincare/matrix.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/undefined.h>
#include <poincare/vector_cross.h>

namespace Poincare {

int VectorCrossNode::numberOfChildren() const {
  return VectorCross::s_functionHelper.numberOfChildren();
}

Expression VectorCrossNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return VectorCross(this).shallowReduce(reductionContext);
}

OLayout VectorCrossNode::createLayout(
    Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits,
    Context* context) const {
  return LayoutHelper::Prefix(
      VectorCross(this), floatDisplayMode, numberOfSignificantDigits,
      VectorCross::s_functionHelper.aliasesList().mainAlias(), context);
}

size_t VectorCrossNode::serialize(char* buffer, size_t bufferSize,
                                  Preferences::PrintFloatMode floatDisplayMode,
                                  int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      VectorCross::s_functionHelper.aliasesList().mainAlias());
}

template <typename T>
Evaluation<T> VectorCrossNode::templatedApproximate(
    const ApproximationContext& approximationContext) const {
  if (Poincare::Preferences::SharedPreferences()
          ->examMode()
          .forbidVectorProduct()) {
    return Complex<T>::Undefined();
  }
  Evaluation<T> input0 =
      childAtIndex(0)->approximate(T(), approximationContext);
  if (input0.type() != EvaluationNode<T>::Type::MatrixComplex) {
    return Complex<T>::Undefined();
  }
  Evaluation<T> input1 =
      childAtIndex(1)->approximate(T(), approximationContext);
  if (input1.type() != EvaluationNode<T>::Type::MatrixComplex) {
    return Complex<T>::Undefined();
  }
  return static_cast<MatrixComplex<T>&>(input0).cross(
      static_cast<MatrixComplex<T>*>(&input1));
}

Expression VectorCross::shallowReduce(ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(
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
  Expression c0 = childAtIndex(0);
  Expression c1 = childAtIndex(1);
  if (c0.type() == ExpressionNode::Type::Matrix &&
      c1.type() == ExpressionNode::Type::Matrix) {
    Matrix matrixChild0 = static_cast<Matrix&>(c0);
    Matrix matrixChild1 = static_cast<Matrix&>(c1);
    // Cross product is defined between two vectors of same type and of size 3
    if (!matrixChild0.isVector() ||
        matrixChild0.vectorType() != matrixChild1.vectorType() ||
        matrixChild0.numberOfChildren() != 3 ||
        matrixChild1.numberOfChildren() != 3) {
      return replaceWithUndefinedInPlace();
    }
    Expression a = matrixChild0.cross(&matrixChild1, reductionContext);
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
