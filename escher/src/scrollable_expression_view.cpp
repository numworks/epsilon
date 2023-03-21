#include <assert.h>
#include <escher/menu_cell.h>
#include <escher/metric.h>
#include <escher/scrollable_expression_view.h>
#include <poincare/layout.h>

namespace Escher {

ScrollableExpressionView::ScrollableExpressionView(Responder* parentResponder,
                                                   KDCoordinate leftRightMargin,
                                                   KDCoordinate topBottomMargin,
                                                   KDGlyph::Format format)
    : ScrollableView(parentResponder, &m_expressionView, this),
      m_expressionView(format) {
  decorator()->setFont(format.style.font);
  setMargins(topBottomMargin, leftRightMargin, topBottomMargin,
             leftRightMargin);
  setBackgroundColor(format.style.backgroundColor);
}

ScrollableExpressionView::ScrollableExpressionView(KDGlyph::Format format)
    : ScrollableExpressionView(nullptr, 0, 0, format) {}

Poincare::Layout ScrollableExpressionView::layout() const {
  return m_expressionView.layout();
}

void ScrollableExpressionView::setLayout(Poincare::Layout layout) {
  m_expressionView.setLayout(layout);
}

void ScrollableExpressionView::setBackgroundColor(KDColor backgroundColor) {
  m_expressionView.setBackgroundColor(backgroundColor);
  ScrollableView::setBackgroundColor(backgroundColor);
}

void ScrollableExpressionView::setExpressionBackgroundColor(
    KDColor backgroundColor) {
  m_expressionView.setBackgroundColor(backgroundColor);
}

void ScrollableExpressionView::setHighlighted(bool highlighted) {
  m_expressionView.setHighlighted(highlighted);
  ScrollableView::setBackgroundColor(
      AbstractMenuCell::BackgroundColor(highlighted));
}

}  // namespace Escher
