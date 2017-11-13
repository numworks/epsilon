#include <escher/message_table_cell.h>
#include <escher/palette.h>
#include <assert.h>

MessageTableCell::MessageTableCell(I18n::Message label, KDText::FontSize size, Layout layout) :
  TableCell(layout),
  m_messageTextView(size, label, 0, 0.5, KDColorBlack, KDColorWhite)
{
}

View * MessageTableCell::labelView() const {
  return (View *)&m_messageTextView;
}

void MessageTableCell::setHighlighted(bool highlight) {
  HighlightCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Palette::Select : KDColorWhite;
  m_messageTextView.setBackgroundColor(backgroundColor);
}

void MessageTableCell::setMessage(I18n::Message text) {
  m_messageTextView.setMessage(text);
  layoutSubviews();
  markRectAsDirty(bounds());
}

void MessageTableCell::setTextColor(KDColor color) {
  m_messageTextView.setTextColor(color);
}

void MessageTableCell::setMessageFontSize(KDText::FontSize fontSize) {
  m_messageTextView.setFontSize(fontSize);
  layoutSubviews();
}
