#include "layout_cell_with_sub_message.h"

namespace Probability {

LayoutCellWithSubMessage::LayoutCellWithSubMessage() : m_messageTextView(KDFont::SmallFont) {
  m_messageTextView.setTextColor(Escher::Palette::GrayDark);
}

void LayoutCellWithSubMessage::setSubLabelMessage(I18n::Message message) {
  m_messageTextView.setMessage(message);
}

void LayoutCellWithSubMessage::setHighlighted(bool highlight) {
  LayoutCell::setHighlighted(highlight);
  m_messageTextView.setBackgroundColor(currentBackgroundColor());
}

}  // namespace Probability
