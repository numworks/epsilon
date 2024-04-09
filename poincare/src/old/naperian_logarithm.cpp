#include <poincare/layout.h>
#include <poincare/old/constant.h>
#include <poincare/old/logarithm.h>
#include <poincare/old/naperian_logarithm.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>

namespace Poincare {

int NaperianLogarithmNode::numberOfChildren() const {
  return NaperianLogarithm::s_functionHelper.numberOfChildren();
}

size_t NaperianLogarithmNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      NaperianLogarithm::s_functionHelper.aliasesList().mainAlias());
}

OExpression NaperianLogarithmNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return NaperianLogarithm(this).shallowReduce(reductionContext);
}

OExpression NaperianLogarithm::shallowReduce(
    ReductionContext reductionContext) {
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
  Logarithm l =
      Logarithm::Builder(childAtIndex(0), Constant::ExponentialEBuilder());
  replaceWithInPlace(l);
  return l.shallowReduce(reductionContext);
}

}  // namespace Poincare
