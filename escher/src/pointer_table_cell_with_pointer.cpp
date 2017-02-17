#include <escher/pointer_table_cell_with_pointer.h>
#include <escher/palette.h>

PointerTableCellWithPointer::PointerTableCellWithPointer(char * label, Layout layout) :
  PointerTableCell(label, KDText::FontSize::Small, layout),
  m_accessoryView(PointerTextView(KDText::FontSize::Small, nullptr, 0.0f, 0.5f))
{
  if (layout != Layout::Vertical) {
    m_accessoryView.setAlignment(1.0f, 0.5f);
  }
}

void PointerTableCellWithPointer::setAccessoryText(const char * textBody) {
  m_accessoryView.setText(textBody);
  reloadCell();
}

const char * PointerTableCellWithPointer::accessoryText() {
  return m_accessoryView.text();
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
