#include "editable_sequence_cell.h"

#include <escher/palette.h>

using namespace Escher;

namespace Sequence {

void EditableSequenceCell::layoutSubviews(bool force) {
  m_sequenceTitleCell.setFrame(
      KDRect(0, 0, k_titlesColmunWidth, bounds().height()), force);
  expressionCell()->setFrame(
      KDRect(k_titlesColmunWidth + k_expressionMargin, 0,
             bounds().width() - k_titlesColmunWidth - 2 * k_expressionMargin,
             bounds().height()),
      force);
}

}  // namespace Sequence
