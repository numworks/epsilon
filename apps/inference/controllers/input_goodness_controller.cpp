#include "input_goodness_controller.h"

namespace Inference {

InputGoodnessController::InputGoodnessController(StackViewController * parent, Escher::ViewController * resultsController, GoodnessTest * statistic, InputEventHandlerDelegate * inputEventHandlerDelegate) :
  InputCategoricalController(parent, resultsController, statistic, inputEventHandlerDelegate),
  m_innerDegreeOfFreedomCell(&m_selectableTableView, inputEventHandlerDelegate, this),
  m_degreeOfFreedomCell(&m_innerDegreeOfFreedomCell),
  m_goodnessTableCell(&m_selectableTableView, this, this, statistic, this)
{
  m_innerDegreeOfFreedomCell.setMessage(I18n::Message::DegreesOfFreedom);
  m_innerDegreeOfFreedomCell.setSubLabelMessage(I18n::Message::Default);
}

void InputGoodnessController::updateDegreeOfFreedomCell() {
  PrintValueInTextHolder(m_statistic->degreeOfFreedom(), m_innerDegreeOfFreedomCell.textField(), true, true);
}

void InputGoodnessController::didBecomeFirstResponder() {
  updateDegreeOfFreedomCell();
  InputCategoricalController::didBecomeFirstResponder();
}

HighlightCell * InputGoodnessController::reusableCell(int index, int type) {
  if (type == k_indexOfDegreeOfFreedom) {
    return &m_degreeOfFreedomCell;
  } else {
    return InputCategoricalController::reusableCell(index, type);
  }
}

int InputGoodnessController::indexOfEditedParameterAtIndex(int index) const {
  if (index == k_indexOfDegreeOfFreedom) {
    return m_statistic->indexOfDegreeOfFreedom();
  }
  return InputCategoricalController::indexOfEditedParameterAtIndex(index);
}

}
