#include <escher/message_table_cell_with_gauge.h>
#include <escher/palette.h>

MessageTableCellWithGauge::MessageTableCellWithGauge(I18n::Message message, KDText::FontSize size) :
  MessageTableCell(message, size),
  m_accessoryView()
{
}

View * MessageTableCellWithGauge::accessoryView() const {
  return (View *)&m_accessoryView;
}

void MessageTableCellWithGauge::setHighlighted(bool highlight) {
  MessageTableCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Palette::Select : KDColorWhite;
  m_accessoryView.setBackgroundColor(backgroundColor);
}
