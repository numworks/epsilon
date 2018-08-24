#include <poincare/hyperbolic_trigonometric_function.h>
#include <poincare/layout_helper.h>

namespace Poincare {

LayoutReference HyperbolicTrigonometricFunctionNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(HyperbolicTrigonometricFunction(this), floatDisplayMode, numberOfSignificantDigits, name());
}

Expression HyperbolicTrigonometricFunctionNode::shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const {
  return HyperbolicTrigonometricFunction(this).shallowReduce(context, angleUnit);
}

Expression HyperbolicTrigonometricFunction::shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const {
  Expression e = Expression::defaultShallowReduce(context, angleUnit);
  if (e.isUndefinedOrAllocationFailure()) {
    return e;
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
