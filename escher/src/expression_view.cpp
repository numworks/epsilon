#include <escher/expression_view.h>
#include <escher/palette.h>
#include <algorithm>

using namespace Poincare;

ExpressionView::ExpressionView(float horizontalAlignment, float verticalAlignment,
    KDColor textColor, KDColor backgroundColor, Poincare::Layout * selectionStart, Poincare::Layout * selectionEnd ) :
  m_layout(),
  m_textColor(textColor),
  m_backgroundColor(backgroundColor),
  m_selectionStart(selectionStart),
  m_selectionEnd(selectionEnd),
  m_horizontalAlignment(horizontalAlignment),
  m_verticalAlignment(verticalAlignment),
  m_horizontalMargin(0)
{
}

bool ExpressionView::setLayout(Layout layoutR) {
  /* TODO: this would avoid some useless redrawing. However, when we call
   * setLayout after raising an Exception that led to erase all
   * Poincare::TreePool, accessing m_layout will result in an ACCESS ERROR.
   * How do we avoid that? */
  /*if (m_layout.isIdenticalTo(layoutR)) {
    return false;
  }*/
  m_layout = layoutR;
  markRectAsDirty(bounds());
  return true;
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

int ExpressionView::numberOfLayouts() const {
  return m_layout.numberOfDescendants(true);
}

KDSize ExpressionView::minimalSizeForOptimalDisplay() const {
  if (m_layout.isUninitialized()) {
    return KDSizeZero;
  }
  KDSize expressionSize = m_layout.layoutSize();
  return KDSize(expressionSize.width() + 2*m_horizontalMargin, expressionSize.height());
}

KDPoint ExpressionView::drawingOrigin() const {
  KDSize expressionSize = m_layout.layoutSize();
  return KDPoint(m_horizontalMargin + m_horizontalAlignment*(m_frame.width() - 2*m_horizontalMargin - expressionSize.width()), std::max<KDCoordinate>(0, m_verticalAlignment*(m_frame.height() - expressionSize.height())));
}

KDPoint ExpressionView::absoluteDrawingOrigin() const {
  return drawingOrigin().translatedBy(m_frame.topLeft());
}

void ExpressionView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, m_backgroundColor);
  if (!m_layout.isUninitialized()) {
    m_layout.draw(ctx, drawingOrigin(), m_textColor, m_backgroundColor, m_selectionStart, m_selectionEnd, Palette::Select);
  }
}
