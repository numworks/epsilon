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

bool HighlightCell::isHighlighted() const {
  return m_highlighted;
}

void HighlightCell::reloadCell() {
  markRectAsDirty(bounds());
}
