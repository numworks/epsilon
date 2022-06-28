#include <escher/buffer_table_cell.h>
#include <escher/i18n.h>
#include <escher/palette.h>
#include <assert.h>

namespace Escher {

BufferTableCell::BufferTableCell():
  TableCell(),
  m_backgroundColor(KDColorWhite)
{
}

BufferTableCell::BufferTableCell(KDFont::Size font, float horizontalAlignment, float verticalAlignment, KDColor textColor, KDColor backgroundColor, size_t maxNumberOfChars) :
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

void BufferTableCell::setMessageWithPlaceholder(I18n::Message message, const char * string) {
  m_labelView.setMessageWithPlaceholder(message, string);
}

void BufferTableCell::appendText(const char * textBody) {
  assert(textBody);
  m_labelView.appendText(textBody);
  layoutSubviews();
}

}
