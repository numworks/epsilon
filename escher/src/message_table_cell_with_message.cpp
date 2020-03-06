#include <escher/message_table_cell_with_message.h>
#include <escher/palette.h>
#include <string.h>

MessageTableCellWithMessage::MessageTableCellWithMessage(I18n::Message message, Layout layout) :
  MessageTableCell(message, KDFont::SmallFont, layout),
  m_accessoryView(KDFont::SmallFont, (I18n::Message)0, 0.0f, 0.5f)
{
  if (layout != Layout::Vertical) {
    m_accessoryView.setAlignment(1.0f, 0.5f);
  }
}

void MessageTableCellWithMessage::setAccessoryMessage(I18n::Message textBody) {
  m_accessoryView.setMessage(textBody);
  reloadCell();
}

View * MessageTableCellWithMessage::accessoryView() const {
  if (strlen(m_accessoryView.text()) == 0) {
    return nullptr;
  }
  return (View *)&m_accessoryView;
}

void MessageTableCellWithMessage::setHighlighted(bool highlight) {
  MessageTableCell::setHighlighted(highlight);
  KDColor backgroundColor = isHighlighted()? Palette::Select : KDColorWhite;
  m_accessoryView.setBackgroundColor(backgroundColor);
}

void MessageTableCellWithMessage::setTextColor(KDColor color) {
  m_accessoryView.setTextColor(color);
  MessageTableCell::setTextColor(color);
}

void MessageTableCellWithMessage::setAccessoryTextColor(KDColor color) {
  m_accessoryView.setTextColor(color);
}
