#include <escher/message_table_cell_with_buffer.h>
#include <escher/palette.h>

MessageTableCellWithBuffer::MessageTableCellWithBuffer(I18n::Message message, const KDFont * font, const KDFont * accessoryFont, KDColor accessoryTextColor) :
  MessageTableCell(message, font),
  m_accessoryView(accessoryFont, 1.0f, 0.5f, accessoryTextColor)
{
}

void MessageTableCellWithBuffer::setAccessoryText(const char * textBody) {
  m_accessoryView.setText(textBody);
  layoutSubviews();
}

const char * MessageTableCellWithBuffer::accessoryText() {
  return m_accessoryView.text();
}

View * MessageTableCellWithBuffer::accessoryView() const {
  return (View *)&m_accessoryView;
}

void MessageTableCellWithBuffer::setHighlighted(bool highlight) {
  MessageTableCell::setHighlighted(highlight);
  KDColor backgroundColor = isHighlighted()? Palette::Select : KDColorWhite;
  m_accessoryView.setBackgroundColor(backgroundColor);
}

void MessageTableCellWithBuffer::setTextColor(KDColor color) {
  m_accessoryView.setTextColor(color);
  MessageTableCell::setTextColor(color);
}
