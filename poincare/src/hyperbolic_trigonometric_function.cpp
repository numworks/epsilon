#include <poincare/hyperbolic_trigonometric_function.h>

namespace Poincare {

Expression HyperbolicTrigonometricFunctionNode::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) {
  return HyperbolicTrigonometricFunction(this).shallowReduce(context, complexFormat, angleUnit);
}

Expression HyperbolicTrigonometricFunction::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) {
  {
    Expression e = Expression::defaultShallowReduce();
    if (e.isUndefined()) {
      return e;
    }
  }
#if MATRIX_EXACT_REDUCING
  Expression c = childAtIndex(0);
  if (c.type() == ExpressionNode::Type::Matrix) {
    return SimplificationHelper::Map(*this, context, angleUnit);
  }
#endif
  return *this;
}

}
