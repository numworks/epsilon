#include "abscissa_parameter_controller.h"
#include <assert.h>

namespace Graph {

AbscissaParameterController::AbscissaParameterController(Responder * parentResponder, ValuesParameterController * valuesParameterController) :
  ViewController(parentResponder),
  m_deleteColumn(ListViewCell((char*)"Effacer la colonne")),
  m_copyColumn(ListViewCell((char*)"Copier la colonne dans une liste")),
  m_setInterval(ListViewCell((char*)"Regler l'intervalle")),
  m_listView(ListView(this,Metric::TopMargin, Metric::RightMargin,
    Metric::BottomMargin, Metric::LeftMargin)),
  m_activeCell(0),
  m_valuesParameterController(valuesParameterController)
{
}

const char * AbscissaParameterController::title() const {
  return "Colonne x";
}

View * AbscissaParameterController::view() {
  return &m_listView;
}

void AbscissaParameterController::didBecomeFirstResponder() {
  setActiveCell(m_activeCell);
}

void AbscissaParameterController::setActiveCell(int index) {
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

bool AbscissaParameterController::handleEvent(Ion::Events::Event event) {
  switch (event) {
    case Ion::Events::Event::DOWN_ARROW:
      setActiveCell(m_activeCell+1);
      return true;
    case Ion::Events::Event::UP_ARROW:
      setActiveCell(m_activeCell-1);
      return true;
    case Ion::Events::Event::ENTER:
      return true;
    default:
      return false;
  }
}

int AbscissaParameterController::numberOfRows() {
  return k_totalNumberOfCell;
};

View * AbscissaParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
  View * cells[] = {&m_deleteColumn, &m_copyColumn, &m_setInterval};
  return cells[index];
}

int AbscissaParameterController::reusableCellCount() {
  return k_totalNumberOfCell;
}

KDCoordinate AbscissaParameterController::cellHeight() {
  return 35;
}

}
