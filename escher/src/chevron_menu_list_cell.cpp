#include <escher/chevron_menu_list_cell.h>

ChevronMenuListCell::ChevronMenuListCell(char * label) :
  MenuListCell(label),
  m_contentView(ChevronView())
{
}

View * ChevronMenuListCell::contentView() const {
  return (View *)&m_contentView;
}

void ChevronMenuListCell::reloadCell() {
  MenuListCell::reloadCell();
  m_contentView.setHighlighted(isHighlighted());
}