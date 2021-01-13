#include <escher/message_table_cell_with_switch.h>
#include <escher/palette.h>

namespace Escher {

MessageTableCellWithSwitch::MessageTableCellWithSwitch(I18n::Message message) :
  MessageTableCell(message),
  m_accessoryView()
{
}

View * MessageTableCellWithSwitch::accessoryView() const {
  return (View *)&m_accessoryView;
}

}
