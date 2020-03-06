#include <escher/message_table_cell_with_color.h>
#include <escher/palette.h>

MessageTableCellWithColor::MessageTableCellWithColor(I18n::Message message, const KDFont * font) :
  MessageTableCell(message, font),
  m_accessoryView()
{
}

View * MessageTableCellWithColor::accessoryView() const {
  return (View *)&m_accessoryView;
}

void MessageTableCellWithColor::setHighlighted(bool highlight) {
  MessageTableCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Palette::Select : KDColorWhite;
  m_accessoryView.setBackgroundColor(backgroundColor);
}
