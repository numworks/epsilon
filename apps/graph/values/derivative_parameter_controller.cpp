#include "derivative_parameter_controller.h"
#include <assert.h>

namespace Graph {

DerivativeParameterController::DerivativeParameterController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_pageTitle("Colonne f'(x)"),
  m_hideColumn(ListViewCell((char*)"Masquer la colonne de la derivee")),
  m_copyColumn(ListViewCell((char*)"Copier la colonne dans une liste")),
  m_selectableTableView(SelectableTableView(this, this, Metric::TopMargin, Metric::RightMargin,
    Metric::BottomMargin, Metric::LeftMargin)),
  m_function(nullptr)
{
}

const char * DerivativeParameterController::title() const {
  return m_pageTitle;
}

View * DerivativeParameterController::view() {
  return &m_selectableTableView;
}

void DerivativeParameterController::setFunction(Function * function) {
  m_function = function;
  for (int currentChar = 0; currentChar < k_maxNumberOfCharsInTitle; currentChar++) {
    if (m_pageTitle[currentChar] == '(') {
      m_pageTitle[currentChar-2] = *m_function->name();
      return;
    }
  }
}

void DerivativeParameterController::didBecomeFirstResponder() {
  m_selectableTableView.setSelectedCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

bool DerivativeParameterController::handleEvent(Ion::Events::Event event) {
  switch (event) {
    case Ion::Events::Event::ENTER:
      return handleEnter();
    default:
      return false;
  }
}

bool DerivativeParameterController::handleEnter() {
  switch (m_selectableTableView.selectedRow()) {
    case 0:
    {
      m_function->setDisplayDerivative(false);
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
  return 35;
}

}
