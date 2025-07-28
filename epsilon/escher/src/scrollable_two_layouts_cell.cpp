#include <assert.h>
#include <escher/container.h>
#include <escher/scrollable_two_layouts_cell.h>

using namespace Poincare;

namespace Escher {

ScrollableTwoLayoutsCell::ScrollableTwoLayoutsCell(Responder* parentResponder,
                                                   float horizontalAlignment,
                                                   KDFont::Size font)
    : Responder(parentResponder), m_view(this, horizontalAlignment, font) {}

void ScrollableTwoLayoutsCell::setEven(bool even) {
  EvenOddCell::setEven(even);
  m_view.setBackgroundColor(backgroundColor());
  m_view.evenOddCell()->setEven(even);
}

void ScrollableTwoLayoutsCell::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::HasBecomeFirst) {
    reinitSelection();
    App::app()->setFirstResponder(&m_view);
  } else {
    Responder::handleResponderChainEvent(event);
  }
}

void ScrollableTwoLayoutsCell::reinitSelection() {
  ScrollableTwoLayoutsView::SubviewPosition selectedSubview =
      m_view.displayCenter() ? ScrollableTwoLayoutsView::SubviewPosition::Center
                             : ScrollableTwoLayoutsView::SubviewPosition::Right;
  m_view.setSelectedSubviewPosition(selectedSubview);
  m_view.reloadScroll();
}

}  // namespace Escher
