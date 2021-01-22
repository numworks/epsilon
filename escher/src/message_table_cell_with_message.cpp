#include <escher/message_table_cell_with_message.h>
#include <escher/palette.h>
#include <string.h>

namespace Escher {

MessageTableCellWithMessage::MessageTableCellWithMessage(I18n::Message message) :
  MessageTableCell(message),
  m_subLabelView(KDFont::SmallFont, (I18n::Message)0, 0.0f, 0.5f, Palette::GrayDark)
{
}

void MessageTableCellWithMessage::setSubLabelMessage(I18n::Message textBody) {
  m_subLabelView.setMessage(textBody);
  reloadCell();
}

View * MessageTableCellWithMessage::subLabelView() const {
  if (strlen(m_subLabelView.text()) == 0) {
    return nullptr;
  }
  return (View *)&m_subLabelView;
}

void MessageTableCellWithMessage::setHighlighted(bool highlight) {
  MessageTableCell::setHighlighted(highlight);
  KDColor backgroundColor = isHighlighted()? Palette::Select : KDColorWhite;
  m_subLabelView.setBackgroundColor(backgroundColor);
}

}
