#include <poincare/left_square_bracket_layout.h>

namespace Poincare {

void LeftSquareBracketLayoutNode::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  ctx->fillRect(KDRect(p.x()+k_externWidthMargin, p.y(), k_lineThickness, childHeight()), expressionColor);
  ctx->fillRect(KDRect(p.x()+k_externWidthMargin, p.y(), k_bracketWidth, k_lineThickness), expressionColor);
  ctx->fillRect(KDRect(p.x()+k_externWidthMargin, p.y() + childHeight(), k_bracketWidth, k_lineThickness), expressionColor);
}

LeftSquareBracketLayout LeftSquareBracketLayout::Builder() {
  void * bufferNode = TreePool::sharedPool()->alloc(sizeof(LeftSquareBracketLayoutNode));
  LeftSquareBracketLayoutNode * node = new (bufferNode) LeftSquareBracketLayoutNode();
  TreeHandle h = TreeHandle::BuildWithBasicChildren(node);
  return static_cast<LeftSquareBracketLayout &>(h);
}

}
