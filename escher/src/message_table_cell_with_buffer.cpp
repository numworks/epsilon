#include <escher/message_table_cell_with_buffer.h>
#include <escher/palette.h>

namespace Escher {

MessageTableCellWithBuffer::MessageTableCellWithBuffer(I18n::Message message) :
  MessageTableCell(message),
  m_subLabelView(KDFont::SmallFont, 1.0f, 0.5f, Palette::GrayDark)
{
}

void MessageTableCellWithBuffer::setSubLabelText(const char * textBody) {
  m_subLabelView.setText(textBody);
  layoutSubviews();
}

const char * MessageTableCellWithBuffer::subLabelText() {
  return m_subLabelView.text();
}

View * MessageTableCellWithBuffer::subLabelView() const {
  return (View *)&m_subLabelView;
}

void MessageTableCellWithBuffer::setHighlighted(bool highlight) {
  MessageTableCell::setHighlighted(highlight);
  KDColor backgroundColor = isHighlighted()? Palette::Select : KDColorWhite;
  m_subLabelView.setBackgroundColor(backgroundColor);
}

// void MessageTableCellWithBuffer::setTextColor(KDColor color) {
//   m_subLabelView.setTextColor(color);
//   MessageTableCell::setTextColor(color);
// }

}