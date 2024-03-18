#include <poincare/constant.h>
#include <poincare/layout_helper.h>
#include <poincare/logarithm.h>
#include <poincare/naperian_logarithm.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>

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

Expression NaperianLogarithmNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return NaperianLogarithm(this).shallowReduce(reductionContext);
}

Expression NaperianLogarithm::shallowReduce(ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(
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
