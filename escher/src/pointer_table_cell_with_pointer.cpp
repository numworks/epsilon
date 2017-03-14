#include <escher/pointer_table_cell_with_pointer.h>
#include <escher/palette.h>

PointerTableCellWithPointer::PointerTableCellWithPointer(I18n::Message message, Layout layout) :
  PointerTableCell(message, KDText::FontSize::Small, layout),
  m_accessoryView(PointerTextView(KDText::FontSize::Small, (I18n::Message)0, 0.0f, 0.5f))
{
  if (layout != Layout::Vertical) {
    m_accessoryView.setAlignment(1.0f, 0.5f);
  }
}

void PointerTableCellWithPointer::setAccessoryMessage(I18n::Message textBody) {
  m_accessoryView.setMessage(textBody);
  reloadCell();
}

View * PointerTableCellWithPointer::accessoryView() const {
  return (View *)&m_accessoryView;
}

void PointerTableCellWithPointer::setHighlighted(bool highlight) {
  PointerTableCell::setHighlighted(highlight);
  KDColor backgroundColor = isHighlighted()? Palette::Select : KDColorWhite;
  m_accessoryView.setBackgroundColor(backgroundColor);
}

void PointerTableCellWithPointer::setTextColor(KDColor color) {
  m_accessoryView.setTextColor(color);
  PointerTableCell::setTextColor(color);
}

void PointerTableCellWithPointer::setAccessoryTextColor(KDColor color) {
  m_accessoryView.setTextColor(color);
}
