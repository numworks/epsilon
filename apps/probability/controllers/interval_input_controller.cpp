#include "interval_input_controller.h"

using namespace Probability;

IntervalInputController::IntervalInputController(Escher::StackViewController * stack,
                                                 TestResults * resultsController,
                                                 Escher::InputEventHandlerDelegate * handler)
    : Shared::FloatParameterController<float>(stack), m_resultsController(resultsController) {
  m_paramCells[0].setMessage(I18n::Message::Mean);
  m_paramCells[0].setSubLabelMessage(I18n::Message::SampleMean);
  m_paramCells[1].setMessage(I18n::Message::N);
  m_paramCells[1].setSubLabelMessage(I18n::Message::SampleSize);
  m_paramCells[2].setMessage(I18n::Message::Sigma);
  m_paramCells[2].setSubLabelMessage(I18n::Message::StandardDeviation);
  m_confidenceCell.setMessage(I18n::Message::ConfidenceLevel);
  for (int i = 0; i < k_numberOfParams; i++) {
    m_paramCells[i].textField()->setDelegates(handler, this);
    m_paramCells[i].setParentResponder(&m_selectableTableView);
  }
  m_confidenceCell.textField()->setDelegates(handler, this);
  m_confidenceCell.textField()->setDelegates(handler, this);
  m_confidenceCell.setParentResponder(&m_selectableTableView);
}

int IntervalInputController::typeAtIndex(int index) {
  if (index < k_numberOfParams) {
    return k_parameterCellType;
  } else if (index == k_numberOfParams) {
    return k_confidenceCellType;
  }
  assert(index == k_numberOfParams + 1);
  return k_buttonCellType;
}

Escher::HighlightCell * IntervalInputController::reusableParameterCell(int index, int type) {
  if (type == k_parameterCellType) {
    return &m_paramCells[index];
  }
  if (type == k_confidenceCellType) {
    return &m_confidenceCell;
  }
  return Shared::FloatParameterController<float>::reusableCell(index, type);
}

int IntervalInputController::reusableParameterCellCount(int type) {
  if (type == k_parameterCellType) {
    return k_numberOfParams;
  }
  assert(type == k_buttonCellType || type == k_confidenceCellType);
  return 1;
}

void IntervalInputController::buttonAction() {
  static_cast<StackViewController *>(parentResponder())->push(m_resultsController);
}
