#include <poincare/right_square_bracket_layout.h>

namespace Poincare {

void RightSquareBracketLayoutNode::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  ctx->fillRect(KDRect(p.x()+k_widthMargin, p.y(), k_lineThickness, childHeight()), expressionColor);
  ctx->fillRect(KDRect(p.x()+k_widthMargin-k_bracketWidth+1, p.y(), k_bracketWidth, k_lineThickness), expressionColor);
  ctx->fillRect(KDRect(p.x()+k_widthMargin-k_bracketWidth+1, p.y() + childHeight(), k_bracketWidth, k_lineThickness), expressionColor);
}

RightSquareBracketLayout RightSquareBracketLayout::Builder() {
  void * bufferNode = TreePool::sharedPool()->alloc(sizeof(RightSquareBracketLayoutNode));
  RightSquareBracketLayoutNode * node = new (bufferNode) RightSquareBracketLayoutNode();
  TreeHandle h = TreeHandle::BuildWithBasicChildren(node);
  return static_cast<RightSquareBracketLayout &>(h);
}

}
