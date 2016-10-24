#include "even_odd_cell.h"

namespace Graph {

constexpr KDColor EvenOddCell::k_evenLineBackgroundColor;
constexpr KDColor EvenOddCell::k_oddLineBackgroundColor;
constexpr KDColor EvenOddCell::k_selectedLineBackgroundColor;

EvenOddCell::EvenOddCell() :
  TableViewCell(),
  m_even(false)
{
}

void EvenOddCell::setEven(bool even) {
  m_even = even;
  reloadCell();
}

KDColor EvenOddCell::backgroundColor() const {
   // Select the background color according to the even line and the cursor selection
  KDColor background = m_even ? EvenOddCell::k_evenLineBackgroundColor : EvenOddCell::k_oddLineBackgroundColor;
  background = isHighlighted() ? EvenOddCell::k_selectedLineBackgroundColor : background;
  return background;
}

void EvenOddCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor background = backgroundColor();
  ctx->fillRect(rect, background);
}

}
