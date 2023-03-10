#include "message_table_cell_with_separator.h"

#include <escher/palette.h>

namespace Inference {

MessageTableCellWithSeparator::MessageTableCellWithSeparator(
    Escher::Responder* parentResponder,
    Escher::InputEventHandlerDelegate* inputEventHandlerDelegate,
    Escher::TextFieldDelegate* textFieldDelegate, I18n::Message message)
    : Escher::Responder(parentResponder),
      m_innerCell(this, inputEventHandlerDelegate, textFieldDelegate, message) {
}

void MessageTableCellWithSeparator::drawRect(KDContext* ctx,
                                             KDRect rect) const {
  KDCoordinate width = bounds().width();
  // Upper cell separator
  ctx->fillRect(KDRect(0, 0, width, k_lineThickness),
                Escher::Palette::GrayBright);
  // Fill gap
  ctx->fillRect(KDRect(0, k_lineThickness, width, k_margin),
                Escher::Palette::WallScreen);
}

KDSize MessageTableCellWithSeparator::minimalSizeForOptimalDisplay() const {
  if (m_innerCell.bounds().width() == 0) {
    setChildFrame(&m_innerCell, KDRect(KDPointZero, bounds().width(), 40),
                  false);
  }
  KDSize requiredSize = m_innerCell.minimalSizeForOptimalDisplay();
  return KDSize(requiredSize.width(),
                requiredSize.height() + k_margin + k_lineThickness);
}

void MessageTableCellWithSeparator::layoutSubviews(bool force) {
  setChildFrame(&m_innerCell,
                KDRect(0, k_margin + k_lineThickness, bounds().width(),
                       bounds().height() - k_margin - k_lineThickness),
                force);
}

void MessageTableCellWithSeparator::setHighlighted(bool highlight) {
  m_innerCell.setHighlighted(highlight);
}

}  // namespace Inference
