#include <escher/buffer_table_cell.h>
#include <escher/palette.h>
#include <assert.h>

namespace Escher {

BufferTableCell::BufferTableCell() :
  TableCell(),
  m_labelView(KDFont::LargeFont, 1.0f, 0.5f, Palette::GrayDark),
  m_backgroundColor(KDColorWhite)
{
}

void BufferTableCell::setHighlighted(bool highlight) {
  HighlightCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Palette::Select : m_backgroundColor;
  m_labelView.setBackgroundColor(backgroundColor);
}

void BufferTableCell::setLabelText(const char * textBody) {
  assert(textBody);
  m_labelView.setText(textBody);
  layoutSubviews();
}

void BufferTableCell::appendText(const char * textBody) {
  assert(textBody);
  m_labelView.appendText(textBody);
  layoutSubviews();
}

void BufferTableCell::setTextColor(KDColor color) {
  m_labelView.setTextColor(color);
}

void BufferTableCell::setMessageFont(const KDFont * font) {
  m_labelView.setFont(font);
}

void BufferTableCell::setBackgroundColor(KDColor color) {
  m_backgroundColor = color;
  m_labelView.setBackgroundColor(color);
}

}
