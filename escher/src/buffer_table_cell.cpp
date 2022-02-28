#include <escher/buffer_table_cell.h>
#include <escher/palette.h>
#include <apps/i18n.h>
#include <assert.h>

namespace Escher {

BufferTableCell::BufferTableCell():
  TableCell(),
  m_labelView(),
  m_backgroundColor(KDColorWhite)
{
}

BufferTableCell::BufferTableCell(const KDFont * font, float horizontalAlignment, float verticalAlignment, KDColor textColor, KDColor backgroundColor, size_t maxNumberOfChars) :
  TableCell(),
  m_labelView(font, horizontalAlignment, verticalAlignment, textColor, backgroundColor, maxNumberOfChars),
  m_backgroundColor(backgroundColor)
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

void BufferTableCell::setSimpleCustomLabelText(I18n::Message message, const char * string) {
  m_labelView.setSimpleCustomText(message, string);
}

void BufferTableCell::appendText(const char * textBody) {
  assert(textBody);
  m_labelView.appendText(textBody);
  layoutSubviews();
}

}
