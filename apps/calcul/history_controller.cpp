#include "history_controller.h"
#include "app.h"
#include <assert.h>

namespace Calcul {

HistoryController::HistoryController(Responder * parentResponder, CalculStore * calculStore) :
  ViewController(parentResponder),
  m_listView(ListView(this, 0, 0, 0, 0)),
  m_activeCell(0),
  m_calculStore(calculStore)
{
}

View * HistoryController::HistoryController::view() {
  return &m_listView;
}

const char * HistoryController::title() const {
  return "Calcul Table";
}

void HistoryController::didBecomeFirstResponder() {
  setActiveCell(numberOfRows()-1);
  m_listView.reloadData();
}

void HistoryController::setActiveCell(int index) {
  if (index < -1 || index >= numberOfRows()) {
    return;
  }
  if (m_activeCell >= 0) {
    HistoryViewCell * previousCell = (HistoryViewCell *)(m_listView.cellAtIndex(m_activeCell));
    previousCell->setHighlighted(false);
  }
  m_activeCell = index;
  if (m_activeCell >= 0) {
    m_listView.scrollToRow(index);
    HistoryViewCell * cell = (HistoryViewCell *)(m_listView.cellAtIndex(index));
    cell->setHighlighted(true);
  }
}

bool HistoryController::handleEvent(Ion::Events::Event event) {
  switch (event) {
    case Ion::Events::Event::DOWN_ARROW:
      if (m_activeCell == numberOfRows()-1) {
        app()->setFirstResponder(parentResponder());
        setActiveCell(-1);
        return true;
      }
      setActiveCell(m_activeCell+1);
      return true;
    case Ion::Events::Event::UP_ARROW:
      setActiveCell(m_activeCell-1);
      return true;
    case Ion::Events::Event::ENTER:
      return false;
    default:
      return false;
  }
}

int HistoryController::numberOfRows() {
  return m_calculStore->numberOfCalculs();
};


View * HistoryController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_maxNumberOfDisplayedRows);
  return &m_calculHistory[index];
}

int HistoryController::reusableCellCount() {
  return k_maxNumberOfDisplayedRows;
}

KDCoordinate HistoryController::cellHeight() {
  return 35;
}

void HistoryController::willDisplayCellForIndex(View * cell, int index) {
  HistoryViewCell * myCell = (HistoryViewCell *)cell;
  myCell->setCalcul(m_calculStore->calculAtIndex(index));
}

}
