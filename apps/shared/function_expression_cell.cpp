#include "function_expression_cell.h"

using namespace Escher;

namespace Shared {

KDSize FunctionExpressionCell::minimalSizeForOptimalDisplay() const {
  KDSize expressionSize = m_expressionView.minimalSizeForOptimalDisplay();
  return KDSize(m_leftMargin + expressionSize.width() + m_rightMargin, expressionSize.height());
}

void FunctionExpressionCell::drawRect(KDContext * ctx, KDRect rect) const {
  // Color the left margin
  ctx->fillRect(KDRect(0, 0, m_leftMargin, bounds().height()), backgroundColor());
  // Color the right margin
  ctx->fillRect(KDRect(0, bounds().width() - m_rightMargin, m_rightMargin, bounds().height()), backgroundColor());
}

void FunctionExpressionCell::layoutSubviews(bool force) {
  KDRect expressionFrame(m_leftMargin, 0, bounds().width() - m_leftMargin - m_rightMargin, bounds().height());
  m_expressionView.setFrame(expressionFrame, force);
}

}
