#include <escher/even_odd_cell.h>
#include <escher/theme/color_picker.h>
#include <escher/theme/highlight.h>

namespace Escher {

EvenOddCell::EvenOddCell() : HighlightCell(), m_even(false) {}

void EvenOddCell::setEven(bool even) {
  if (even != m_even) {
    m_even = even;
    reloadCell();
  }
  updateSubviewsBackgroundAfterChangingState();
}

void EvenOddCell::setVisible(bool visible) {
  if (visible != isVisible()) {
    HighlightCell::setVisible(visible);
    updateSubviewsBackgroundAfterChangingState();
    reloadCell();
  }
}

void EvenOddCell::setHighlighted(bool highlighted) {
  HighlightCell::setHighlighted(highlighted);
  updateSubviewsBackgroundAfterChangingState();
}

ThemeComponent EvenOddCell::backgroundColor() const {
  if (!isVisible()) {
    return k_hideColor;
  }
  return baseColor();
}

void EvenOddCell::drawRect(KDContext* ctx, KDRect rect) const {
  ctx->fillRect(rect, ColorPicker::Resolve(
                          Highlight(backgroundColor(), isHighlighted())));
}

}  // namespace Escher
