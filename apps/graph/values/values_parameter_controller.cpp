#include "values_parameter_controller.h"
#include <assert.h>

ValuesParameterController::ValuesParameterController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_intervalStartCell(TableViewCell((char*)"X Debut")),
  m_intervalEndCell(TableViewCell((char*)"X Fin")),
  m_intervalStepCell(TableViewCell((char*)"Pas")),
  m_listView(ListView(this,Metric::TopMargin, Metric::RightMargin,
    Metric::BottomMargin, Metric::LeftMargin)),
  m_activeCell(0)
{
}

const char * ValuesParameterController::title() const {
  return "Regler l'intervalle";
}

View * ValuesParameterController::view() {
  return &m_listView;
}

void ValuesParameterController::didBecomeFirstResponder() {
  setActiveCell(0);
}

void ValuesParameterController::setActiveCell(int index) {
  if (index < 0 || index >= k_totalNumberOfCell) {
    return;
  }
  TableViewCell * previousCell = (TableViewCell *)(m_listView.cellAtIndex(m_activeCell));
  previousCell->setHighlighted(false);

  m_activeCell = index;
  m_listView.scrollToRow(index);
  TableViewCell * cell = (TableViewCell *)(m_listView.cellAtIndex(index));
  cell->setHighlighted(true);
}

bool ValuesParameterController::handleEvent(Ion::Events::Event event) {
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

int ValuesParameterController::numberOfRows() {
  return k_totalNumberOfCell;
};

View * ValuesParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
  View * cells[] = {&m_intervalStartCell, &m_intervalEndCell, &m_intervalStepCell};
  return cells[index];
}

int ValuesParameterController::reusableCellCount() {
  return k_totalNumberOfCell;
}

KDCoordinate ValuesParameterController::cellHeight() {
  return 35;
}
