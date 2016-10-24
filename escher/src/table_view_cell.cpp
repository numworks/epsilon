#include <escher/table_view_cell.h>

TableViewCell::TableViewCell() :
  View(),
  m_highlighted(false)
{
}

void TableViewCell::setHighlighted(bool highlight) {
  m_highlighted = highlight;
  reloadCell();
}

bool TableViewCell::isHighlighted() const {
  return m_highlighted;
}

void TableViewCell::reloadCell() {
  markRectAsDirty(bounds());
}
