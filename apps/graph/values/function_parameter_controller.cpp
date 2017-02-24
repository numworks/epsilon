#include "function_parameter_controller.h"
#include "values_controller.h"
#include <assert.h>

using namespace Shared;

namespace Graph {

FunctionParameterController::FunctionParameterController(ValuesController * valuesController) :
  ValuesFunctionParameterController('x'),
  m_displayDerivativeColumn(PointerTableCellWithSwitch((char*)"Colonne de la fonction derivee")),
  m_cartesianFunction(nullptr),
  m_valuesController(valuesController)
{
}

void FunctionParameterController::setFunction(Function * function) {
  m_cartesianFunction = (CartesianFunction *)function;
  ValuesFunctionParameterController::setFunction(function);
}

bool FunctionParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK) {
    switch (m_selectableTableView.selectedRow()) {
      case 0:
      {
        m_cartesianFunction->setDisplayDerivative(!m_cartesianFunction->displayDerivative());
        if (m_cartesianFunction->displayDerivative()) {
          m_valuesController->selectCellAtLocation(0, -1);
          m_valuesController->selectCellAtLocation(m_valuesController->activeColumn()+1, m_valuesController->activeRow());
        }
        m_selectableTableView.reloadData();
        return true;
      }
      case 1:
        return false;
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
  HighlightCell * cells[] = {&m_displayDerivativeColumn, &m_copyColumn};
  return cells[index];
}

int FunctionParameterController::reusableCellCount() {
  return k_totalNumberOfCell;
}

void FunctionParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (cell == &m_displayDerivativeColumn) {
    SwitchView * switchView = (SwitchView *)m_displayDerivativeColumn.accessoryView();
    switchView->setState(m_cartesianFunction->displayDerivative());
  }
}

}
