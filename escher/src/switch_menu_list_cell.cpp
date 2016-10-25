#include <escher/switch_menu_list_cell.h>

SwitchMenuListCell::SwitchMenuListCell(char * label) :
  MenuListCell(label),
  m_contentView(SwitchView())
{
}

View * SwitchMenuListCell::contentView() const {
  return (View *)&m_contentView;
}
