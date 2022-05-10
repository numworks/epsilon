#include <escher/message_table_cell_with_chevron_and_message.h>
#include <escher/palette.h>

namespace Escher {

MessageTableCellWithChevronAndMessage::MessageTableCellWithChevronAndMessage(I18n::Message message, I18n::Message subtitle) :
  MessageTableCellWithChevron(message),
  m_subtitleView(KDFont::SmallFont, subtitle, KDContext::k_alignRight, KDContext::k_alignCenter, Palette::GrayDark)
{
}

void MessageTableCellWithChevronAndMessage::setHighlighted(bool highlight) {
  MessageTableCellWithChevron::setHighlighted(highlight);
  KDColor backgroundColor = isHighlighted()? Palette::Select : KDColorWhite;
  m_subtitleView.setBackgroundColor(backgroundColor);
}

void MessageTableCellWithChevronAndMessage::setSubtitle(I18n::Message text) {
  m_subtitleView.setMessage(text);
  reloadCell();
  layoutSubviews();
}

}
