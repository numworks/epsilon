#include "function_parameter_controller.h"
#include <assert.h>

namespace Graph {

FunctionParameterController::FunctionParameterController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_pageTitle("Colonne f(x)"),
  m_displayDerivativeColumn(SwitchListViewCell((char*)"Colonne de la fonction derivee")),
  m_copyColumn(ListViewCell((char*)"Copier la colonne dans une liste")),
  m_listView(ListView(this,Metric::TopMargin, Metric::RightMargin,
    Metric::BottomMargin, Metric::LeftMargin)),
  m_activeCell(0),
  m_function(nullptr)
{
}

const char * FunctionParameterController::title() const {
  return m_pageTitle;
}

View * FunctionParameterController::view() {
  return &m_listView;
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
  m_listView.reloadData();
  setActiveCell(m_activeCell);
}

void FunctionParameterController::setActiveCell(int index) {
  if (index < 0 || index >= k_totalNumberOfCell) {
    return;
  }
  ListViewCell * previousCell = (ListViewCell *)(m_listView.cellAtIndex(m_activeCell));
  previousCell->setHighlighted(false);

  m_activeCell = index;
  m_listView.scrollToRow(index);
  ListViewCell * cell = (ListViewCell *)(m_listView.cellAtIndex(index));
  cell->setHighlighted(true);
}

bool FunctionParameterController::handleEvent(Ion::Events::Event event) {
  switch (event) {
    case Ion::Events::Event::DOWN_ARROW:
      setActiveCell(m_activeCell+1);
      return true;
    case Ion::Events::Event::UP_ARROW:
      setActiveCell(m_activeCell-1);
      return true;
    case Ion::Events::Event::ENTER:
      return handleEnter();
    default:
      return false;
  }
}

bool FunctionParameterController::handleEnter() {
  switch (m_activeCell) {
    case 0:
    {
      m_function->setDisplayDerivative(!m_function->displayDerivative());
      m_listView.reloadData();
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
