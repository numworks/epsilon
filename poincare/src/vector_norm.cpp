#include <poincare/addition.h>
#include <poincare/layout.h>
#include <poincare/matrix.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/undefined.h>
#include <poincare/vector_norm.h>

namespace Poincare {

int VectorNormNode::numberOfChildren() const {
  return VectorNorm::s_functionHelper.numberOfChildren();
}

OExpression VectorNormNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return VectorNorm(this).shallowReduce(reductionContext);
}

size_t VectorNormNode::serialize(char* buffer, size_t bufferSize,
                                 Preferences::PrintFloatMode floatDisplayMode,
                                 int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      VectorNorm::s_functionHelper.aliasesList().mainAlias());
}

template <typename T>
Evaluation<T> VectorNormNode::templatedApproximate(
    const ApproximationContext& approximationContext) const {
  if (Poincare::Preferences::SharedPreferences()
          ->examMode()
          .forbidVectorNorm()) {
    return Complex<T>::Undefined();
  }
  Evaluation<T> input = childAtIndex(0)->approximate(T(), approximationContext);
  if (input.otype() != EvaluationNode<T>::Type::MatrixComplex) {
    return Complex<T>::Undefined();
  }
  return Complex<T>::Builder(static_cast<MatrixComplex<T>&>(input).norm());
}

OExpression VectorNorm::shallowReduce(ReductionContext reductionContext) {
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
          .forbidVectorNorm()) {
    return replaceWithUndefinedInPlace();
  }
  OExpression c = childAtIndex(0);
  if (c.otype() == ExpressionNode::Type::OMatrix) {
    OMatrix matrixChild = static_cast<OMatrix&>(c);
    // Norm is only defined on vectors only
    if (!matrixChild.isVector()) {
      return replaceWithUndefinedInPlace();
    }
    OExpression a = matrixChild.norm(reductionContext);
    replaceWithInPlace(a);
    return a.shallowReduce(reductionContext);
  }
  if (c.deepIsMatrix(reductionContext.context(),
                     reductionContext.shouldCheckMatrices())) {
    return *this;
  }
  return replaceWithUndefinedInPlace();
}

}  // namespace Poincare
