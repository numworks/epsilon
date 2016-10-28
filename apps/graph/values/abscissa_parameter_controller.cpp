#include "abscissa_parameter_controller.h"
#include <assert.h>

namespace Graph {

AbscissaParameterController::AbscissaParameterController(Responder * parentResponder, ValuesParameterController * valuesParameterController) :
  ViewController(parentResponder),
  m_deleteColumn(MenuListCell((char*)"Effacer la colonne")),
  m_copyColumn(MenuListCell((char*)"Copier la colonne dans une liste")),
  m_setInterval(MenuListCell((char*)"Regler l'intervalle")),
  m_selectableTableView(SelectableTableView(this, this, Metric::TopMargin, Metric::RightMargin,
    Metric::BottomMargin, Metric::LeftMargin)),
  m_valuesParameterController(valuesParameterController)
{
}

const char * AbscissaParameterController::title() const {
  return "Colonne x";
}

View * AbscissaParameterController::view() {
  return &m_selectableTableView;
}

void AbscissaParameterController::didBecomeFirstResponder() {
  m_selectableTableView.selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

bool AbscissaParameterController::handleEvent(Ion::Events::Event event) {
  switch (event) {
    case Ion::Events::Event::ENTER:
      return handleEnter();
    default:
      return false;
  }
}

bool AbscissaParameterController::handleEnter() {
  switch (m_selectableTableView.selectedRow()) {
    case 0:
    {
      Interval * interval = m_valuesParameterController->interval();
      interval->setEnd(0.0f);
      interval->setStep(1.0f);
      interval->setStart(1.0f);
      StackViewController * stack = ((StackViewController *)parentResponder());
      stack->pop();
      return true;
    }
    case 1:
      return false;
    case 2:
    {
      StackViewController * stack = ((StackViewController *)parentResponder());
      stack->push(m_valuesParameterController);
      return true;
    }
    default:
      assert(false);
      return false;
  }
}

int AbscissaParameterController::numberOfRows() {
  return k_totalNumberOfCell;
};

TableViewCell * AbscissaParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
  TableViewCell * cells[] = {&m_deleteColumn, &m_copyColumn, &m_setInterval};
  return cells[index];
}

int AbscissaParameterController::reusableCellCount() {
  return k_totalNumberOfCell;
}

KDCoordinate AbscissaParameterController::cellHeight() {
  return 35;
}

}
