#include <poincare/hyperbolic_trigonometric_function.h>


namespace Poincare {

Expression HyperbolicTrigonometricFunctionNode::shallowReduce(ReductionContext reductionContext) {
  return HyperbolicTrigonometricFunction(this).shallowReduce(reductionContext);
}

Expression HyperbolicTrigonometricFunction::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = Expression::defaultShallowReduce();
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression c = childAtIndex(0);
  if (childAtIndex(0).type() == ExpressionNode::Type::Matrix) {
    return mapOnMatrixChild(reductionContext);
  }
  return *this;
}

}
