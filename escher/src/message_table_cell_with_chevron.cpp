#include <escher/message_table_cell_with_chevron.h>

namespace Escher {

MessageTableCellWithChevron::MessageTableCellWithChevron(I18n::Message message) :
  MessageTableCell(message),
  m_accessoryView()
{
}

View * MessageTableCellWithChevron::accessoryView() const {
  return (View *)&m_accessoryView;
}

}
