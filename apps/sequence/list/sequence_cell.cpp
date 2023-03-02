#include "sequence_cell.h"

using namespace Escher;

namespace Sequence {

View* AbstractSequenceCell::subviewAtIndex(int index) {
  switch (index) {
    case 0:
      return &m_sequenceTitleCell;
    default:
      assert(index == 1);
      return expressionCell();
  }
}

void AbstractSequenceCell::layoutSubviews(bool force) {
  KDCoordinate k_titlesColmunWidth = 65;
  m_sequenceTitleCell.setFrame(
      KDRect(0, 0, k_titlesColmunWidth, bounds().height()), force);
  expressionCell()->setFrame(
      KDRect(k_titlesColmunWidth, 0, bounds().width() - k_titlesColmunWidth,
             bounds().height()),
      force);
}

}  // namespace Sequence
