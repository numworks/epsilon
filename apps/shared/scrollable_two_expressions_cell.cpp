#include "scrollable_two_expressions_cell.h"

#include <assert.h>
#include <escher/container.h>

using namespace Escher;
using namespace Poincare;

namespace Shared {

ScrollableTwoExpressionsCell::ScrollableTwoExpressionsCell(
    Responder* parentResponder, float horizontalAlignment, KDFont::Size font)
    : Responder(parentResponder), m_view(this, horizontalAlignment, font) {}

void ScrollableTwoExpressionsCell::setLayouts(
    Poincare::Layout exactLayout, Poincare::Layout approximateLayout) {
  m_view.setLayouts(Layout(), exactLayout, approximateLayout);
}

void ScrollableTwoExpressionsCell::setHighlighted(bool highlight) {
  m_view.evenOddCell()->setHighlighted(highlight);
}

void ScrollableTwoExpressionsCell::setEven(bool even) {
  EvenOddCell::setEven(even);
  m_view.setBackgroundColor(backgroundColor());
  m_view.evenOddCell()->setEven(even);
}

void ScrollableTwoExpressionsCell::reloadScroll() { m_view.reloadScroll(); }

void ScrollableTwoExpressionsCell::didBecomeFirstResponder() {
  reinitSelection();
  Container::activeApp()->setFirstResponder(&m_view);
}

void ScrollableTwoExpressionsCell::reinitSelection() {
  ScrollableTwoExpressionsView::SubviewPosition selectedSubview =
      m_view.displayCenter()
          ? ScrollableTwoExpressionsView::SubviewPosition::Center
          : ScrollableTwoExpressionsView::SubviewPosition::Right;
  m_view.setSelectedSubviewPosition(selectedSubview);
  reloadScroll();
}

int ScrollableTwoExpressionsCell::numberOfSubviews() const { return 1; }

View* ScrollableTwoExpressionsCell::subviewAtIndex(int index) {
  return &m_view;
}

void ScrollableTwoExpressionsCell::layoutSubviews(bool force) {
  setChildFrame(&m_view, bounds(), force);
}

}  // namespace Shared
