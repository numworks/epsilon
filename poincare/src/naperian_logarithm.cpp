#include <poincare/naperian_logarithm.h>
#include <poincare/symbol.h>
#include <poincare/logarithm.h>
#include <poincare/simplification_helper.h>

namespace Poincare {

NaperianLogarithmNode * NaperianLogarithmNode::FailedAllocationStaticNode() {
  static AllocationFailureExpressionNode<NaperianLogarithmNode> failure;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failure);
  return &failure;
}

Expression NaperianLogarithmNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  return NaperianLogarithm(this).shallowReduce(context, angleUnit);
}

Expression NaperianLogarithm::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  {
    Expression e = Expression::defaultShallowReduce(context, angleUnit);
    if (e.isUndefinedOrAllocationFailure()) {
      return e;
    }
  }
#if MATRIX_EXACT_REDUCING
  if (childAtIndex(0).type() == ExpressionNode::Type::Matrix) {
    return SimplificationHelper::Map(*this, context, angleUnit);
  }
#endif
  Logarithm l = Logarithm(childAtIndex(0), Symbol(Ion::Charset::Exponential));
  Expression result = l.shallowReduce(context, angleUnit);
  replaceWithInPlace(result);
  return result;
}

}
