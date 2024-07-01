#include <poincare/layout.h>
#include <poincare/old/addition.h>
#include <poincare/old/matrix.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/undefined.h>
#include <poincare/old/vector_norm.h>

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
