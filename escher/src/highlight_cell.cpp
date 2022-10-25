#include <escher/highlight_cell.h>

namespace Escher {

HighlightCell::HighlightCell() :
  View(),
  m_state(State::Visible)
{
}

void HighlightCell::setHighlighted(bool highlight) {
  // An hidden cell might be deselected but not selected
  assert(m_state != State::Hidden || !highlight);
  // WARNING : if state = hidden, the cell will not be reload
  if (isHighlighted() != highlight) {
    HighlightCell::setHighlightedWithoutReload(highlight);
    reloadCell();
  }
}

void HighlightCell::setHighlightedWithoutReload(bool highlight) {
  m_state = highlight ? State::Highlighted : State::Visible;
}

void HighlightCell::reloadCell() {
  markRectAsDirty(bounds());
}

}
