#include <poincare/uninitialized_expression_node.h>

namespace Poincare {

UninitializedExpressionNode * UninitializedExpressionNode::UninitializedExpressionStaticNode() {
  static UninitializedExpressionNode exception;
  return &exception;
}

}
