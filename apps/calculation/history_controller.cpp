#include "history_controller.h"
#include "app.h"
#include "../constant.h"
#include <assert.h>

namespace Calculation {

HistoryController::HistoryController(Responder * parentResponder, CalculationStore * calculationStore) :
  ViewController(parentResponder),
  m_selectableTableView(SelectableTableView(this, this, 0, 0, 0, 0, this)),
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
  m_selectableTableView.selectCellAtLocation(0, numberOfRows()-1);
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
    {
      int focusRow = m_selectableTableView.selectedRow();
      HistoryViewCell * selectedCell = (HistoryViewCell *)m_selectableTableView.cellAtLocation(0, focusRow);
      HistoryViewCell::SelectedView selectedSubview = selectedCell->selectedView();
      EditExpressionController * editController = (EditExpressionController *)parentResponder();
      Calculation * calculation = m_calculationStore->calculationAtIndex(focusRow);
      if (selectedSubview == HistoryViewCell::SelectedView::PrettyPrint) {
        editController->setTextBody(calculation->text());
      } else {
        char buffer[Constant::FloatBufferSizeInScientificMode];
        Float(calculation->evaluation()).convertFloatToText(buffer, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
        editController->setTextBody(buffer);
      }
      m_selectableTableView.deselectTable();
      app()->setFirstResponder(parentResponder());
      return true;
    }
    default:
      return false;
  }
}

void HistoryController::tableViewDidChangeSelection(SelectableTableView * t) {
  m_calculationHistory[t->selectedRow()].setParentResponder(t);
  app()->setFirstResponder(&m_calculationHistory[t->selectedRow()]);
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
  myCell->setEven(index%2 == 0);
}

KDCoordinate HistoryController::rowHeight(int j) {
  Calculation * calculation = m_calculationStore->calculationAtIndex(j);
  KDCoordinate prettyPrintHeight = calculation->layout()->size().height();
  return prettyPrintHeight + k_resultHeight + 3*HistoryViewCell::k_digitVerticalMargin;
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
