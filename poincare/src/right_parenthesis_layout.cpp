#include <poincare/right_parenthesis_layout.h>
#include <assert.h>
#include <stdlib.h>

namespace Poincare {

const uint8_t topRightCurve[ParenthesisLayoutNode::k_curveHeight][ParenthesisLayoutNode::k_curveWidth] = {
  {0x66, 0xF9, 0xFF, 0xFF, 0xFF},
  {0x9A, 0x40, 0xEB, 0xFF, 0xFF},
  {0xFF, 0xBF, 0x40, 0xF2, 0xFF},
  {0xFF, 0xFF, 0xB6, 0x49, 0xFF},
  {0xFF, 0xFF, 0xFF, 0x5A, 0xA9},
  {0xFF, 0xFF, 0xFF, 0xBE, 0x45},
  {0xFF, 0xFF, 0xFF, 0xEE, 0x11},
};

const uint8_t bottomRightCurve[ParenthesisLayoutNode::k_curveHeight][ParenthesisLayoutNode::k_curveWidth] = {
  {0xFF, 0xFF, 0xFF, 0xEE, 0x11},
  {0xFF, 0xFF, 0xFF, 0xBE, 0x45},
  {0xFF, 0xFF, 0xFF, 0x5A, 0xA9},
  {0xFF, 0xFF, 0xB6, 0x49, 0xFF},
  {0xFF, 0xBF, 0x40, 0xF2, 0xFF},
  {0x9A, 0x40, 0xEB, 0xFF, 0xFF},
  {0x66, 0xF9, 0xFF, 0xFF, 0xFF},
};

void RightParenthesisLayoutNode::RenderWithChildHeight(KDCoordinate childHeight, KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  KDColor parenthesisWorkingBuffer[k_curveHeight * k_curveWidth];
  KDCoordinate parenthesisHeight = HeightGivenChildHeight(childHeight);
  KDRect frame = KDRect(k_widthMargin,
      k_verticalMargin,
      k_curveWidth,
      k_curveHeight);

  ctx->blendRectWithMask(frame.translatedBy(p), expressionColor, (const uint8_t *)topRightCurve, parenthesisWorkingBuffer);

  frame = KDRect(k_widthMargin,
      parenthesisHeight - k_curveHeight - k_verticalMargin,
      k_curveWidth,
      k_curveHeight);

  ctx->blendRectWithMask(frame.translatedBy(p), expressionColor, (const uint8_t *)bottomRightCurve, parenthesisWorkingBuffer);

  frame = KDRect(k_widthMargin + k_curveWidth - k_lineThickness,
      k_curveHeight + k_verticalMargin,
      k_lineThickness,
      parenthesisHeight - 2 * (k_curveHeight + k_verticalMargin));

  ctx->fillRect(frame.translatedBy(p), expressionColor);
}

void RightParenthesisLayoutNode::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart, Layout * selectionEnd, KDColor selectionColor) {
  RenderWithChildHeight(OptimalChildHeightGivenLayoutHeight(layoutSize().height()), ctx, p, expressionColor, backgroundColor);
}

}
