#include <assert.h>
#include <poincare/layout.h>
#include <poincare/old/addition.h>
#include <poincare/old/approximation_helper.h>
#include <poincare/old/constant.h>
#include <poincare/old/division.h>
#include <poincare/old/naperian_logarithm.h>
#include <poincare/old/nth_root.h>
#include <poincare/old/power.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/subtraction.h>
#include <poincare/old/undefined.h>

#include <cmath>
#include <utility>

namespace Poincare {

int NthRootNode::numberOfChildren() const {
  return NthRoot::s_functionHelper.numberOfChildren();
}

size_t NthRootNode::serialize(char* buffer, size_t bufferSize,
                              Preferences::PrintFloatMode floatDisplayMode,
                              int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      NthRoot::s_functionHelper.aliasesList().mainAlias());
}

OExpression NthRootNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return NthRoot(this).shallowReduce(reductionContext);
}

OExpression NthRoot::shallowReduce(ReductionContext reductionContext) {
  {
    OExpression e = SimplificationHelper::defaultShallowReduce(
        *this, &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::KeepUnits,
        SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
        SimplificationHelper::ListReduction::DistributeOverLists);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  OExpression invIndex = Power::Builder(childAtIndex(1), Rational::Builder(-1));
  Power p = Power::Builder(childAtIndex(0), invIndex);
  invIndex.shallowReduce(reductionContext);
  replaceWithInPlace(p);
  return p.shallowReduce(reductionContext);
}

}  // namespace Poincare
