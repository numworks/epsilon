#include "table_cells.h"

namespace Periodic {

// MessageTableCellWithMessageWithExpression

void MessageTableCellWithMessageWithExpression::setHighlighted(bool highlight) {
  Escher::MessageTableCellWithMessage::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Escher::Palette::Select : KDColorWhite;
  m_accessoryView.setBackgroundColor(backgroundColor);
}

void MessageTableCellWithMessageWithExpression::setAccessoryLayout(Poincare::Layout layout) {
  m_accessoryView.setLayout(layout);
  if (!layout.isUninitialized()) {
    layoutSubviews();
  }
}

}
