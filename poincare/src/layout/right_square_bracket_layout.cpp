#include "right_square_bracket_layout.h"

namespace Poincare {

ExpressionLayout * RightSquareBracketLayout::clone() const {
  return new RightSquareBracketLayout();
}

void RightSquareBracketLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  ctx->fillRect(KDRect(p.x()+k_widthMargin, p.y(), k_lineThickness, operandHeight()), expressionColor);
  ctx->fillRect(KDRect(p.x()+k_widthMargin-k_bracketWidth+1, p.y(), k_bracketWidth, k_lineThickness), expressionColor);
  ctx->fillRect(KDRect(p.x()+k_widthMargin-k_bracketWidth+1, p.y() + operandHeight(), k_bracketWidth, k_lineThickness), expressionColor);
}

}
