#include "parenthesis_right_layout.h"
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

const uint8_t topRightCurve[ParenthesisLeftRightLayout::k_parenthesisCurveHeight][ParenthesisLeftRightLayout::k_parenthesisCurveWidth] = {
  {0x66, 0xF9, 0xFF, 0xFF, 0xFF},
  {0x9A, 0x40, 0xEB, 0xFF, 0xFF},
  {0xFF, 0xBF, 0x40, 0xF2, 0xFF},
  {0xFF, 0xFF, 0xB6, 0x49, 0xFF},
  {0xFF, 0xFF, 0xFF, 0x5A, 0xA9},
  {0xFF, 0xFF, 0xFF, 0xBE, 0x45},
  {0xFF, 0xFF, 0xFF, 0xEE, 0x11},
};

const uint8_t bottomRightCurve[ParenthesisLeftRightLayout::k_parenthesisCurveHeight][ParenthesisLeftRightLayout::k_parenthesisCurveWidth] = {
  {0xFF, 0xFF, 0xFF, 0xEE, 0x11},
  {0xFF, 0xFF, 0xFF, 0xBE, 0x45},
  {0xFF, 0xFF, 0xFF, 0x5A, 0xA9},
  {0xFF, 0xFF, 0xB6, 0x49, 0xFF},
  {0xFF, 0xBF, 0x40, 0xF2, 0xFF},
  {0x9A, 0x40, 0xEB, 0xFF, 0xFF},
  {0x66, 0xF9, 0xFF, 0xFF, 0xFF},
};

ExpressionLayout * ParenthesisRightLayout::clone() const {
  ParenthesisRightLayout * layout = new ParenthesisRightLayout();
  return layout;
}

bool ParenthesisRightLayout::isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const {
  if (*numberOfOpenParenthesis == 0 && !goingLeft) {
    return false;
  }
  *numberOfOpenParenthesis = goingLeft ? *numberOfOpenParenthesis + 1 : *numberOfOpenParenthesis - 1;
  return true;
}

void ParenthesisRightLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  KDRect frame = KDRect(p.x() + ParenthesisLeftRightLayout::k_widthMargin + ParenthesisLeftRightLayout::k_lineThickness - ParenthesisLeftRightLayout::k_parenthesisCurveWidth,
      p.y() + ParenthesisLeftRightLayout::k_externHeightMargin,
      ParenthesisLeftRightLayout::k_parenthesisCurveWidth,
      ParenthesisLeftRightLayout::k_parenthesisCurveHeight);

  ctx->blendRectWithMask(frame, expressionColor, (const uint8_t *)topRightCurve, (KDColor *)(ParenthesisLeftRightLayout::s_parenthesisWorkingBuffer));

  frame = KDRect(p.x() + ParenthesisLeftRightLayout::k_widthMargin + ParenthesisLeftRightLayout::k_lineThickness - ParenthesisLeftRightLayout::k_parenthesisCurveWidth,
    p.y() + operandHeight() - ParenthesisLeftRightLayout::k_parenthesisCurveHeight - ParenthesisLeftRightLayout::k_externHeightMargin,
    ParenthesisLeftRightLayout::k_parenthesisCurveWidth,
    ParenthesisLeftRightLayout::k_parenthesisCurveHeight);

  ctx->blendRectWithMask(frame, expressionColor, (const uint8_t *)bottomRightCurve, (KDColor *)(ParenthesisLeftRightLayout::s_parenthesisWorkingBuffer));

  ctx->fillRect(KDRect(p.x()+ParenthesisLeftRightLayout::k_widthMargin,
        p.y()+ParenthesisLeftRightLayout::k_parenthesisCurveHeight+2,
        ParenthesisLeftRightLayout::k_lineThickness,
        operandHeight() - 2*(ParenthesisLeftRightLayout::k_parenthesisCurveHeight+ParenthesisLeftRightLayout::k_externHeightMargin)),
      expressionColor);
}

}







