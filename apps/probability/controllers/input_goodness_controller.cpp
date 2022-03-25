#include "input_goodness_controller.h"

namespace Probability {

InputGoodnessController::InputGoodnessController(StackViewController * parent, Escher::ViewController * resultsController, GoodnessTest * statistic, InputEventHandlerDelegate * inputEventHandlerDelegate) :
  InputCategoricalController(parent, resultsController, statistic, inputEventHandlerDelegate),
  m_degreeOfFreedomCell(&m_selectableTableView, inputEventHandlerDelegate, this),
  m_goodnessTableCell(&m_selectableTableView, this, this, statistic)
{
  m_degreeOfFreedomCell.setMessage(I18n::Message::DegreesOfFreedom);
  m_degreeOfFreedomCell.setSubLabelMessage(I18n::Message::Default);
}

void InputGoodnessController::updateDegreeOfFreedomCell() {
  PrintValueInTextHolder(m_statistic->degreeOfFreedom(), m_significanceCell.textField(), true, true);
}

void InputGoodnessController::didBecomeFirstResponder() {
  updateDegreeOfFreedomCell();
  InputCategoricalController::didBecomeFirstResponder();
}

HighlightCell * InputGoodnessController::reusableCell(int index, int type) {
  if (type == k_indexOfDegreeOfFreedom) {
    return &m_goodnessTableCell;
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
