#include "derivative_parameter_controller.h"
#include "values_controller.h"
#include <assert.h>

namespace Graph {

DerivativeParameterController::DerivativeParameterController(ValuesController * valuesController) :
  ViewController(valuesController),
  m_pageTitle{"Colonne f'(x)"},
  m_hideColumn(MenuListCell((char*)"Masquer la colonne de la derivee")),
  m_copyColumn(ChevronMenuListCell((char*)"Copier la colonne dans une liste")),
  m_selectableTableView(SelectableTableView(this, this, Metric::TopMargin, Metric::RightMargin,
    Metric::BottomMargin, Metric::LeftMargin)),
  m_function(nullptr),
  m_valuesController(valuesController)
{
}

const char * DerivativeParameterController::title() const {
  return m_pageTitle;
}

View * DerivativeParameterController::view() {
  return &m_selectableTableView;
}

void DerivativeParameterController::setFunction(CartesianFunction * function) {
  m_function = function;
  for (int currentChar = 0; currentChar < k_maxNumberOfCharsInTitle; currentChar++) {
    if (m_pageTitle[currentChar] == '(') {
      m_pageTitle[currentChar-2] = *m_function->name();
      return;
    }
  }
}

void DerivativeParameterController::didBecomeFirstResponder() {
  m_selectableTableView.selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

bool DerivativeParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK) {
    switch (m_selectableTableView.selectedRow()) {
      case 0:
      {
        m_valuesController->selectCellAtLocation(0, -1);
        m_function->setDisplayDerivative(false);
        m_valuesController->selectCellAtLocation(m_valuesController->activeColumn()-1, m_valuesController->activeRow());
        StackViewController * stack = (StackViewController *)(parentResponder());
        stack->pop();
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

int DerivativeParameterController::numberOfRows() {
  return k_totalNumberOfCell;
};

TableViewCell * DerivativeParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
  TableViewCell * cells[] = {&m_hideColumn, &m_copyColumn};
  return cells[index];
}

int DerivativeParameterController::reusableCellCount() {
  return k_totalNumberOfCell;
}

KDCoordinate DerivativeParameterController::cellHeight() {
  return Metric::ParameterCellHeight;
}

}
