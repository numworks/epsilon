#include "right_bracket_layout.h"

namespace Poincare {

ExpressionLayout * RightBracketLayout::clone() const {
  return new RightBracketLayout();
}

void RightBracketLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  ctx->fillRect(KDRect(p.x()+k_widthMargin, p.y(), k_lineThickness, operandHeight()), expressionColor);
  ctx->fillRect(KDRect(p.x()+k_widthMargin-k_bracketWidth+1, p.y(), k_bracketWidth, k_lineThickness), expressionColor);
  ctx->fillRect(KDRect(p.x()+k_widthMargin-k_bracketWidth+1, p.y() + operandHeight(), k_bracketWidth, k_lineThickness), expressionColor);
}

}
