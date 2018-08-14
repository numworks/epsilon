#include <poincare/uninitialized_expression_node.h>

namespace Poincare {

UninitializedExpressionNode * UninitializedExpression::UninitializedExpressionStaticNode() {
  static UninitializedExpressionNode exception;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&exception);
  return &exception;
}

}
