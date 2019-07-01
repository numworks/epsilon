#include <poincare/hyperbolic_trigonometric_function.h>


namespace Poincare {

Expression HyperbolicTrigonometricFunctionNode::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target, bool symbolicComputation) {
  return HyperbolicTrigonometricFunction(this).shallowReduce(context, complexFormat, angleUnit, target, symbolicComputation);
}

Expression HyperbolicTrigonometricFunction::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target, bool symbolicComputation) {
  {
    Expression e = Expression::defaultShallowReduce();
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression c = childAtIndex(0);
  if (childAtIndex(0).type() == ExpressionNode::Type::Matrix) {
    return mapOnMatrixChild(context, complexFormat, angleUnit, target, symbolicComputation);
  }
  return *this;
}

}
