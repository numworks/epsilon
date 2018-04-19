#include "parenthesis_left_layout.h"
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

const uint8_t topLeftCurve[ParenthesisLeftRightLayout::k_parenthesisCurveHeight][ParenthesisLeftRightLayout::k_parenthesisCurveWidth] = {
  {0xFF, 0xFF, 0xFF, 0xF9, 0x66},
  {0xFF, 0xFF, 0xEB, 0x40, 0x9A},
  {0xFF, 0xF2, 0x40, 0xBF, 0xFF},
  {0xFF, 0x49, 0xB6, 0xFF, 0xFF},
  {0xA9, 0x5A, 0xFF, 0xFF, 0xFF},
  {0x45, 0xBE, 0xFF, 0xFF, 0xFF},
  {0x11, 0xEE, 0xFF, 0xFF, 0xFF},
};

const uint8_t bottomLeftCurve[ParenthesisLeftRightLayout::k_parenthesisCurveHeight][ParenthesisLeftRightLayout::k_parenthesisCurveWidth] = {
  {0x11, 0xEE, 0xFF, 0xFF, 0xFF},
  {0x45, 0xBE, 0xFF, 0xFF, 0xFF},
  {0xA9, 0x5A, 0xFF, 0xFF, 0xFF},
  {0xFF, 0x49, 0xB6, 0xFF, 0xFF},
  {0xFF, 0xF2, 0x40, 0xBF, 0xFF},
  {0xFF, 0xFF, 0xEB, 0x40, 0x9A},
  {0xFF, 0xFF, 0xFF, 0xF9, 0x66},
};

ExpressionLayout * ParenthesisLeftLayout::clone() const {
  ParenthesisLeftLayout * layout = new ParenthesisLeftLayout();
  return layout;
}

bool ParenthesisLeftLayout::isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const {
  if (*numberOfOpenParenthesis == 0 && goingLeft) {
    return false;
  }
  *numberOfOpenParenthesis = goingLeft ? *numberOfOpenParenthesis - 1 : *numberOfOpenParenthesis + 1;
  return true;
}

void ParenthesisLeftLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  KDRect frame(p.x()+ParenthesisLeftRightLayout::k_externWidthMargin,
      p.y()+ParenthesisLeftRightLayout::k_externHeightMargin,
      ParenthesisLeftRightLayout::k_parenthesisCurveWidth,
      ParenthesisLeftRightLayout::k_parenthesisCurveHeight);

  ctx->blendRectWithMask(frame, expressionColor, (const uint8_t *)topLeftCurve, (KDColor *)(ParenthesisLeftRightLayout::s_parenthesisWorkingBuffer));

  frame = KDRect(p.x()+ParenthesisLeftRightLayout::k_externWidthMargin,
      p.y() + operandHeight() - ParenthesisLeftRightLayout::k_parenthesisCurveHeight - ParenthesisLeftRightLayout::k_externHeightMargin,
      ParenthesisLeftRightLayout::k_parenthesisCurveWidth,
      ParenthesisLeftRightLayout::k_parenthesisCurveHeight);

  ctx->blendRectWithMask(frame, expressionColor, (const uint8_t *)bottomLeftCurve, (KDColor *)(ParenthesisLeftRightLayout::s_parenthesisWorkingBuffer));

  ctx->fillRect(KDRect(p.x()+ParenthesisLeftRightLayout::k_externWidthMargin,
        p.y()+ParenthesisLeftRightLayout::k_parenthesisCurveHeight+ParenthesisLeftRightLayout::k_externHeightMargin,
        ParenthesisLeftRightLayout::k_lineThickness,
        operandHeight() - 2*(ParenthesisLeftRightLayout::k_parenthesisCurveHeight+ParenthesisLeftRightLayout::k_externHeightMargin)),
      expressionColor);
}

}
