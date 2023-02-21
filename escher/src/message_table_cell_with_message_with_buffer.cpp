#include <escher/message_table_cell_with_message_with_buffer.h>
#include <escher/palette.h>

namespace Escher {

MessageTableCellWithMessageWithBuffer::MessageTableCellWithMessageWithBuffer(
    I18n::Message message)
    : MessageTableCellWithMessage(message),
      m_accessoryView(KDFont::Size::Large, 1.0f, 0.5f, KDColorBlack) {}

void MessageTableCellWithMessageWithBuffer::setAccessoryText(
    const char* textBody) {
  m_accessoryView.setText(textBody);
  layoutSubviews();
}

void MessageTableCellWithMessageWithBuffer::setHighlighted(bool highlight) {
  MessageTableCellWithMessage::setHighlighted(highlight);
  m_accessoryView.setBackgroundColor(defaultBackgroundColor());
}

}  // namespace Escher
