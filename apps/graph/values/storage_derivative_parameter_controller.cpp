#include "storage_derivative_parameter_controller.h"
#include "storage_values_controller.h"
#include "../app.h"
#include <assert.h>

namespace Graph {

StorageDerivativeParameterController::StorageDerivativeParameterController(StorageValuesController * valuesController) :
  ViewController(valuesController),
  m_hideColumn(I18n::Message::HideDerivativeColumn),
#if COPY_COLUMN
  m_copyColumn(I18n::Message::CopyColumnInList),
#endif
  m_selectableTableView(this),
  m_record(),
  m_valuesController(valuesController)
{
}

void StorageDerivativeParameterController::viewWillAppear() {
  functionStore()->modelForRecord(m_record)->derivativeNameWithArgument(m_pageTitle, k_maxNumberOfCharsInTitle, Shared::StorageCartesianFunction::Symbol());
}

const char * StorageDerivativeParameterController::title() {
  return m_pageTitle;
}

View * StorageDerivativeParameterController::view() {
  return &m_selectableTableView;
}

void StorageDerivativeParameterController::didBecomeFirstResponder() {
  selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

bool StorageDerivativeParameterController::handleEvent(Ion::Events::Event event) {
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

int StorageDerivativeParameterController::numberOfRows() {
  return k_totalNumberOfCell;
};

HighlightCell * StorageDerivativeParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
#if COPY_COLUMN
  HighlightCell * cells[] = {&m_hideColumn, &m_copyColumn};
#else
  HighlightCell * cells[] = {&m_hideColumn};
#endif
  return cells[index];
}

int StorageDerivativeParameterController::reusableCellCount() {
  return k_totalNumberOfCell;
}

KDCoordinate StorageDerivativeParameterController::cellHeight() {
  return Metric::ParameterCellHeight;
}

StorageCartesianFunctionStore * StorageDerivativeParameterController::functionStore() {
  App * a = static_cast<App *>(app());
  return a->functionStore();
}

}
