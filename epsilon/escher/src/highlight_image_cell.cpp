#include <escher/highlight_image_cell.h>
#include <escher/theme/components.h>
#include <escher/theme/highlight.h>

namespace Escher {

HighlightImageCell::HighlightImageCell() : HighlightCell() {
  m_contentView.setBackgroundColor(k_unselectedBackgroundColor);
}

void HighlightImageCell::setHighlighted(bool highlighted) {
  m_contentView.setHighlighted(highlighted);
  HighlightCell::setHighlighted(highlighted);
}

KDSize HighlightImageCell::minimalSizeForOptimalDisplay() const {
  return m_contentView.minimalSizeForOptimalDisplay();
}

void HighlightImageCell::layoutSubviews(bool force) {
  setChildFrame(&m_contentView, bounds(), force);
}

}  // namespace Escher
