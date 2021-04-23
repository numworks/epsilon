#include <escher/message_table_cell_with_editable_text_with_message.h>
#include <escher/palette.h>

namespace Escher {

void MessageTableCellWithEditableTextWithMessage::setSubLabelMessage(I18n::Message textBody) {
  m_subLabelView.setMessage(textBody);
  reloadCell();
}

void MessageTableCellWithEditableTextWithMessage::setHighlighted(bool highlight) {
  MessageTableCellWithEditableText::setHighlighted(highlight);
  KDColor backgroundColor = isHighlighted()? Palette::Select : KDColorWhite;
  m_subLabelView.setBackgroundColor(backgroundColor);
}

}
