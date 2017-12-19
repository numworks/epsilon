#include "bracket_left_layout.h"

namespace Poincare {

ExpressionLayout * BracketLeftLayout::clone() const {
  BracketLeftLayout * layout = new BracketLeftLayout();
  return layout;
}

void BracketLeftLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  //TODO Make sure m_operandHeight is up-to-date.
  ctx->fillRect(KDRect(p.x()+k_externWidthMargin, p.y(), k_lineThickness, m_operandHeight), expressionColor);
  ctx->fillRect(KDRect(p.x()+k_externWidthMargin, p.y(), k_bracketWidth, k_lineThickness), expressionColor);
  ctx->fillRect(KDRect(p.x()+k_externWidthMargin, p.y() + m_operandHeight, k_bracketWidth, k_lineThickness), expressionColor);
}

}
