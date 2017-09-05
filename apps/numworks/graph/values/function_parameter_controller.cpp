#include "function_parameter_controller.h"
#include "values_controller.h"
#include <assert.h>

using namespace Shared;

namespace Graph {

FunctionParameterController::FunctionParameterController(ValuesController * valuesController) :
  ValuesFunctionParameterController('x'),
  m_displayDerivativeColumn(I18n::Message::DerivativeFunctionColumn),
  m_cartesianFunction(nullptr),
  m_valuesController(valuesController)
{
}

void FunctionParameterController::setFunction(Function * function) {
  m_cartesianFunction = (CartesianFunction *)function;
  ValuesFunctionParameterController::setFunction(function);
}

bool FunctionParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    switch (selectedRow()) {
      case 0:
      {
        m_cartesianFunction->setDisplayDerivative(!m_cartesianFunction->displayDerivative());
        m_selectableTableView.reloadData();
        return true;
      }
#if COPY_COLUMN
    case 1:
    /* TODO: implement function copy column */
      return true;
#endif
      default:
        assert(false);
        return false;
    }
  }
  return false;
}

int FunctionParameterController::numberOfRows() {
  return k_totalNumberOfCell;
};

HighlightCell * FunctionParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
#if COPY_COLUMN
  HighlightCell * cells[] = {&m_displayDerivativeColumn, &m_copyColumn};
#else
  HighlightCell * cells[] = {&m_displayDerivativeColumn};
#endif
  return cells[index];
}

int FunctionParameterController::reusableCellCount() {
  return k_totalNumberOfCell;
}

void FunctionParameterController::viewWillAppear() {
  ValuesFunctionParameterController::viewWillAppear();
  if (m_cartesianFunction->displayDerivative()) {
    m_valuesController->selectCellAtLocation(m_valuesController->selectedColumn()+1, m_valuesController->selectedRow());
  }
}

void FunctionParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (cell == &m_displayDerivativeColumn) {
    SwitchView * switchView = (SwitchView *)m_displayDerivativeColumn.accessoryView();
    switchView->setState(m_cartesianFunction->displayDerivative());
  }
}

}
