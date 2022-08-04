#include <escher/even_odd_cell.h>
#include <escher/palette.h>

namespace Escher {

EvenOddCell::EvenOddCell() :
  HighlightCell(),
  m_even(false)
{
}

void EvenOddCell::setEven(bool even) {
  if (even != m_even) {
    m_even = even;
    reloadCell();
  }
}

KDColor EvenOddCell::backgroundColor() const {
   // Select the background color according to the even line and the cursor selection
  KDColor background = m_even ? KDColorWhite : Palette::WallScreen;
  return isHighlighted() ? Palette::Select : background;
}

void EvenOddCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor background = backgroundColor();
  ctx->fillRect(rect, background);
}

}
