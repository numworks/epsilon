#include <escher/message_table_cell_with_chevron_and_message.h>
#include <escher/palette.h>

namespace Escher {

MessageTableCellWithChevronAndMessage::MessageTableCellWithChevronAndMessage() :
  MessageTableCellWithChevron((I18n::Message)0),
  m_subtitleView(KDFont::SmallFont, (I18n::Message)0, 1.0f, 0.5f, Palette::GrayDark)
{
}

View * MessageTableCellWithChevronAndMessage::subLabelView() const {
  return (View *)&m_subtitleView;
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
