#include "message_table_cell_with_gauge_with_separator.h"

using namespace Escher;

namespace Settings {

MessageTableCellWithGaugeWithSeparator::MessageTableCellWithGaugeWithSeparator(I18n::Message message) :
  CellWithSeparator(),
  m_cell(message)
{
}

/* MinimalSizeForOptimalDisplay being const, we have no way of ensuring that the
 * width of m_cell's frame is equal to our frame width. We then cannot call
 * m_cell's minimalSizeForOptimalDisplay and must handle everything here. */
KDSize MessageTableCellWithGaugeWithSeparator::minimalSizeForOptimalDisplay() const {
  // Available width is necessary to compute it minimal height.
  KDCoordinate expectedWidth = m_frame.width();
  assert(expectedWidth > 0);
  return KDSize(
    expectedWidth,
    TableCell::minimalHeightForOptimalDisplay(
      m_cell.labelView(),
      m_cell.subLabelView(),
      m_cell.accessoryView(),
      expectedWidth
    ) + k_margin + Metric::CellSeparatorThickness
  );
}

}
