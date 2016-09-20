#include <escher/switch_table_view_cell.h>

SwitchTableViewCell::SwitchTableViewCell(char * label) :
  TableViewCell(label),
  m_contentView(SwitchView())
{
}

View * SwitchTableViewCell::contentView() const {
  return (View *)&m_contentView;
}
