#include <escher/highlight_cell.h>

namespace Escher {

HighlightCell::HighlightCell() : View(), m_state(State::Visible) {}

void HighlightCell::setVisible(bool visible) {
  if (m_state == State::Hidden && visible) {
    m_state = State::Visible;
  } else if (!visible) {
    m_state = State::Hidden;
  }
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

}  // namespace Escher
