#include <escher/switch_menu_list_cell.h>

SwitchMenuListCell::SwitchMenuListCell(char * label) :
  MenuListCell(label),
  m_accessoryView(SwitchView())
{
}

View * SwitchMenuListCell::accessoryView() const {
  return (View *)&m_accessoryView;
}
