#include <poincare/layout.h>
#include <poincare/old/addition.h>
#include <poincare/old/determinant.h>
#include <poincare/old/matrix.h>
#include <poincare/old/multiplication.h>
#include <poincare/old/rational.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/subtraction.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

int DeterminantNode::numberOfChildren() const {
  return Determinant::s_functionHelper.numberOfChildren();
}

size_t DeterminantNode::serialize(char* buffer, size_t bufferSize,
                                  Preferences::PrintFloatMode floatDisplayMode,
                                  int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      Determinant::s_functionHelper.aliasesList().mainAlias());
}

OExpression DeterminantNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return Determinant(this).shallowReduce(reductionContext);
}

OExpression Determinant::shallowReduce(ReductionContext reductionContext) {
  {
    OExpression e = SimplificationHelper::defaultShallowReduce(
        *this, &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::BanUnits);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  OExpression c0 = childAtIndex(0);
  // det(A) = undef if A is not a matrix
  if (!c0.deepIsMatrix(reductionContext.context(),
                       reductionContext.shouldCheckMatrices())) {
    return replaceWithUndefinedInPlace();
  }
  if (c0.otype() == ExpressionNode::Type::OMatrix) {
    OMatrix m0 = static_cast<OMatrix&>(c0);
    bool couldComputeDeterminant = true;
    OExpression result =
        m0.determinant(reductionContext, &couldComputeDeterminant, true);
    if (couldComputeDeterminant) {
      assert(!result.isUninitialized());
      replaceWithInPlace(result);
      return result.shallowReduce(reductionContext);
    } else if (reductionContext.target() ==
               Poincare::ReductionTarget::SystemForApproximation) {
      /* ReductionTarget::SystemForApproximation requires an exact value, which
       * couldn't be computed by OMatrix::determinant due to the lacked of
       * computational resources. */

      return replaceWithUndefinedInPlace();
    }
  }
  return *this;
}

}  // namespace Poincare
