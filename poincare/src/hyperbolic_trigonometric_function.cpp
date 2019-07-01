#include <poincare/hyperbolic_trigonometric_function.h>
#include <poincare/simplification_helper.h>

namespace Poincare {

Expression HyperbolicTrigonometricFunctionNode::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target, bool symbolicComputation) {
  return HyperbolicTrigonometricFunction(this).shallowReduce(context, angleUnit);
}

Expression HyperbolicTrigonometricFunction::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  {
    Expression e = Expression::defaultShallowReduce();
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression c = childAtIndex(0);
  if (childAtIndex(0).type() == ExpressionNode::Type::Matrix) {
    return SimplificationHelper::Map(*this, context, angleUnit);
  }
  return *this;
}

}
