#include <escher/expression_view.h>
using namespace Poincare;

ExpressionView::ExpressionView(float horizontalAlignment, float verticalAlignment,
    KDColor textColor, KDColor backgroundColor) :
  m_expressionLayout(nullptr),
  m_horizontalAlignment(horizontalAlignment),
  m_verticalAlignment(verticalAlignment),
  m_textColor(textColor),
  m_backgroundColor(backgroundColor)
{
}

ExpressionLayout * ExpressionView::expressionLayout() const {
  return m_expressionLayout;
}

void ExpressionView::setExpression(ExpressionLayout * expressionLayout) {
  m_expressionLayout = expressionLayout;
  markRectAsDirty(bounds());
}

void ExpressionView::setBackgroundColor(KDColor backgroundColor) {
  if (m_backgroundColor != backgroundColor) {
    m_backgroundColor = backgroundColor;
    markRectAsDirty(bounds());
  }
}

void ExpressionView::setTextColor(KDColor textColor) {
  if (textColor != m_textColor) {
    m_textColor = textColor;
    markRectAsDirty(bounds());
  }
}

void ExpressionView::setAlignment(float horizontalAlignment, float verticalAlignment) {
  m_horizontalAlignment = horizontalAlignment;
  m_verticalAlignment = verticalAlignment;
  markRectAsDirty(bounds());
}

KDSize ExpressionView::minimalSizeForOptimalDisplay() const {
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
    KDPoint origin(m_horizontalAlignment*(m_frame.width() - expressionSize.width()),
      0.5f*(m_frame.height() - expressionSize.height()));
    m_expressionLayout->draw(ctx, origin, m_textColor, m_backgroundColor);
  }
}
