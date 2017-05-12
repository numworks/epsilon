#include <escher/message_table_cell_with_switch.h>
#include <escher/palette.h>

MessageTableCellWithSwitch::MessageTableCellWithSwitch(I18n::Message message, KDText::FontSize size) :
  MessageTableCell(message, size),
  m_accessoryView()
{
}

View * MessageTableCellWithSwitch::accessoryView() const {
  return (View *)&m_accessoryView;
}
