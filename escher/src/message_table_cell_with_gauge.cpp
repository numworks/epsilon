#include <escher/message_table_cell_with_gauge.h>
#include <escher/palette.h>

namespace Escher {

MessageTableCellWithGauge::MessageTableCellWithGauge(I18n::Message message) :
  MessageTableCell(message),
  m_accessoryView()
{
}

void MessageTableCellWithGauge::setHighlighted(bool highlight) {
  MessageTableCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Palette::Select : KDColorWhite;
  m_accessoryView.setBackgroundColor(backgroundColor);
}

}
