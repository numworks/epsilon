#include "storage_function_parameter_controller.h"
#include "storage_values_controller.h"
#include <assert.h>

using namespace Shared;

namespace Graph {

StorageFunctionParameterController::StorageFunctionParameterController(StorageValuesController * valuesController) :
  StorageValuesFunctionParameterController('x'),
  m_displayDerivativeColumn(I18n::Message::DerivativeFunctionColumn),
  m_cartesianFunction(nullptr),
  m_valuesController(valuesController)
{
}

void StorageFunctionParameterController::setFunction(StorageFunction * function) {
  m_cartesianFunction = (Shared::StorageCartesianFunction *)function;
  StorageValuesFunctionParameterController::setFunction(function);
}

bool StorageFunctionParameterController::handleEvent(Ion::Events::Event event) {
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

int StorageFunctionParameterController::numberOfRows() {
  return k_totalNumberOfCell;
};

HighlightCell * StorageFunctionParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
#if COPY_COLUMN
  HighlightCell * cells[] = {&m_displayDerivativeColumn, &m_copyColumn};
#else
  HighlightCell * cells[] = {&m_displayDerivativeColumn};
#endif
  return cells[index];
}

int StorageFunctionParameterController::reusableCellCount() {
  return k_totalNumberOfCell;
}

void StorageFunctionParameterController::viewWillAppear() {
  StorageValuesFunctionParameterController::viewWillAppear();
  if (m_cartesianFunction->displayDerivative()) {
    m_valuesController->selectCellAtLocation(m_valuesController->selectedColumn()+1, m_valuesController->selectedRow());
  }
}

void StorageFunctionParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (cell == &m_displayDerivativeColumn) {
    SwitchView * switchView = (SwitchView *)m_displayDerivativeColumn.accessoryView();
    switchView->setState(m_cartesianFunction->displayDerivative());
  }
}

}
