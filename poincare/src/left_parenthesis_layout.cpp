#include <poincare/left_parenthesis_layout.h>
#include <assert.h>
#include <stdlib.h>

namespace Poincare {

const uint8_t topLeftCurve[ParenthesisLayoutNode::k_curveHeight][ParenthesisLayoutNode::k_curveWidth] = {
  {0xFF, 0xFF, 0xFF, 0xF9, 0x66},
  {0xFF, 0xFF, 0xEB, 0x40, 0x9A},
  {0xFF, 0xF2, 0x40, 0xBF, 0xFF},
  {0xFF, 0x49, 0xB6, 0xFF, 0xFF},
  {0xA9, 0x5A, 0xFF, 0xFF, 0xFF},
  {0x45, 0xBE, 0xFF, 0xFF, 0xFF},
  {0x11, 0xEE, 0xFF, 0xFF, 0xFF},
};

const uint8_t bottomLeftCurve[ParenthesisLayoutNode::k_curveHeight][ParenthesisLayoutNode::k_curveWidth] = {
  {0x11, 0xEE, 0xFF, 0xFF, 0xFF},
  {0x45, 0xBE, 0xFF, 0xFF, 0xFF},
  {0xA9, 0x5A, 0xFF, 0xFF, 0xFF},
  {0xFF, 0x49, 0xB6, 0xFF, 0xFF},
  {0xFF, 0xF2, 0x40, 0xBF, 0xFF},
  {0xFF, 0xFF, 0xEB, 0x40, 0x9A},
  {0xFF, 0xFF, 0xFF, 0xF9, 0x66},
};

void LeftParenthesisLayoutNode::RenderWithChildHeight(KDCoordinate childHeight, KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  KDColor parenthesisWorkingBuffer[k_curveHeight * k_curveWidth];
  KDCoordinate parenthesisHeight = HeightGivenChildHeight(childHeight);
  KDRect frame = KDRect(k_externWidthMargin,
      k_verticalMargin,
      k_curveWidth,
      k_curveHeight);

  ctx->blendRectWithMask(frame.translatedBy(p), expressionColor, (const uint8_t *)topLeftCurve, parenthesisWorkingBuffer);

  frame = KDRect(k_externWidthMargin,
      parenthesisHeight - k_curveHeight - k_verticalMargin,
      k_curveWidth,
      k_curveHeight);

  ctx->blendRectWithMask(frame.translatedBy(p), expressionColor, (const uint8_t *)bottomLeftCurve, parenthesisWorkingBuffer);

  frame = KDRect(k_externWidthMargin,
      k_curveHeight + k_verticalMargin,
      k_lineThickness,
      parenthesisHeight - 2 * (k_curveHeight + k_verticalMargin));

  ctx->fillRect(frame.translatedBy(p), expressionColor);
}

void LeftParenthesisLayoutNode::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart, Layout * selectionEnd, KDColor selectionColor) {
  RenderWithChildHeight(OptimalChildHeightGivenLayoutHeight(layoutSize().height()), ctx, p, expressionColor, backgroundColor);
}

}
