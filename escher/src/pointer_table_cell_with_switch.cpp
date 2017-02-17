#include <escher/pointer_table_cell_with_switch.h>
#include <escher/palette.h>

PointerTableCellWithSwitch::PointerTableCellWithSwitch(char * label) :
  PointerTableCell(label),
  m_accessoryView(SwitchView())
{
}

View * PointerTableCellWithSwitch::accessoryView() const {
  return (View *)&m_accessoryView;
}
