#include "highlight_image_cell.h"

namespace Probability {

HighlightImageCell::HighlightImageCell() : Escher::HighlightCell() { m_contentView.setBackgroundColor(KDColorWhite); }

void HighlightImageCell::setHighlighted(bool highlighted) {
  m_contentView.setBackgroundColor(highlighted ? Escher::Palette::Select : KDColorWhite);
  HighlightCell::setHighlighted(highlighted);
};

KDSize HighlightImageCell::minimalSizeForOptimalDisplay() const {
  return m_contentView.minimalSizeForOptimalDisplay();
}

void HighlightImageCell::layoutSubviews(bool force) { m_contentView.setFrame(bounds(), force); }

} // namespace Probability
