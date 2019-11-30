#include <poincare/right_parenthesis_layout.h>
#include <assert.h>
#include <stdlib.h>

namespace Poincare {

const uint8_t topRightCurve[ParenthesisLayoutNode::k_parenthesisCurveHeight][ParenthesisLayoutNode::k_parenthesisCurveWidth] = {
  {0x66, 0xF9, 0xFF, 0xFF, 0xFF},
  {0x9A, 0x40, 0xEB, 0xFF, 0xFF},
  {0xFF, 0xBF, 0x40, 0xF2, 0xFF},
  {0xFF, 0xFF, 0xB6, 0x49, 0xFF},
  {0xFF, 0xFF, 0xFF, 0x5A, 0xA9},
  {0xFF, 0xFF, 0xFF, 0xBE, 0x45},
  {0xFF, 0xFF, 0xFF, 0xEE, 0x11},
};

const uint8_t bottomRightCurve[ParenthesisLayoutNode::k_parenthesisCurveHeight][ParenthesisLayoutNode::k_parenthesisCurveWidth] = {
  {0xFF, 0xFF, 0xFF, 0xEE, 0x11},
  {0xFF, 0xFF, 0xFF, 0xBE, 0x45},
  {0xFF, 0xFF, 0xFF, 0x5A, 0xA9},
  {0xFF, 0xFF, 0xB6, 0x49, 0xFF},
  {0xFF, 0xBF, 0x40, 0xF2, 0xFF},
  {0x9A, 0x40, 0xEB, 0xFF, 0xFF},
  {0x66, 0xF9, 0xFF, 0xFF, 0xFF},
};

void RightParenthesisLayoutNode::RenderWithChildHeight(KDCoordinate childHeight, KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  KDColor parenthesisWorkingBuffer[k_parenthesisCurveHeight*k_parenthesisCurveWidth];
  KDCoordinate parenthesisHeight = ParenthesisLayoutNode::HeightGivenChildHeight(childHeight);
  KDRect frame = KDRect(p.x() + ParenthesisLayoutNode::k_widthMargin + ParenthesisLayoutNode::k_lineThickness - ParenthesisLayoutNode::k_parenthesisCurveWidth,
      p.y() + ParenthesisLayoutNode::k_externHeightMargin,
      ParenthesisLayoutNode::k_parenthesisCurveWidth,
      ParenthesisLayoutNode::k_parenthesisCurveHeight);

  ctx->blendRectWithMask(frame, expressionColor, (const uint8_t *)topRightCurve, parenthesisWorkingBuffer);

  frame = KDRect(p.x() + ParenthesisLayoutNode::k_widthMargin + ParenthesisLayoutNode::k_lineThickness - ParenthesisLayoutNode::k_parenthesisCurveWidth,
    p.y() + parenthesisHeight - ParenthesisLayoutNode::k_parenthesisCurveHeight - ParenthesisLayoutNode::k_externHeightMargin,
    ParenthesisLayoutNode::k_parenthesisCurveWidth,
    ParenthesisLayoutNode::k_parenthesisCurveHeight);

  ctx->blendRectWithMask(frame, expressionColor, (const uint8_t *)bottomRightCurve, parenthesisWorkingBuffer);

  ctx->fillRect(KDRect(p.x()+ParenthesisLayoutNode::k_widthMargin,
        p.y()+ParenthesisLayoutNode::k_parenthesisCurveHeight+2,
        ParenthesisLayoutNode::k_lineThickness,
        parenthesisHeight - 2*(ParenthesisLayoutNode::k_parenthesisCurveHeight+ParenthesisLayoutNode::k_externHeightMargin)),
      expressionColor);
}

bool RightParenthesisLayoutNode::isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const {
  if (*numberOfOpenParenthesis == 0 && !goingLeft) {
    return false;
  }
  *numberOfOpenParenthesis = goingLeft ? *numberOfOpenParenthesis + 1 : *numberOfOpenParenthesis - 1;
  return true;
}

void RightParenthesisLayoutNode::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart, Layout * selectionEnd, KDColor selectionColor) {
  RenderWithChildHeight(ParenthesisLayoutNode::ChildHeightGivenLayoutHeight(layoutSize().height()), ctx, p, expressionColor, backgroundColor);
}

}
