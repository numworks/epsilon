#include <escher/message_table_cell_with_chevron.h>

MessageTableCellWithChevron::MessageTableCellWithChevron(I18n::Message message, KDText::FontSize size) :
  MessageTableCell(message, size),
  m_accessoryView()
{
}

View * MessageTableCellWithChevron::accessoryView() const {
  return (View *)&m_accessoryView;
}

