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

void HistoryController::reload() {
  m_listView.reloadData();
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


View * HistoryController::reusableCell(int index, int type) {
  assert(type == 0);
  assert(index >= 0);
  assert(index < k_maxNumberOfDisplayedRows);
  return &m_calculHistory[index];
}

int HistoryController::reusableCellCount(int type) {
  assert(type == 0);
  return k_maxNumberOfDisplayedRows;
}

void HistoryController::willDisplayCellForIndex(View * cell, int index) {
  HistoryViewCell * myCell = (HistoryViewCell *)cell;
  myCell->setCalcul(m_calculStore->calculAtIndex(index));
}

KDCoordinate HistoryController::rowHeight(int j) {
  Calcul * calcul = m_calculStore->calculAtIndex(j);
  KDCoordinate calculHeight = calcul->layout()->size().height();
  return calculHeight;
}

KDCoordinate HistoryController::cumulatedHeightFromIndex(int j) {
  int result = 0;
  for (int k = 0; k < j; k++) {
    result += rowHeight(k);
  }
  return result;
}

int HistoryController::indexFromCumulatedHeight(KDCoordinate offsetY) {
  int result = 0;
  int j = 0;
  while (result < offsetY && j < numberOfRows()) {
    result += rowHeight(j++);
  }
  return (result < offsetY || offsetY == 0) ? j : j - 1;
}

int HistoryController::typeAtLocation(int i, int j) {
  return 0;
}

}
