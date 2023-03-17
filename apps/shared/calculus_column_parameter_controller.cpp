#include "calculus_column_parameter_controller.h"

using namespace Escher;

namespace Shared {

CalculusColumnParameterController::CalculusColumnParameterController(
    I18n::Message hideMessage, ValuesController *valuesController)
    : ColumnParameterController(valuesController),
      m_valuesController(valuesController) {
  m_hideColumn.label()->setMessage(hideMessage);
}

bool CalculusColumnParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    assert(selectedRow() == 0);
    m_valuesController->selectCellAtLocation(
        m_valuesController->selectedColumn() - 1,
        m_valuesController->selectedRow());
    hideCalculusColumn();
    StackViewController *stack = (StackViewController *)(parentResponder());
    stack->pop();
    return true;
  }
  return false;
}

}  // namespace Shared
