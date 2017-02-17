#include <escher/pointer_table_cell.h>
#include <escher/palette.h>
#include <assert.h>

PointerTableCell::PointerTableCell(char * label, KDText::FontSize size, Layout layout) :
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

void PointerTableCell::setText(const char * text) {
  m_pointerTextView.setText(text);
  layoutSubviews();
}

void PointerTableCell::setTextColor(KDColor color) {
  m_pointerTextView.setTextColor(color);
}
