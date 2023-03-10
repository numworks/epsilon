#include <assert.h>
#include <escher/buffer_table_cell.h>
#include <escher/i18n.h>
#include <escher/palette.h>
#include <stdarg.h>

namespace Escher {

BufferTableCell::BufferTableCell()
    : TableCell(), m_backgroundColor(KDColorWhite) {}

BufferTableCell::BufferTableCell(KDFont::Size font, float horizontalAlignment,
                                 float verticalAlignment, KDColor textColor,
                                 KDColor backgroundColor,
                                 size_t maxNumberOfChars)
    : TableCell(),
      m_labelView({{.glyphColor = textColor,
                    .backgroundColor = backgroundColor,
                    .font = font},
                   .horizontalAlignment = horizontalAlignment,
                   .verticalAlignment = verticalAlignment},
                  maxNumberOfChars),
      m_backgroundColor(backgroundColor) {}

void BufferTableCell::setHighlighted(bool highlight) {
  HighlightCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight ? Palette::Select : m_backgroundColor;
  m_labelView.setBackgroundColor(backgroundColor);
}

void BufferTableCell::setLabelText(const char* textBody) {
  assert(textBody);
  m_labelView.setText(textBody);
  layoutSubviews();
}

void BufferTableCell::setMessageWithPlaceholders(I18n::Message message, ...) {
  va_list args;
  va_start(args, message);
  m_labelView.privateSetMessageWithPlaceholders(message, args);
  va_end(args);
}

void BufferTableCell::appendText(const char* textBody) {
  assert(textBody);
  m_labelView.appendText(textBody);
  layoutSubviews();
}

}  // namespace Escher
