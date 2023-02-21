#include <escher/message_table_cell_with_message.h>
#include <escher/palette.h>
#include <string.h>

namespace Escher {

MessageTableCellWithMessage::MessageTableCellWithMessage(
    I18n::Message message, I18n::Message subLabelMessage)
    : MessageTableCell(message),
      m_subLabelView(KDFont::Size::Small, subLabelMessage,
                     KDContext::k_alignLeft, KDContext::k_alignCenter,
                     Palette::GrayDark) {}

void MessageTableCellWithMessage::setSubLabelMessage(I18n::Message textBody) {
  m_subLabelView.setMessage(textBody);
  reloadCell();
}

const View* MessageTableCellWithMessage::subLabelView() const {
  if (m_subLabelView.text()[0] == 0) {
    return nullptr;
  }
  return &m_subLabelView;
}

void MessageTableCellWithMessage::setHighlighted(bool highlight) {
  MessageTableCell::setHighlighted(highlight);
  m_subLabelView.setBackgroundColor(defaultBackgroundColor());
}

}  // namespace Escher
