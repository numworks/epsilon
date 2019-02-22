#include "storage_function_parameter_controller.h"
#include "storage_values_controller.h"
#include "../app.h"
#include <assert.h>

using namespace Shared;

namespace Graph {

StorageFunctionParameterController::StorageFunctionParameterController(StorageValuesController * valuesController) :
  StorageValuesFunctionParameterController(StorageCartesianFunction::Symbol()),
  m_displayDerivativeColumn(I18n::Message::DerivativeFunctionColumn),
  m_valuesController(valuesController)
{
}

bool StorageFunctionParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    switch (selectedRow()) {
      case 0:
      {
        bool isDisplayingDerivative = function()->displayDerivative();
        function()->setDisplayDerivative(!isDisplayingDerivative);
        m_valuesController->selectCellAtLocation(isDisplayingDerivative ? m_selectedFunctionColumn : m_selectedFunctionColumn + 1, m_valuesController->selectedRow());
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
  m_selectedFunctionColumn = m_valuesController->selectedColumn();
}

void StorageFunctionParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (cell == &m_displayDerivativeColumn) {
    SwitchView * switchView = (SwitchView *)m_displayDerivativeColumn.accessoryView();
    switchView->setState(function()->displayDerivative());
  }
}

ExpiringPointer<StorageCartesianFunction> StorageFunctionParameterController::function() {
  App * a = static_cast<App *>(app());
  return a->functionStore()->modelForRecord(m_record);
}

}
