#include "input_goodness_controller.h"

using namespace Escher;

namespace Inference {

InputGoodnessController::InputGoodnessController(
    StackViewController *parent, ViewController *resultsController,
    GoodnessTest *statistic,
    InputEventHandlerDelegate *inputEventHandlerDelegate)
    : InputCategoricalController(parent, resultsController, statistic,
                                 inputEventHandlerDelegate),
      m_degreeOfFreedomCell(&m_selectableListView, inputEventHandlerDelegate,
                            this),
      m_goodnessTableCell(&m_selectableListView, this, this, statistic, this) {
  m_degreeOfFreedomCell.label()->setMessage(I18n::Message::DegreesOfFreedom);
  m_degreeOfFreedomCell.subLabel()->setMessage(I18n::Message::Default);
}

void InputGoodnessController::updateDegreeOfFreedomCell() {
  PrintValueInTextHolder(m_statistic->degreeOfFreedom(),
                         m_degreeOfFreedomCell.textField(), true, true);
}

void InputGoodnessController::didBecomeFirstResponder() {
  updateDegreeOfFreedomCell();
  InputCategoricalController::didBecomeFirstResponder();
}

HighlightCell *InputGoodnessController::reusableCell(int index, int type) {
  if (type == k_indexOfDegreeOfFreedom) {
    return &m_degreeOfFreedomCell;
  } else {
    return InputCategoricalController::reusableCell(index, type);
  }
}

int InputGoodnessController::indexOfEditedParameterAtIndex(int index) const {
  if (index == k_indexOfDegreeOfFreedom) {
    return static_cast<GoodnessTest *>(m_statistic)->indexOfDegreeOfFreedom();
  }
  return InputCategoricalController::indexOfEditedParameterAtIndex(index);
}

}  // namespace Inference
