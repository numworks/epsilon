#include "sum_column_parameter_controller.h"

using namespace Escher;
using namespace Shared;

namespace Sequence {

SumColumnParameterController::SumColumnParameterController(
    Shared::ValuesController* valuesController)
    : ColumnParameterController(valuesController),
      m_valuesController(valuesController) {
  m_hideColumn.label()->setMessage(I18n::Message::HideSumOfTerms);
}

bool SumColumnParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    assert(selectedCell() == &m_hideColumn);
    m_valuesController->selectCellAtLocation(
        m_valuesController->selectedColumn() - 1,
        m_valuesController->selectedRow());
    GlobalContext::sequenceStore->modelForRecord(m_record)->setDisplaySum(
        false);
    StackViewController* stack = (StackViewController*)(parentResponder());
    stack->pop();
    return true;
  }
  return false;
}

}  // namespace Sequence
