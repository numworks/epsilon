#include <escher/message_table_cell.h>
#include <escher/palette.h>
#include <assert.h>

namespace Escher {

MessageTableCell::MessageTableCell(I18n::Message label) :
  TableCell(),
  m_messageTextView(KDFont::LargeFont, label, 0, 0.5, KDColorBlack, KDColorWhite),
  m_backgroundColor(KDColorWhite)
{
}

View * MessageTableCell::labelView() const {
  return (View *)&m_messageTextView;
}

void MessageTableCell::setHighlighted(bool highlight) {
  HighlightCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Palette::Select : m_backgroundColor;
  m_messageTextView.setBackgroundColor(backgroundColor);
}

void MessageTableCell::setMessage(I18n::Message text) {
  m_messageTextView.setMessage(text);
}

void MessageTableCell::setBackgroundColor(KDColor color) {
  m_backgroundColor = color;
  m_messageTextView.setBackgroundColor(color);
}

}
