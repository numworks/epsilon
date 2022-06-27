#include <poincare/default_parameter.h>

namespace Poincare {

Expression DefaultParameterNode::shallowReduce(const ReductionContext& reductionContext) {
  return DefaultParameter(this).shallowReduce(reductionContext);
}

Expression DefaultParameter::shallowReduce(const ExpressionNode::ReductionContext& reductionContext) {
  Expression result = actualExpression();
  replaceWithInPlace(result);
  return result.shallowReduce(reductionContext);
}

}
