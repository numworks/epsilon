#include <escher/pointer_table_cell_with_switch.h>
#include <escher/palette.h>

PointerTableCellWithSwitch::PointerTableCellWithSwitch(I18n::Message message) :
  PointerTableCell(message),
  m_accessoryView(SwitchView())
{
}

View * PointerTableCellWithSwitch::accessoryView() const {
  return (View *)&m_accessoryView;
}
