#include <poincare/layout.h>
#include <poincare/old/division.h>
#include <poincare/old/matrix.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/undefined.h>
#include <poincare/old/vector_cross.h>

namespace Poincare {

int VectorCrossNode::numberOfChildren() const {
  return VectorCross::s_functionHelper.numberOfChildren();
}

OExpression VectorCrossNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return VectorCross(this).shallowReduce(reductionContext);
}

size_t VectorCrossNode::serialize(char* buffer, size_t bufferSize,
                                  Preferences::PrintFloatMode floatDisplayMode,
                                  int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      VectorCross::s_functionHelper.aliasesList().mainAlias());
}

OExpression VectorCross::shallowReduce(ReductionContext reductionContext) {
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
    // Cross product is defined between two vectors of same type and of size 3
    if (!matrixChild0.isVector() ||
        matrixChild0.vectorType() != matrixChild1.vectorType() ||
        matrixChild0.numberOfChildren() != 3 ||
        matrixChild1.numberOfChildren() != 3) {
      return replaceWithUndefinedInPlace();
    }
    OExpression a = matrixChild0.cross(&matrixChild1, reductionContext);
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
