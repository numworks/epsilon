#include <poincare/student_distribution_function.h>
#include <poincare/student_distribution.h>
#include <poincare/square_root.h>
#include <poincare/simplification_helper.h>
#include <assert.h>

namespace Poincare {

Expression StudentDistributionFunctionNode::shallowReduce(ReductionContext reductionContext) {
  return StudentDistributionFunction(this).shallowReduce(reductionContext.context());
}

Expression StudentDistributionFunction::shallowReduce(Context * context, bool * stopReduction) {
  if (stopReduction != nullptr) {
    *stopReduction = true;
  }
  {
    Expression e = SimplificationHelper::defaultShallowReduce(*this);
    if (!e.isUninitialized()) {
      return e;
    }
  }

  // Check k
  bool kIsOK = false;
  bool couldCheckK = StudentDistribution::ExpressionKIsOK(
      &kIsOK,
      childAtIndex(kIndex()),
      context);
  if (!couldCheckK) {
    return *this;
  }
  if (!kIsOK) {
    return replaceWithUndefinedInPlace();
  }

  if (stopReduction != nullptr) {
    *stopReduction = false;
  }
  return *this;
}

}
