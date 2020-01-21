#include "scrollable_exact_approximate_expressions_cell.h"
#include <assert.h>
using namespace Poincare;

namespace Shared {

ScrollableExactApproximateExpressionsCell::ScrollableExactApproximateExpressionsCell(Responder * parentResponder) :
  Responder(parentResponder),
  m_view(this)
{
}

void ScrollableExactApproximateExpressionsCell::setLayouts(Poincare::Layout exactLayout, Poincare::Layout approximateLayout) {
  m_view.setLayouts(Layout(), exactLayout, approximateLayout);
}

void ScrollableExactApproximateExpressionsCell::setHighlighted(bool highlight) {
  m_view.evenOddCell()->setHighlighted(highlight);
}

void ScrollableExactApproximateExpressionsCell::setEven(bool even) {
  EvenOddCell::setEven(even);
  m_view.setBackgroundColor(backgroundColor());
  m_view.evenOddCell()->setEven(even);
}

void ScrollableExactApproximateExpressionsCell::reloadScroll() {
  m_view.reloadScroll();
}

void ScrollableExactApproximateExpressionsCell::didBecomeFirstResponder() {
  reinitSelection();
  Container::activeApp()->setFirstResponder(&m_view);
}

void ScrollableExactApproximateExpressionsCell::reinitSelection() {
  ScrollableExactApproximateExpressionsView::SubviewPosition selectedSubview = m_view.displayCenter() ? ScrollableExactApproximateExpressionsView::SubviewPosition::Center : ScrollableExactApproximateExpressionsView::SubviewPosition::Right;
  m_view.setSelectedSubviewPosition(selectedSubview);
  reloadScroll();
}

int ScrollableExactApproximateExpressionsCell::numberOfSubviews() const {
  return 1;
}

View * ScrollableExactApproximateExpressionsCell::subviewAtIndex(int index) {
  return &m_view;
}

void ScrollableExactApproximateExpressionsCell::layoutSubviews(bool force) {
  m_view.setFrame(bounds(), force);
}

}
