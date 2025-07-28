#include <escher/even_odd_cell.h>
#include <escher/palette.h>

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

KDColor EvenOddCell::backgroundColor() const {
  if (!isVisible()) {
    return k_hideColor;
  } else if (isHighlighted()) {
    return Palette::Select;
  }
  return m_even ? KDColorWhite : Palette::WallScreen;
}

void EvenOddCell::drawRect(KDContext* ctx, KDRect rect) const {
  KDColor background = backgroundColor();
  ctx->fillRect(rect, background);
}

}  // namespace Escher
