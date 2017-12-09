#include <escher/highlight_cell.h>

void HighlightCell::setHighlighted(bool highlight) {
  if (m_highlighted != highlight) {
    m_highlighted = highlight;
    reloadCell();
  }
}

void HighlightCell::reloadCell() {
  markRectAsDirty(bounds());
}
