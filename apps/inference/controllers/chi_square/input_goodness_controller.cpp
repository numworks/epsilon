#include "input_goodness_controller.h"

using namespace Escher;

namespace Inference {

InputGoodnessController::InputGoodnessController(
    StackViewController* parent, ViewController* resultsController,
    GoodnessTest* inference)
    : InputCategoricalController(
          parent, resultsController, inference,
          Invocation::Builder<InputCategoricalController>(
              &InputCategoricalController::ButtonAction, this)),
      m_degreeOfFreedomCell(&m_selectableListView, this),
      m_inputGoodnessTableCell(&m_selectableListView, inference, this, this) {
  m_degreeOfFreedomCell.setMessages(I18n::Message::DegreesOfFreedom);
}

void InputGoodnessController::updateDegreeOfFreedomCell() {
  PrintValueInTextHolder(m_inference->degreeOfFreedom(),
                         m_degreeOfFreedomCell.textField(), true, true, true);
}

void InputGoodnessController::viewWillAppear() {
  updateDegreeOfFreedomCell();
  InputCategoricalController::viewWillAppear();
}

void InputGoodnessController::createDynamicCells() {
  m_inputGoodnessTableCell.createCells();
}

const HighlightCell* InputGoodnessController::privateExplicitCellAtRow(
    int row) const {
  if (row == k_indexOfDegreesOfFreedom) {
    return &m_degreeOfFreedomCell;
  }
  return InputCategoricalController::privateExplicitCellAtRow(row);
}

int InputGoodnessController::indexOfEditedParameterAtIndex(int index) const {
  if (index == k_indexOfDegreesOfFreedom) {
    return static_cast<GoodnessTest*>(m_inference)->indexOfDegreesOfFreedom();
  }
  return InputCategoricalController::indexOfEditedParameterAtIndex(index);
}

}  // namespace Inference
