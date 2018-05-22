#include "model_expression_cell.h"

namespace Shared {

ModelExpressionCell::ModelExpressionCell() :
  EvenOddExpressionCell()
{
}

KDSize ModelExpressionCell::minimalSizeForOptimalDisplay() const {
  KDSize expressionSize = m_expressionView.minimalSizeForOptimalDisplay();
  return KDSize(k_margin+expressionSize.width(), expressionSize.height()+k_separatorThickness);
}

void ModelExpressionCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor separatorColor = m_even ? Palette::WallScreen : KDColorWhite;
  // Color the horizontal separator
  ctx->fillRect(KDRect(0, bounds().height()-k_separatorThickness, bounds().width(), k_separatorThickness), separatorColor);
  // Color the margin
  ctx->fillRect(KDRect(0, 0, k_margin, bounds().height()-k_separatorThickness), backgroundColor());
}

void ModelExpressionCell::layoutSubviews() {
  KDRect expressionFrame(k_margin, 0, bounds().width() - k_margin, bounds().height()-k_separatorThickness);
  m_expressionView.setFrame(expressionFrame);
}

}
