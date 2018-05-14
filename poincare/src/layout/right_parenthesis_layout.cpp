#include "right_parenthesis_layout.h"
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

const uint8_t topRightCurve[ParenthesisLayout::k_parenthesisCurveHeight][ParenthesisLayout::k_parenthesisCurveWidth] = {
  {0x66, 0xF9, 0xFF, 0xFF, 0xFF},
  {0x9A, 0x40, 0xEB, 0xFF, 0xFF},
  {0xFF, 0xBF, 0x40, 0xF2, 0xFF},
  {0xFF, 0xFF, 0xB6, 0x49, 0xFF},
  {0xFF, 0xFF, 0xFF, 0x5A, 0xA9},
  {0xFF, 0xFF, 0xFF, 0xBE, 0x45},
  {0xFF, 0xFF, 0xFF, 0xEE, 0x11},
};

const uint8_t bottomRightCurve[ParenthesisLayout::k_parenthesisCurveHeight][ParenthesisLayout::k_parenthesisCurveWidth] = {
  {0xFF, 0xFF, 0xFF, 0xEE, 0x11},
  {0xFF, 0xFF, 0xFF, 0xBE, 0x45},
  {0xFF, 0xFF, 0xFF, 0x5A, 0xA9},
  {0xFF, 0xFF, 0xB6, 0x49, 0xFF},
  {0xFF, 0xBF, 0x40, 0xF2, 0xFF},
  {0x9A, 0x40, 0xEB, 0xFF, 0xFF},
  {0x66, 0xF9, 0xFF, 0xFF, 0xFF},
};

ExpressionLayout * RightParenthesisLayout::clone() const {
  RightParenthesisLayout * layout = new RightParenthesisLayout();
  return layout;
}

bool RightParenthesisLayout::isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const {
  if (*numberOfOpenParenthesis == 0 && !goingLeft) {
    return false;
  }
  *numberOfOpenParenthesis = goingLeft ? *numberOfOpenParenthesis + 1 : *numberOfOpenParenthesis - 1;
  return true;
}

void RightParenthesisLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  KDRect frame = KDRect(p.x() + ParenthesisLayout::k_widthMargin + ParenthesisLayout::k_lineThickness - ParenthesisLayout::k_parenthesisCurveWidth,
      p.y() + ParenthesisLayout::k_externHeightMargin,
      ParenthesisLayout::k_parenthesisCurveWidth,
      ParenthesisLayout::k_parenthesisCurveHeight);

  ctx->blendRectWithMask(frame, expressionColor, (const uint8_t *)topRightCurve, (KDColor *)(ParenthesisLayout::s_parenthesisWorkingBuffer));

  frame = KDRect(p.x() + ParenthesisLayout::k_widthMargin + ParenthesisLayout::k_lineThickness - ParenthesisLayout::k_parenthesisCurveWidth,
    p.y() + size().height() - ParenthesisLayout::k_parenthesisCurveHeight - ParenthesisLayout::k_externHeightMargin,
    ParenthesisLayout::k_parenthesisCurveWidth,
    ParenthesisLayout::k_parenthesisCurveHeight);

  ctx->blendRectWithMask(frame, expressionColor, (const uint8_t *)bottomRightCurve, (KDColor *)(ParenthesisLayout::s_parenthesisWorkingBuffer));

  ctx->fillRect(KDRect(p.x()+ParenthesisLayout::k_widthMargin,
        p.y()+ParenthesisLayout::k_parenthesisCurveHeight+2,
        ParenthesisLayout::k_lineThickness,
        size().height() - 2*(ParenthesisLayout::k_parenthesisCurveHeight+ParenthesisLayout::k_externHeightMargin)),
      expressionColor);
}

}







