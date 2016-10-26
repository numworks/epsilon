#include "history_controller.h"
#include "app.h"
#include <assert.h>

namespace Calculation {

HistoryController::HistoryController(Responder * parentResponder, CalculationStore * calculationStore) :
  ViewController(parentResponder),
  m_selectableTableView(SelectableTableView(this, this)),
  m_calculationStore(calculationStore)
{
}

View * HistoryController::HistoryController::view() {
  return &m_selectableTableView;
}

const char * HistoryController::title() const {
  return "Calculation Table";
}

void HistoryController::reload() {
  m_selectableTableView.reloadData();
}

void HistoryController::didBecomeFirstResponder() {
  m_selectableTableView.setSelectedCellAtLocation(0, numberOfRows()-1);
  app()->setFirstResponder(&m_selectableTableView);
}

bool HistoryController::handleEvent(Ion::Events::Event event) {
  switch (event) {
    case Ion::Events::Event::DOWN_ARROW:
      m_selectableTableView.deselectTable();
      app()->setFirstResponder(parentResponder());
      return true;
    case Ion::Events::Event::UP_ARROW:
      return true;
    case Ion::Events::Event::ENTER:
      return false;
    default:
      return false;
  }
}

int HistoryController::numberOfRows() {
  return m_calculationStore->numberOfCalculations();
};

TableViewCell * HistoryController::reusableCell(int index, int type) {
  assert(type == 0);
  assert(index >= 0);
  assert(index < k_maxNumberOfDisplayedRows);
  return &m_calculationHistory[index];
}

int HistoryController::reusableCellCount(int type) {
  assert(type == 0);
  return k_maxNumberOfDisplayedRows;
}

void HistoryController::willDisplayCellForIndex(TableViewCell * cell, int index) {
  HistoryViewCell * myCell = (HistoryViewCell *)cell;
  myCell->setCalculation(m_calculationStore->calculationAtIndex(index));
}

KDCoordinate HistoryController::rowHeight(int j) {
  Calculation * calculation = m_calculationStore->calculationAtIndex(j);
  KDCoordinate prettyPrintHeight = calculation->layout()->size().height();
  return prettyPrintHeight + k_resultHeight;
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
