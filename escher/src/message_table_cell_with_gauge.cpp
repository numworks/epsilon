#include <escher/message_table_cell_with_gauge.h>
#include <escher/palette.h>

namespace Escher {

void MessageTableCellWithGauge::setHighlighted(bool highlight) {
  MessageTableCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight ? Palette::Select : KDColorWhite;
  m_accessoryView.setBackgroundColor(backgroundColor);
}

}  // namespace Escher
