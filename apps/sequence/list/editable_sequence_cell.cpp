#include "editable_sequence_cell.h"

#include <escher/palette.h>

using namespace Escher;

namespace Sequence {

EditableSequenceCell::EditableSequenceCell(
    Escher::Responder* parentResponder,
    Escher::LayoutFieldDelegate* layoutFieldDelegate)
    : TemplatedSequenceCell<Shared::WithEditableExpressionCell>() {
  expressionCell()->layoutField()->setParentResponder(parentResponder);
  expressionCell()->layoutField()->setDelegate(layoutFieldDelegate);
}

void EditableSequenceCell::layoutSubviews(bool force) {
  setChildFrame(&m_sequenceTitleCell,
                KDRect(0, 0, k_titlesColmunWidth, bounds().height()), force);
  setChildFrame(
      expressionCell(),
      KDRect(k_titlesColmunWidth + k_expressionMargin, 0,
             bounds().width() - k_titlesColmunWidth - 2 * k_expressionMargin,
             bounds().height()),
      force);
}

}  // namespace Sequence
