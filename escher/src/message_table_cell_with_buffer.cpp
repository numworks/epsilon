#include <escher/message_table_cell_with_buffer.h>
#include <escher/palette.h>

MessageTableCellWithBuffer::MessageTableCellWithBuffer(I18n::Message message, KDText::FontSize fontSize, KDText::FontSize accessoryFontSize, KDColor accessoryTextColor) :
  MessageTableCell(message, fontSize),
  m_accessoryView(accessoryFontSize, 1.0f, 0.5f, accessoryTextColor)
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

void MessageTableCellWithBuffer::setAccessoryTextColor(KDColor color) {
  m_accessoryView.setTextColor(color);
}

void MessageTableCellWithBuffer::setAccessoryFontSize(KDText::FontSize fontSize) {
  m_accessoryView.setFontSize(fontSize);
}
