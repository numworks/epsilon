#include <poincare/square_bracket_pair_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

void SquareBracketPairLayoutNode::RenderWithParameters(bool left, KDCoordinate childHeight, KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, KDCoordinate verticalMargin, KDCoordinate bracketWidth, bool renderTopBar, bool renderBottomBar, bool renderDoubleBar) {
  KDCoordinate horizontalBarX = p.x() + (left ? k_externalWidthMargin : k_lineThickness);
  KDCoordinate horizontalBarLength = bracketWidth - k_externalWidthMargin - k_lineThickness;
  KDCoordinate verticalBarX = left ? horizontalBarX : p.x() + bracketWidth - k_lineThickness - k_externalWidthMargin;
  KDCoordinate verticalBarY = p.y();
  KDCoordinate verticalBarLength = HeightGivenChildHeight(childHeight, verticalMargin);

  if (renderTopBar) {
    ctx->fillRect(KDRect(horizontalBarX, verticalBarY, horizontalBarLength, k_lineThickness), expressionColor);
  }
  if (renderBottomBar) {
    ctx->fillRect(KDRect(horizontalBarX, verticalBarY + verticalBarLength - k_lineThickness, horizontalBarLength, k_lineThickness), expressionColor);
  }

  ctx->fillRect(KDRect(verticalBarX, verticalBarY, k_lineThickness, verticalBarLength), expressionColor);

  if (renderDoubleBar) {
    verticalBarX += (left ? 1 : -1) * (k_lineThickness + k_doubleBarMargin);
    ctx->fillRect(KDRect(verticalBarX, verticalBarY, k_lineThickness, verticalBarLength), expressionColor);
  }
}

}
