#include <escher/highlight_cell.h>

HighlightCell::HighlightCell() :
  View(),
  m_highlighted(false)
{
}

void HighlightCell::setHighlighted(bool highlight) {
  if (m_highlighted != highlight) {
    m_highlighted = highlight;
    reloadCell();
  }
}

void HighlightCell::reloadCell() {
  markRectAsDirty(bounds());
}
