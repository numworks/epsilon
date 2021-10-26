#include "function_parameter_controller.h"
#include "values_controller.h"
#include "../app.h"
#include <assert.h>

using namespace Shared;
using namespace Escher;

namespace Graph {

FunctionParameterController::FunctionParameterController(ValuesController * valuesController) :
  ValuesFunctionParameterController(),
  m_displayDerivativeColumn(I18n::Message::DerivativeFunctionColumn),
  m_valuesController(valuesController)
{
}

bool FunctionParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    switch (selectedRow()) {
      case 0:
      {
        bool isDisplayingDerivative = function()->displayDerivative();
        function()->setDisplayDerivative(!isDisplayingDerivative);
        m_valuesController->selectCellAtLocation(isDisplayingDerivative ? m_selectedFunctionColumn : m_selectedFunctionColumn + 1, m_valuesController->selectedRow());
        resetMemoization();
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

int FunctionParameterController::numberOfRows() const {
  return k_totalNumberOfCell;
};

HighlightCell * FunctionParameterController::reusableCell(int index, int type) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
#if COPY_COLUMN
  HighlightCell * cells[] = {&m_displayDerivativeColumn, &m_copyColumn};
#else
  HighlightCell * cells[] = {&m_displayDerivativeColumn};
#endif
  return cells[index];
}

void FunctionParameterController::viewWillAppear() {
  ValuesFunctionParameterController::viewWillAppear();
  m_selectedFunctionColumn = m_valuesController->selectedColumn();
}

void FunctionParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (cell == &m_displayDerivativeColumn) {
    m_displayDerivativeColumn.setState(function()->displayDerivative());
  }
}

ExpiringPointer<ContinuousFunction> FunctionParameterController::function() {
  return App::app()->functionStore()->modelForRecord(m_record);
}

}
