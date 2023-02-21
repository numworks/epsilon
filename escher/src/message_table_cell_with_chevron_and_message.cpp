#include <escher/message_table_cell_with_chevron_and_message.h>
#include <escher/palette.h>

namespace Escher {

MessageTableCellWithChevronAndMessage::MessageTableCellWithChevronAndMessage(
    I18n::Message message, I18n::Message subtitle)
    : MessageTableCellWithChevron(message),
      m_subtitleView(KDFont::Size::Small, subtitle, KDContext::k_alignRight,
                     KDContext::k_alignCenter, Palette::GrayDark) {}

void MessageTableCellWithChevronAndMessage::setHighlighted(bool highlight) {
  MessageTableCellWithChevron::setHighlighted(highlight);
  m_subtitleView.setBackgroundColor(defaultBackgroundColor());
}

void MessageTableCellWithChevronAndMessage::setSubtitle(I18n::Message text) {
  m_subtitleView.setMessage(text);
  reloadCell();
  layoutSubviews();
}

}  // namespace Escher
