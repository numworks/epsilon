#include <assert.h>
#include <escher/buffer_table_cell_with_message.h>
#include <escher/palette.h>

namespace Escher {

BufferTableCellWithMessage::BufferTableCellWithMessage(
    I18n::Message subLabelMessage)
    : BufferTableCell(),
      m_subLabelView(KDFont::Size::Small, subLabelMessage,
                     KDContext::k_alignLeft, KDContext::k_alignCenter,
                     Palette::GrayDark, KDColorWhite) {}

void BufferTableCellWithMessage::setHighlighted(bool highlight) {
  BufferTableCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight ? Palette::Select : KDColorWhite;
  m_subLabelView.setBackgroundColor(backgroundColor);
}

void BufferTableCellWithMessage::setSubLabelMessage(I18n::Message text) {
  m_subLabelView.setMessage(text);
}

}  // namespace Escher
