#include "pointer_text_table_cell.h"
#include <escher/buffer_text_view.h>
#include <escher/palette.h>
#include <assert.h>

PointerTextTableCell::PointerTextTableCell(const char * text, const KDFont * font, Layout layout) :
  TableCell(layout),
  m_pointerTextView(font, text, 0, 0.5, KDColorBlack, KDColorWhite)
{
}

View * PointerTextTableCell::labelView() const {
  return (View *)&m_pointerTextView;
}

const char * PointerTextTableCell::text() const {
  return m_pointerTextView.text();
}

void PointerTextTableCell::setHighlighted(bool highlight) {
  HighlightCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Palette::ListCellBackgroundSelected : Palette::ListCellBackground;
  m_pointerTextView.setBackgroundColor(backgroundColor);
}

void PointerTextTableCell::setText(const char * text) {
  m_pointerTextView.setText(text);
  layoutSubviews();
}

void PointerTextTableCell::setTextColor(KDColor color) {
  m_pointerTextView.setTextColor(color);
}

void PointerTextTableCell::setTextFont(const KDFont * font) {
  m_pointerTextView.setFont(font);
  layoutSubviews();
}
