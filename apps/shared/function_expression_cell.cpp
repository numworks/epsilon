#include "function_expression_cell.h"

namespace Shared {

KDSize FunctionExpressionCell::minimalSizeForOptimalDisplay() const {
  KDSize expressionSize = m_expressionView.minimalSizeForOptimalDisplay();
  return KDSize(m_leftMargin + expressionSize.width() + m_rightMargin, expressionSize.height()+k_separatorThickness);
}

void FunctionExpressionCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor separatorColor = m_even ? Palette::WallScreen : KDColorWhite;
  // Color the horizontal separator
  ctx->fillRect(KDRect(0, bounds().height()-k_separatorThickness, bounds().width(), k_separatorThickness), separatorColor);
  // Color the left margin
  ctx->fillRect(KDRect(0, 0, m_leftMargin, bounds().height()-k_separatorThickness), backgroundColor());
  // Color the right margin
  ctx->fillRect(KDRect(0, bounds().width() - m_rightMargin, m_rightMargin, bounds().height()-k_separatorThickness), backgroundColor());

}

void FunctionExpressionCell::layoutSubviews(bool force) {
  KDRect expressionFrame(m_leftMargin, 0, bounds().width() - m_leftMargin - m_rightMargin, bounds().height()-k_separatorThickness);
  m_expressionView.setFrame(expressionFrame, force);
}

}
