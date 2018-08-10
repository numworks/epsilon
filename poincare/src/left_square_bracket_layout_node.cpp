#include <poincare/left_square_bracket_layout_node.h>
#include <poincare/allocation_failure_layout_node.h>

namespace Poincare {

LeftSquareBracketLayoutNode * LeftSquareBracketLayoutNode::FailedAllocationStaticNode() {
  static AllocationFailureLayoutNode<LeftSquareBracketLayoutNode> failure;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failure);
  return &failure;
}

void LeftSquareBracketLayoutNode::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  ctx->fillRect(KDRect(p.x()+k_externWidthMargin, p.y(), k_lineThickness, childHeight()), expressionColor);
  ctx->fillRect(KDRect(p.x()+k_externWidthMargin, p.y(), k_bracketWidth, k_lineThickness), expressionColor);
  ctx->fillRect(KDRect(p.x()+k_externWidthMargin, p.y() + childHeight(), k_bracketWidth, k_lineThickness), expressionColor);
}

}
