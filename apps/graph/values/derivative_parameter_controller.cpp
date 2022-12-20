#include "derivative_parameter_controller.h"
#include "values_controller.h"
#include "../app.h"
#include <assert.h>

using namespace Escher;

namespace Graph {

DerivativeParameterController::DerivativeParameterController(ValuesController * valuesController) :
  ColumnParameterController(valuesController),
  m_hideColumn(I18n::Message::HideDerivativeColumn),
  m_valuesController(valuesController)
{}

Shared::ClearColumnHelper * DerivativeParameterController::clearColumnHelper() {
  return m_valuesController;
}

bool DerivativeParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    assert(selectedRow() == 0);
    m_valuesController->selectCellAtLocation(m_valuesController->selectedColumn()-1, m_valuesController->selectedRow());
    functionStore()->modelForRecord(m_record)->setDisplayDerivative(false);
    StackViewController * stack = (StackViewController *)(parentResponder());
    stack->pop();
    return true;
  }
  return false;
}

int DerivativeParameterController::numberOfRows() const {
  return k_totalNumberOfCell;
};

HighlightCell * DerivativeParameterController::reusableCell(int index, int type) {
  assert(index == 0);
  return &m_hideColumn;
}

Shared::ContinuousFunctionStore * DerivativeParameterController::functionStore() {
  return App::app()->functionStore();
}

}
