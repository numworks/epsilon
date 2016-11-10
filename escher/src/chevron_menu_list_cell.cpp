#include <escher/chevron_menu_list_cell.h>

ChevronMenuListCell::ChevronMenuListCell(char * label) :
  MenuListCell(label),
  m_accessoryView(ChevronView())
{
}

View * ChevronMenuListCell::accessoryView() const {
  return (View *)&m_accessoryView;
}

void ChevronMenuListCell::reloadCell() {
  MenuListCell::reloadCell();
  m_accessoryView.setHighlighted(isHighlighted());
}