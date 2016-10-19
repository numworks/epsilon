#include "derivative_parameter_controller.h"
#include <assert.h>

namespace Graph {

DerivativeParameterController::DerivativeParameterController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_pageTitle("Colonne f'(x)"),
  m_hideColumn(ListViewCell((char*)"Masquer la colonne de la derivee")),
  m_copyColumn(ListViewCell((char*)"Copier la colonne dans une liste")),
  m_listView(ListView(this,Metric::TopMargin, Metric::RightMargin,
    Metric::BottomMargin, Metric::LeftMargin)),
  m_activeCell(0),
  m_function(nullptr)
{
}

const char * DerivativeParameterController::title() const {
  return m_pageTitle;
}

View * DerivativeParameterController::view() {
  return &m_listView;
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
  m_listView.reloadData();
  setActiveCell(m_activeCell);
}

void DerivativeParameterController::setActiveCell(int index) {
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

bool DerivativeParameterController::handleEvent(Ion::Events::Event event) {
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

bool DerivativeParameterController::handleEnter() {
  switch (m_activeCell) {
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

View * DerivativeParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
  View * cells[] = {&m_hideColumn, &m_copyColumn};
  return cells[index];
}

int DerivativeParameterController::reusableCellCount() {
  return k_totalNumberOfCell;
}

KDCoordinate DerivativeParameterController::cellHeight() {
  return 35;
}

}
