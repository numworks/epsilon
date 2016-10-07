#include "even_odd_cell.h"

constexpr KDColor EvenOddCell::k_evenLineBackgroundColor;
constexpr KDColor EvenOddCell::k_oddLineBackgroundColor;
constexpr KDColor EvenOddCell::k_selectedLineBackgroundColor;

EvenOddCell::EvenOddCell() :
  ChildlessView(),
  m_highlighted(false),
  m_even(false)
{
}

void EvenOddCell::setEven(bool even) {
  m_even = even;
  markRectAsDirty(bounds());
}

void EvenOddCell::setHighlighted(bool highlight) {
  m_highlighted = highlight;
  markRectAsDirty(bounds());
}

void EvenOddCell::reloadCell() {
  markRectAsDirty(bounds());
}

KDColor EvenOddCell::backgroundColor() const {
   // Select the background color according to the even line and the cursor selection
  KDColor background = m_even ? EvenOddCell::k_evenLineBackgroundColor : EvenOddCell::k_oddLineBackgroundColor;
  background = m_highlighted ? EvenOddCell::k_selectedLineBackgroundColor : background;
  return background;
}


void EvenOddCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor background = backgroundColor();
  ctx->fillRect(rect, background);
}

