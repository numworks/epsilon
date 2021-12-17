#include "highlight_image_cell.h"

namespace Solver {

constexpr KDColor HighlightImageCell::k_unselectedBackgroundColor;

HighlightImageCell::HighlightImageCell() : Escher::HighlightCell() {
  m_contentView.setBackgroundColor(k_unselectedBackgroundColor);
}

void HighlightImageCell::setHighlighted(bool highlighted) {
  m_contentView.setBackgroundColor(highlighted ? Escher::Palette::Select
                                               : k_unselectedBackgroundColor);
  HighlightCell::setHighlighted(highlighted);
}

KDSize HighlightImageCell::minimalSizeForOptimalDisplay() const {
  return m_contentView.minimalSizeForOptimalDisplay();
}

void HighlightImageCell::layoutSubviews(bool force) {
  m_contentView.setFrame(bounds(), force);
}

}  // namespace Solver
