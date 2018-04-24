#include "left_bracket_layout.h"

namespace Poincare {

ExpressionLayout * LeftBracketLayout::clone() const {
  return new LeftBracketLayout();
}

void LeftBracketLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  ctx->fillRect(KDRect(p.x()+k_externWidthMargin, p.y(), k_lineThickness, operandHeight()), expressionColor);
  ctx->fillRect(KDRect(p.x()+k_externWidthMargin, p.y(), k_bracketWidth, k_lineThickness), expressionColor);
  ctx->fillRect(KDRect(p.x()+k_externWidthMargin, p.y() + operandHeight(), k_bracketWidth, k_lineThickness), expressionColor);
}

}
