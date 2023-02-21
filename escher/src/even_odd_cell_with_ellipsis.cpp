#include <escher/even_odd_cell_with_ellipsis.h>

namespace Escher {

EvenOddCellWithEllipsis::EvenOddCellWithEllipsis() : EvenOddCell() {}

void EvenOddCellWithEllipsis::layoutSubviews(bool force) {
  m_ellipsisView.setFrame(bounds(), force);
}

}  // namespace Escher
