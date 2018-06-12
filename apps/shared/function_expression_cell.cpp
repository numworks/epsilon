#include "function_expression_cell.h"

namespace Shared {

FunctionExpressionCell::FunctionExpressionCell() :
  EvenOddExpressionCell()
{
}

KDSize FunctionExpressionCell::minimalSizeForOptimalDisplay() const {
  KDSize expressionSize = m_expressionView.minimalSizeForOptimalDisplay();
  return KDSize(m_margin+expressionSize.width(), expressionSize.height()+k_separatorThickness);
}

void FunctionExpressionCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor separatorColor = m_even ? Palette::WallScreen : KDColorWhite;
  // Color the horizontal separator
  ctx->fillRect(KDRect(0, bounds().height()-k_separatorThickness, bounds().width(), k_separatorThickness), separatorColor);
  // Color the margin
  ctx->fillRect(KDRect(0, 0, m_margin, bounds().height()-k_separatorThickness), backgroundColor());
}

void FunctionExpressionCell::layoutSubviews() {
  KDRect expressionFrame(m_margin, 0, bounds().width() - m_margin, bounds().height()-k_separatorThickness);
  m_expressionView.setFrame(expressionFrame);
}

}
