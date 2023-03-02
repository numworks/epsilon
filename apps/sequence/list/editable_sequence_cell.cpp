#include "editable_sequence_cell.h"

#include <escher/palette.h>

using namespace Escher;

namespace Sequence {

EditableSequenceCell::EditableSequenceCell(
    Escher::Responder* parentResponder,
    Escher::InputEventHandlerDelegate* inputEventHandler,
    Escher::LayoutFieldDelegate* layoutFieldDelegate)
    : AbstractSequenceCell(),
      m_expressionField(parentResponder, inputEventHandler,
                        layoutFieldDelegate) {
  // m_expressionField.setLeftMargin(Metric::EditableExpressionAdditionalMargin);
}

}  // namespace Sequence
