#include "layout_cell.h"

namespace Probability {

void LayoutCell::setLayout(Poincare::Layout layout) {
  m_expressionView.setLayout(layout);
}

void LayoutCell::setHighlighted(bool highlight) {
  Escher::TableCell::setHighlighted(highlight);
  KDColor color = highlight ? Escher::Palette::Select : backgroundColor();
  m_expressionView.setBackgroundColor(color);
}

KDColor LayoutCell::currentBackgroundColor() {
  return isHighlighted() ? Escher::Palette::Select : backgroundColor();
}

}  // namespace Probability
