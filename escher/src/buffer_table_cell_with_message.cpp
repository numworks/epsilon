#include <assert.h>
#include <escher/buffer_table_cell_with_message.h>
#include <escher/palette.h>

namespace Escher {

BufferTableCellWithMessage::BufferTableCellWithMessage(
    I18n::Message subLabelMessage)
    : BufferTableCell(), m_subLabelView(subLabelMessage) {
  m_subLabelView.defaultInitialization(CellWidget::Type::SubLabel);
}

void BufferTableCellWithMessage::setHighlighted(bool highlight) {
  BufferTableCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight ? Palette::Select : KDColorWhite;
  m_subLabelView.setBackgroundColor(backgroundColor);
}

void BufferTableCellWithMessage::setSubLabelMessage(I18n::Message text) {
  m_subLabelView.setMessage(text);
}

}  // namespace Escher
