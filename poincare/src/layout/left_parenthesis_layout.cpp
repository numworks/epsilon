#include "left_parenthesis_layout.h"
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

const uint8_t topLeftCurve[ParenthesisLayout::k_parenthesisCurveHeight][ParenthesisLayout::k_parenthesisCurveWidth] = {
  {0xFF, 0xFF, 0xFF, 0xF9, 0x66},
  {0xFF, 0xFF, 0xEB, 0x40, 0x9A},
  {0xFF, 0xF2, 0x40, 0xBF, 0xFF},
  {0xFF, 0x49, 0xB6, 0xFF, 0xFF},
  {0xA9, 0x5A, 0xFF, 0xFF, 0xFF},
  {0x45, 0xBE, 0xFF, 0xFF, 0xFF},
  {0x11, 0xEE, 0xFF, 0xFF, 0xFF},
};

const uint8_t bottomLeftCurve[ParenthesisLayout::k_parenthesisCurveHeight][ParenthesisLayout::k_parenthesisCurveWidth] = {
  {0x11, 0xEE, 0xFF, 0xFF, 0xFF},
  {0x45, 0xBE, 0xFF, 0xFF, 0xFF},
  {0xA9, 0x5A, 0xFF, 0xFF, 0xFF},
  {0xFF, 0x49, 0xB6, 0xFF, 0xFF},
  {0xFF, 0xF2, 0x40, 0xBF, 0xFF},
  {0xFF, 0xFF, 0xEB, 0x40, 0x9A},
  {0xFF, 0xFF, 0xFF, 0xF9, 0x66},
};

ExpressionLayout * LeftParenthesisLayout::clone() const {
  LeftParenthesisLayout * layout = new LeftParenthesisLayout();
  return layout;
}

bool LeftParenthesisLayout::isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const {
  if (*numberOfOpenParenthesis == 0 && goingLeft) {
    return false;
  }
  *numberOfOpenParenthesis = goingLeft ? *numberOfOpenParenthesis - 1 : *numberOfOpenParenthesis + 1;
  return true;
}

void LeftParenthesisLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  KDRect frame(p.x()+ParenthesisLayout::k_externWidthMargin,
      p.y()+ParenthesisLayout::k_externHeightMargin,
      ParenthesisLayout::k_parenthesisCurveWidth,
      ParenthesisLayout::k_parenthesisCurveHeight);

  ctx->blendRectWithMask(frame, expressionColor, (const uint8_t *)topLeftCurve, (KDColor *)(ParenthesisLayout::s_parenthesisWorkingBuffer));

  frame = KDRect(p.x()+ParenthesisLayout::k_externWidthMargin,
      p.y() + size().height() - ParenthesisLayout::k_parenthesisCurveHeight - ParenthesisLayout::k_externHeightMargin,
      ParenthesisLayout::k_parenthesisCurveWidth,
      ParenthesisLayout::k_parenthesisCurveHeight);

  ctx->blendRectWithMask(frame, expressionColor, (const uint8_t *)bottomLeftCurve, (KDColor *)(ParenthesisLayout::s_parenthesisWorkingBuffer));

  ctx->fillRect(KDRect(p.x()+ParenthesisLayout::k_externWidthMargin,
        p.y()+ParenthesisLayout::k_parenthesisCurveHeight+ParenthesisLayout::k_externHeightMargin,
        ParenthesisLayout::k_lineThickness,
        size().height() - 2*(ParenthesisLayout::k_parenthesisCurveHeight+ParenthesisLayout::k_externHeightMargin)),
      expressionColor);
}

}
