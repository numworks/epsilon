#include "function_parameter_controller.h"
#include "values_controller.h"
#include <assert.h>

namespace Graph {

FunctionParameterController::FunctionParameterController(ValuesController * valuesController) :
  ViewController(nullptr),
  m_pageTitle{"Colonne f(x)"},
  m_displayDerivativeColumn(PointerTableCellWithSwitch((char*)"Colonne de la fonction derivee")),
  m_copyColumn(PointerTableCellWithChevron((char*)"Copier la colonne dans une liste")),
  m_selectableTableView(SelectableTableView(this, this, Metric::CommonTopMargin, Metric::CommonRightMargin,
    Metric::CommonBottomMargin, Metric::CommonLeftMargin)),
  m_function(nullptr),
  m_valuesController(valuesController)
{
}

const char * FunctionParameterController::title() const {
  return m_pageTitle;
}

View * FunctionParameterController::view() {
  return &m_selectableTableView;
}

void FunctionParameterController::setFunction(CartesianFunction * function) {
  m_function = function;
  for (int currentChar = 0; currentChar < k_maxNumberOfCharsInTitle; currentChar++) {
    if (m_pageTitle[currentChar] == '(') {
      m_pageTitle[currentChar-1] = *m_function->name();
      return;
    }
  }
}

void FunctionParameterController::didBecomeFirstResponder() {
  m_selectableTableView.reloadData();
  m_selectableTableView.selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

bool FunctionParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK) {
    switch (m_selectableTableView.selectedRow()) {
      case 0:
      {
        m_function->setDisplayDerivative(!m_function->displayDerivative());
        if (m_function->displayDerivative()) {
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

KDCoordinate FunctionParameterController::cellHeight() {
  return Metric::ParameterCellHeight;
}

void FunctionParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (cell == &m_displayDerivativeColumn) {
    SwitchView * switchView = (SwitchView *)m_displayDerivativeColumn.accessoryView();
    switchView->setState(m_function->displayDerivative());
  }
}

}
