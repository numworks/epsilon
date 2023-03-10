#include <escher/message_table_cell_with_buffer.h>
#include <escher/palette.h>

namespace Escher {

MessageTableCellWithBuffer::MessageTableCellWithBuffer(I18n::Message message)
    : MessageTableCell(message), m_subLabelView() {
  m_subLabelView.defaultInitialization(CellWidget::Type::SubLabel);
}

void MessageTableCellWithBuffer::setSubLabelText(const char* textBody) {
  m_subLabelView.setText(textBody);
  layoutSubviews();
}

const char* MessageTableCellWithBuffer::subLabelText() {
  return m_subLabelView.text();
}

void MessageTableCellWithBuffer::setHighlighted(bool highlight) {
  MessageTableCell::setHighlighted(highlight);
  m_subLabelView.setBackgroundColor(defaultBackgroundColor());
}

}  // namespace Escher
