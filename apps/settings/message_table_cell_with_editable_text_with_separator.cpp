#include "message_table_cell_with_editable_text_with_separator.h"

using namespace Escher;
using namespace Shared;

namespace Settings {

MessageTableCellWithEditableTextWithSeparator::MessageTableCellWithEditableTextWithSeparator(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * textFieldDelegate, I18n::Message message) :
  CellWithSeparator(),
  m_cell(parentResponder, inputEventHandlerDelegate, textFieldDelegate, message)
{
}

/* MinimalSizeForOptimalDisplay being const, we have no way of ensuring that the
 * width of m_cell's frame is equal to our frame width. We then cannot call
 * m_cell's minimalSizeForOptimalDisplay and must handle everything here.
 * Additionally, subLabelView is ignored because it is an editable TextField
 * View that should not force a two row table cell while the user writes. */
KDSize MessageTableCellWithEditableTextWithSeparator::minimalSizeForOptimalDisplay() const {
  // Available width is necessary to compute it minimal height.
  KDCoordinate expectedWidth = m_frame.width();
  assert(expectedWidth > 0);
  return KDSize(
    expectedWidth,
    TableCell::minimalHeightForOptimalDisplay(
      m_cell.labelView(),
      nullptr,
      m_cell.accessoryView(),
      expectedWidth
    ) + k_margin + Metric::CellSeparatorThickness
  );
}

}
