#include <assert.h>
#include <escher/menu_cell.h>
#include <escher/metric.h>
#include <escher/scrollable_layout_view.h>
#include <poincare/layout.h>

namespace Escher {

ScrollableLayoutView::ScrollableLayoutView(Responder* parentResponder,
                                           KDCoordinate leftRightMargin,
                                           KDCoordinate topBottomMargin,
                                           TextFormat format)
    : ScrollableView(parentResponder, &m_layoutView, this),
      m_layoutView(format) {
  decorator()->setFont(format.style.font);
  setMargins(
      {{leftRightMargin, leftRightMargin}, {topBottomMargin, topBottomMargin}});
  ScrollableView::setBackgroundColor(format.style.backgroundColor);
}

ScrollableLayoutView::ScrollableLayoutView(TextFormat format)
    : ScrollableLayoutView(nullptr, 0, 0, format) {}

const Poincare::Layout& ScrollableLayoutView::layout() const {
  return m_layoutView.layout();
}

void ScrollableLayoutView::setLayout(const Poincare::Layout& layout) {
  m_layoutView.setLayout(layout);
}

void ScrollableLayoutView::setBackgroundColor(ThemeComponent backgroundColor) {
  m_layoutView.setBackgroundColor(backgroundColor);
  ScrollableView::setBackgroundColor(backgroundColor);
}

void ScrollableLayoutView::setExpressionBackgroundColor(
    ThemeComponent backgroundColor) {
  m_layoutView.setBackgroundColor(backgroundColor);
}

void ScrollableLayoutView::setHighlighted(bool highlighted) {
  m_layoutView.setHighlighted(highlighted);
  ScrollableView::setBackgroundColor(
      AbstractMenuCell::BackgroundColor(highlighted));
}

}  // namespace Escher
