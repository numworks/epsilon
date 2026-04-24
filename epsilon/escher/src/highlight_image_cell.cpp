#include <escher/highlight_image_cell.h>
#include <escher/theme/color_picker.h>

namespace Escher {

HighlightImageCell::HighlightImageCell() : HighlightCell() {
  m_contentView.setBackgroundColor(
      ColorPicker::Resolve(k_unselectedBackgroundColor));
}

void HighlightImageCell::setHighlighted(bool highlighted) {
  m_contentView.setBackgroundColor(
      highlighted ? Palette::Select
                  : ColorPicker::Resolve(k_unselectedBackgroundColor));
  HighlightCell::setHighlighted(highlighted);
}

KDSize HighlightImageCell::minimalSizeForOptimalDisplay() const {
  return m_contentView.minimalSizeForOptimalDisplay();
}

void HighlightImageCell::layoutSubviews(bool force) {
  setChildFrame(&m_contentView, bounds(), force);
}

}  // namespace Escher
