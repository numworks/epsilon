#include <escher/pointer_table_cell_with_buffer.h>
#include <escher/palette.h>

PointerTableCellWithBuffer::PointerTableCellWithBuffer(I18n::Message message) :
  PointerTableCell(message),
  m_accessoryView(BufferTextView(KDText::FontSize::Large, 1.0f, 0.5f))
{
}

void PointerTableCellWithBuffer::setAccessoryText(const char * textBody) {
  m_accessoryView.setText(textBody);
  reloadCell();
}

const char * PointerTableCellWithBuffer::accessoryText() {
  return m_accessoryView.text();
}

View * PointerTableCellWithBuffer::accessoryView() const {
  return (View *)&m_accessoryView;
}

void PointerTableCellWithBuffer::setHighlighted(bool highlight) {
  PointerTableCell::setHighlighted(highlight);
  KDColor backgroundColor = isHighlighted()? Palette::Select : KDColorWhite;
  m_accessoryView.setBackgroundColor(backgroundColor);
}

void PointerTableCellWithBuffer::setTextColor(KDColor color) {
  m_accessoryView.setTextColor(color);
  PointerTableCell::setTextColor(color);
}
