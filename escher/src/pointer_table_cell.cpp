#include <escher/pointer_table_cell.h>
#include <escher/palette.h>
#include <assert.h>

PointerTableCell::PointerTableCell(I18n::Message label, KDText::FontSize size, Layout layout) :
  TableCell(layout),
  m_pointerTextView(PointerTextView(size, label, 0, 0.5, KDColorBlack, KDColorWhite))
{
}

View * PointerTableCell::labelView() const {
  return (View *)&m_pointerTextView;
}

void PointerTableCell::setHighlighted(bool highlight) {
  HighlightCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Palette::Select : KDColorWhite;
  m_pointerTextView.setBackgroundColor(backgroundColor);
}

void PointerTableCell::setMessage(I18n::Message text) {
  m_pointerTextView.setMessage(text);
  layoutSubviews();
}

void PointerTableCell::setTextColor(KDColor color) {
  m_pointerTextView.setTextColor(color);
}
