#include <escher/even_odd_cell_with_ellipsis.h>

EvenOddCellWithEllipsis::EvenOddCellWithEllipsis() :
  EvenOddCell()
{
}

void EvenOddCellWithEllipsis::layoutSubviews() {
  m_ellipsisView.setFrame(bounds());
}
