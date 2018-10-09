#include <escher/message_table_cell_with_chevron.h>

MessageTableCellWithChevron::MessageTableCellWithChevron(I18n::Message message, const KDFont * font) :
  MessageTableCell(message, font),
  m_accessoryView()
{
}

View * MessageTableCellWithChevron::accessoryView() const {
  return (View *)&m_accessoryView;
}

