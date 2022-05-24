#include "function_parameter_controller.h"
#include "values_controller.h"
#include "../app.h"
#include <assert.h>

using namespace Shared;
using namespace Escher;

namespace Graph {

FunctionParameterController::FunctionParameterController(ValuesController * valuesController) :
  ColumnParameterController(valuesController),
  m_displayDerivativeColumn(I18n::Message::DerivativeFunctionColumn),
  m_valuesController(valuesController)
{ }

Shared::ClearColumnHelper * FunctionParameterController::clearColumnHelper() {
  return m_valuesController;
}



bool FunctionParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    assert(selectedRow() == 0);
    bool isDisplayingDerivative = function()->displayDerivative();
    function()->setDisplayDerivative(!isDisplayingDerivative);
    m_valuesController->selectCellAtLocation(isDisplayingDerivative ? m_valuesController->selectedColumn() : m_valuesController->selectedColumn() + 1, m_valuesController->selectedRow());
    m_selectableTableView.reloadData();
    return true;
  }
  return false;
}

int FunctionParameterController::numberOfRows() const {
  return k_totalNumberOfCell;
};

HighlightCell * FunctionParameterController::reusableCell(int index, int type) {
  assert(index == 0);
  return &m_displayDerivativeColumn;
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
