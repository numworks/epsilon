#include <poincare/square_bracket_pair_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

void SquareBracketPairLayoutNode::didCollapseSiblings(LayoutCursor * cursor) {
  if (cursor != nullptr) {
    cursor->setLayoutNode(childLayout());
    cursor->setPosition(LayoutCursor::Position::Left);
  }
}

void SquareBracketPairLayoutNode::RenderWithParameters(bool left, KDCoordinate childHeight, KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, KDCoordinate verticalMargin, KDCoordinate bracketWidth, bool renderTopBar, bool renderBottomBar, bool renderDoubleBar) {
  KDCoordinate horizontalBarX = left ? p.x() + k_externalWidthMargin : p.x() + k_lineThickness;
  KDCoordinate horizontalBarLength = bracketWidth - k_externalWidthMargin - k_lineThickness;
  KDCoordinate verticalBarX = left ? horizontalBarX : p.x() + bracketWidth - k_lineThickness - k_externalWidthMargin;
  KDCoordinate verticalBarY = p.y();
  KDCoordinate verticalBarLength = HeightGivenChildHeight(childHeight, verticalMargin);

  if (renderTopBar) {
    ctx->fillRect(KDRect(horizontalBarX, p.y(), horizontalBarLength, k_lineThickness), expressionColor);
  }
  if (renderBottomBar) {
    ctx->fillRect(KDRect(horizontalBarX, verticalBarY + verticalBarLength - k_lineThickness, horizontalBarLength, k_lineThickness), expressionColor);
  }

  if (renderDoubleBar) {
    ctx->fillRect(KDRect(verticalBarX, verticalBarY, k_lineThickness, verticalBarLength), expressionColor);
    verticalBarX += (left ? 1 : -1) * (k_lineThickness + k_doubleBarMargin);
  }
  ctx->fillRect(KDRect(verticalBarX, verticalBarY, k_lineThickness, verticalBarLength), expressionColor);
}

}
