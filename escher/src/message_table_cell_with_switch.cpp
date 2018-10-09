#include <escher/message_table_cell_with_switch.h>
#include <escher/palette.h>

MessageTableCellWithSwitch::MessageTableCellWithSwitch(I18n::Message message, const KDFont * font) :
  MessageTableCell(message, font),
  m_accessoryView()
{
}

View * MessageTableCellWithSwitch::accessoryView() const {
  return (View *)&m_accessoryView;
}
