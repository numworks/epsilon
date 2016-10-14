#include <escher/switch_list_view_cell.h>

SwitchListViewCell::SwitchListViewCell(char * label) :
  ListViewCell(label),
  m_contentView(SwitchView())
{
}

View * SwitchListViewCell::contentView() const {
  return (View *)&m_contentView;
}
