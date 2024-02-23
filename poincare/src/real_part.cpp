#include <assert.h>
#include <poincare/complex_cartesian.h>
#include <poincare/layout_helper.h>
#include <poincare/real_part.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>

#include <cmath>

namespace Poincare {

int RealPartNode::numberOfChildren() const {
  return RealPart::s_functionHelper.numberOfChildren();
}

size_t RealPartNode::serialize(char* buffer, size_t bufferSize,
                               Preferences::PrintFloatMode floatDisplayMode,
                               int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      RealPart::s_functionHelper.aliasesList().mainAlias());
}

OExpression RealPartNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return RealPart(this).shallowReduce(reductionContext);
}

OExpression RealPart::shallowReduce(ReductionContext reductionContext) {
  {
    OExpression e = SimplificationHelper::defaultShallowReduce(
        *this, &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::BanUnits,
        SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
        SimplificationHelper::ListReduction::DistributeOverLists);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  OExpression c = childAtIndex(0);
  if (c.isReal(reductionContext.context(),
               reductionContext.shouldCheckMatrices())) {
    replaceWithInPlace(c);
    return c;
  }
  if (c.type() == ExpressionNode::Type::ComplexCartesian) {
    ComplexCartesian complexChild = static_cast<ComplexCartesian&>(c);
    OExpression r = complexChild.real();
    replaceWithInPlace(r);
    return r.shallowReduce(reductionContext);
  }
  return *this;
}

}  // namespace Poincare
