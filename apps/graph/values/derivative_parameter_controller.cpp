#include "derivative_parameter_controller.h"
#include "values_controller.h"
#include "../app.h"
#include <assert.h>

using namespace Escher;

namespace Graph {

DerivativeParameterController::DerivativeParameterController(ValuesController * valuesController) :
  ColumnParameterController(valuesController),
  m_hideColumn(I18n::Message::HideDerivativeColumn),
  m_record(),
  m_valuesController(valuesController)
{
}

Shared::EditableCellTableViewController * DerivativeParameterController::editableCellTableViewController() {
  return m_valuesController;
}

bool DerivativeParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    switch (selectedRow()) {
      case 0:
      {
        m_valuesController->selectCellAtLocation(m_valuesController->selectedColumn()-1, m_valuesController->selectedRow());
        functionStore()->modelForRecord(m_record)->setDisplayDerivative(false);
        StackViewController * stack = (StackViewController *)(parentResponder());
        stack->pop();
        return true;
      }
      default:
        assert(false);
        return false;
    }
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

ContinuousFunctionStore * DerivativeParameterController::functionStore() {
  return App::app()->functionStore();
}

}
