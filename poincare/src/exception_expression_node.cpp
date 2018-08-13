#include <poincare/exception_expression_node.h>

namespace Poincare {

ExceptionExpressionNode * ExceptionExpression::ExceptionExpressionStaticNode() {
  static ExceptionExpressionNode exception;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&exception);
  return &exception;
}

}
