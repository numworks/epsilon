#include <poincare/old/list_standard_deviation.h>
#include <poincare/old/list_variance.h>
#include <poincare/old/power.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/square_root.h>

namespace Poincare {

OExpression ListStandardDeviationNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return ListStandardDeviation(this).shallowReduce(reductionContext);
}

OExpression ListStandardDeviation::shallowReduce(
    ReductionContext reductionContext) {
  assert(numberOfChildren() == 1 || numberOfChildren() == 2);
  OExpression children[2];
  if (!static_cast<ListFunctionWithOneOrTwoParametersNode*>(node())
           ->getChildrenIfNonEmptyList(children)) {
    return replaceWithUndefinedInPlace();
  }
  ListVariance var = ListVariance::Builder(children[0], children[1]);
  Power sqrt = Power::Builder(var, Rational::Builder(1, 2));
  var.shallowReduce(reductionContext);
  replaceWithInPlace(sqrt);
  return sqrt.shallowReduce(reductionContext);
}

}  // namespace Poincare
