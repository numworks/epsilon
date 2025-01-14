#include "input_goodness_controller.h"

using namespace Escher;

namespace Inference {

InputGoodnessController::InputGoodnessController(
    StackViewController* parent, ViewController* resultsController,
    GoodnessTest* statistic)
    : InputCategoricalController(
          parent, resultsController, statistic,
          Invocation::Builder<InputCategoricalController>(
              &InputCategoricalController::ButtonAction, this)),
      m_degreeOfFreedomCell(&m_selectableListView, this),
      m_inputGoodnessTableCell(&m_selectableListView, statistic, this, this) {
  m_degreeOfFreedomCell.setMessages(I18n::Message::DegreesOfFreedom);
}

void InputGoodnessController::updateDegreeOfFreedomCell() {
  PrintValueInTextHolder(m_statistic->degreeOfFreedom(),
                         m_degreeOfFreedomCell.textField(), true, true);
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
  if (row == k_indexOfDegreeOfFreedom) {
    return &m_degreeOfFreedomCell;
  }
  return InputCategoricalController::privateExplicitCellAtRow(row);
}

int InputGoodnessController::indexOfEditedParameterAtIndex(int index) const {
  if (index == k_indexOfDegreeOfFreedom) {
    return static_cast<GoodnessTest*>(m_statistic)->indexOfDegreeOfFreedom();
  }
  return InputCategoricalController::indexOfEditedParameterAtIndex(index);
}

}  // namespace Inference
