#include <poincare/hyperbolic_trigonometric_function.h>

namespace Poincare {

Expression HyperbolicTrigonometricFunctionNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols) {
  return HyperbolicTrigonometricFunction(this).shallowReduce(context, angleUnit, replaceSymbols);
}

Expression HyperbolicTrigonometricFunction::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols) {
  {
    Expression e = Expression::defaultShallowReduce(context, angleUnit);
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
