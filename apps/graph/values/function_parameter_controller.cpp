#include "function_parameter_controller.h"
#include <assert.h>

namespace Graph {

FunctionParameterController::FunctionParameterController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_pageTitle("Colonne f(x)"),
  m_displayDerivativeColumn(SwitchMenuListCell((char*)"Colonne de la fonction derivee")),
  m_copyColumn(MenuListCell((char*)"Copier la colonne dans une liste")),
  m_selectableTableView(SelectableTableView(this, this, Metric::TopMargin, Metric::RightMargin,
    Metric::BottomMargin, Metric::LeftMargin)),
  m_function(nullptr)
{
}

const char * FunctionParameterController::title() const {
  return m_pageTitle;
}

View * FunctionParameterController::view() {
  return &m_selectableTableView;
}

void FunctionParameterController::setFunction(Function * function) {
  m_function = function;
  for (int currentChar = 0; currentChar < k_maxNumberOfCharsInTitle; currentChar++) {
    if (m_pageTitle[currentChar] == '(') {
      m_pageTitle[currentChar-1] = *m_function->name();
      return;
    }
  }
}

void FunctionParameterController::didBecomeFirstResponder() {
  m_selectableTableView.setSelectedCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

bool FunctionParameterController::handleEvent(Ion::Events::Event event) {
  switch (event) {
    case Ion::Events::Event::ENTER:
      return handleEnter();
    default:
      return false;
  }
}

bool FunctionParameterController::handleEnter() {
  switch (m_selectableTableView.selectedRow()) {
    case 0:
    {
      m_function->setDisplayDerivative(!m_function->displayDerivative());
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

int FunctionParameterController::numberOfRows() {
  return k_totalNumberOfCell;
};

TableViewCell * FunctionParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
  TableViewCell * cells[] = {&m_displayDerivativeColumn, &m_copyColumn};
  return cells[index];
}

int FunctionParameterController::reusableCellCount() {
  return k_totalNumberOfCell;
}

KDCoordinate FunctionParameterController::cellHeight() {
  return 35;
}

void FunctionParameterController::willDisplayCellForIndex(TableViewCell * cell, int index) {
  if (cell == &m_displayDerivativeColumn) {
    SwitchView * switchView = (SwitchView *)m_displayDerivativeColumn.contentView();
    switchView->setState(m_function->displayDerivative());
  }
}

}
