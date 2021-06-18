#include "message_table_cell_with_editable_text_with_separator.h"

namespace Settings {

MessageTableCellWithEditableTextWithSeparator::MessageTableCellWithEditableTextWithSeparator(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * textFieldDelegate, I18n::Message message) :
  HighlightCell(),
  m_cell(parentResponder, inputEventHandlerDelegate, textFieldDelegate, message)
{
}

void MessageTableCellWithEditableTextWithSeparator::setHighlighted(bool highlight) {
  m_cell.setHighlighted(highlight);
  HighlightCell::setHighlighted(highlight);
}

void MessageTableCellWithEditableTextWithSeparator::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(KDRect(0, 0, bounds().width(), k_separatorThickness), Palette::ListCellBorder);
  ctx->fillRect(KDRect(0, k_separatorThickness, bounds().width(), k_margin-k_separatorThickness), Palette::BackgroundApps);
}

int MessageTableCellWithEditableTextWithSeparator::numberOfSubviews() const {
  return 1;
}

View * MessageTableCellWithEditableTextWithSeparator::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_cell;
}

void MessageTableCellWithEditableTextWithSeparator::layoutSubviews() {
  m_cell.setFrame(KDRect(0, k_margin, bounds().width(), bounds().height()-k_margin));
}

}
