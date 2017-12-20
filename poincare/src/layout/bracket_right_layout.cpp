#include "bracket_right_layout.h"

namespace Poincare {

ExpressionLayout * BracketRightLayout::clone() const {
  BracketRightLayout * layout = new BracketRightLayout();
  return layout;
}

void BracketRightLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  //TODO Make sure m_operandHeight is up-to-date.
  ctx->fillRect(KDRect(p.x()+k_widthMargin, p.y(), k_lineThickness, m_operandHeight), expressionColor);
  ctx->fillRect(KDRect(p.x()+k_widthMargin-k_bracketWidth+1, p.y(), k_bracketWidth, k_lineThickness), expressionColor);
  ctx->fillRect(KDRect(p.x()+k_widthMargin-k_bracketWidth+1, p.y() + m_operandHeight, k_bracketWidth, k_lineThickness), expressionColor);
}

}
