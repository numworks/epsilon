#include "scrollable_exact_approximate_expressions_cell.h"
#include <assert.h>
using namespace Poincare;

namespace Shared {

ScrollableExactApproximateExpressionsCell::ScrollableExactApproximateExpressionsCell(Responder * parentResponder) :
  Responder(parentResponder),
  m_view(this)
{
}

void ScrollableExactApproximateExpressionsCell::setHighlighted(bool highlight) {
  m_view.evenOddCell()->setHighlighted(highlight);
}
void ScrollableExactApproximateExpressionsCell::setEven(bool even) {
  m_view.evenOddCell()->setEven(even);
}
void ScrollableExactApproximateExpressionsCell::reloadCell() {
  m_view.evenOddCell()->reloadCell();
}

void ScrollableExactApproximateExpressionsCell::reloadScroll() {
  m_view.reloadScroll();
}

void ScrollableExactApproximateExpressionsCell::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_view);
}

int ScrollableExactApproximateExpressionsCell::numberOfSubviews() const {
  return 1;
}

View * ScrollableExactApproximateExpressionsCell::subviewAtIndex(int index) {
  return &m_view;
}

void ScrollableExactApproximateExpressionsCell::layoutSubviews() {
  m_view.setFrame(bounds());
}

}
