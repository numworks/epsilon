#include <escher/expression_view.h>

ExpressionView::ExpressionView() :
  m_expressionLayout(nullptr),
  m_textColor(KDColorBlack),
  m_backgroundColor(KDColorWhite)
{
}

void ExpressionView::setExpression(ExpressionLayout * expressionLayout) {
  m_expressionLayout = expressionLayout;
}

void ExpressionView::setBackgroundColor(KDColor backgroundColor) {
  m_backgroundColor = backgroundColor;
}

void ExpressionView::setTextColor(KDColor textColor) {
  m_textColor = textColor;
}

KDSize ExpressionView::minimalSizeForOptimalDisplay() {
  if (m_expressionLayout ==  nullptr) {
    return KDSizeZero;
  }
  return m_expressionLayout->size();
}

void ExpressionView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, m_backgroundColor);
  if (m_expressionLayout != nullptr) {
    //Position the origin of expression
    KDSize expressionSize = m_expressionLayout->size();
    KDPoint origin(0, 0.5f*(m_frame.height() - expressionSize.height()));
    m_expressionLayout->draw(ctx, origin, m_textColor, m_backgroundColor);
  }
}
