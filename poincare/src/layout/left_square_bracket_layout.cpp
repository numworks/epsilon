#include "left_square_bracket_layout.h"

namespace Poincare {

ExpressionLayout * LeftSquareBracketLayout::clone() const {
  return new LeftSquareBracketLayout();
}

void LeftSquareBracketLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  ctx->fillRect(KDRect(p.x()+k_externWidthMargin, p.y(), k_lineThickness, operandHeight()), expressionColor);
  ctx->fillRect(KDRect(p.x()+k_externWidthMargin, p.y(), k_bracketWidth, k_lineThickness), expressionColor);
  ctx->fillRect(KDRect(p.x()+k_externWidthMargin, p.y() + operandHeight(), k_bracketWidth, k_lineThickness), expressionColor);
}

}
