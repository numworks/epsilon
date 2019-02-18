#include "history_controller.h"
#include "app.h"
#include "../apps_container.h"
#include <assert.h>

using namespace Shared;

namespace Calculation {

HistoryController::HistoryController(Responder * parentResponder, CalculationStore * calculationStore) :
  ViewController(parentResponder),
  m_selectableTableView(this, this, this, this),
  m_calculationHistory{},
  m_calculationStore(calculationStore)
{
  for (int i = 0; i < k_maxNumberOfDisplayedRows; i++) {
    m_calculationHistory[i].setParentResponder(&m_selectableTableView);
    m_calculationHistory[i].setDataSource(this);
  }
}

void HistoryController::reload() {
  m_selectableTableView.reloadData();
}

void HistoryController::didBecomeFirstResponder() {
  selectCellAtLocation(0, numberOfRows()-1);
  app()->setFirstResponder(&m_selectableTableView);
}

void HistoryController::willExitResponderChain(Responder * nextFirstResponder) {
  if (nextFirstResponder == parentResponder()) {
    m_selectableTableView.deselectTable();
  }
}

bool HistoryController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Down) {
    m_selectableTableView.deselectTable();
    app()->setFirstResponder(parentResponder());
    return true;
  }
  if (event == Ion::Events::Up) {
    return true;
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    int focusRow = selectedRow();
    HistoryViewCell * selectedCell = (HistoryViewCell *)m_selectableTableView.selectedCell();
    SubviewType subviewType = selectedSubviewType();
    EditExpressionController * editController = (EditExpressionController *)parentResponder();
    m_selectableTableView.deselectTable();
    app()->setFirstResponder(editController);
    Calculation * calculation = m_calculationStore->calculationAtIndex(focusRow);
    if (subviewType == SubviewType::Input) {
      editController->insertTextBody(calculation->inputText());
    } else {
      ScrollableExactApproximateExpressionsView::SubviewPosition outputSubviewPosition = selectedCell->outputView()->selectedSubviewPosition();
      if (outputSubviewPosition == ScrollableExactApproximateExpressionsView::SubviewPosition::Right
          && !calculation->shouldOnlyDisplayExactOutput())
      {
        editController->insertTextBody(calculation->approximateOutputText());
      } else {
        editController->insertTextBody(calculation->exactOutputText());
      }
    }
    return true;
  }
  if (event == Ion::Events::Backspace) {
    int focusRow = selectedRow();
    SubviewType subviewType = selectedSubviewType();
    m_selectableTableView.deselectTable();
    EditExpressionController * editController = (EditExpressionController *)parentResponder();
    m_calculationStore->deleteCalculationAtIndex(focusRow);
    reload();
    if (numberOfRows()== 0) {
      app()->setFirstResponder(editController);
      return true;
    }
    if (focusRow > 0) {
      m_selectableTableView.selectCellAtLocation(0, focusRow-1);
    } else {
      m_selectableTableView.selectCellAtLocation(0, 0);
    }
    if (subviewType == SubviewType::Input) {
      tableViewDidChangeSelection(&m_selectableTableView, 0, selectedRow());
    } else {
      tableViewDidChangeSelection(&m_selectableTableView, 0, -1);
    }
    m_selectableTableView.scrollToCell(0, selectedRow());
    return true;
  }
  if (event == Ion::Events::Clear) {
    m_selectableTableView.deselectTable();
    m_calculationStore->deleteAll();
    reload();
    app()->setFirstResponder(parentResponder());
    return true;
  }
  if (event == Ion::Events::Back) {
    EditExpressionController * editController = (EditExpressionController *)parentResponder();
    m_selectableTableView.deselectTable();
    app()->setFirstResponder(editController);
    return true;
  }
  return false;
}

void HistoryController::tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) {
  if (previousSelectedCellY == -1) {
    setSelectedSubviewType(SubviewType::Output);
  } else if (selectedRow() < previousSelectedCellY) {
    setSelectedSubviewType(SubviewType::Output);
  } else if (selectedRow() > previousSelectedCellY) {
    setSelectedSubviewType(SubviewType::Input);
  }
  HistoryViewCell * selectedCell = (HistoryViewCell *)(t->selectedCell());
  if (selectedCell == nullptr) {
    return;
  }
  app()->setFirstResponder(selectedCell);
  selectedCell->reloadCell();
}

int HistoryController::numberOfRows() {
  return m_calculationStore->numberOfCalculations();
};

HighlightCell * HistoryController::reusableCell(int index, int type) {
  assert(type == 0);
  assert(index >= 0);
  assert(index < k_maxNumberOfDisplayedRows);
  return &m_calculationHistory[index];
}

int HistoryController::reusableCellCount(int type) {
  assert(type == 0);
  return k_maxNumberOfDisplayedRows;
}

void HistoryController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  HistoryViewCell * myCell = (HistoryViewCell *)cell;
  myCell->setCalculation(m_calculationStore->calculationAtIndex(index));
  myCell->setEven(index%2 == 0);
  myCell->reloadCell();
}

KDCoordinate HistoryController::rowHeight(int j) {
  if (j >= m_calculationStore->numberOfCalculations()) {
    return 0;
  }
  Calculation * calculation = m_calculationStore->calculationAtIndex(j);
  App * calculationApp = (App *)app();
  return calculation->height(calculationApp->localContext()) + 4 * Metric::CommonSmallMargin;
}

int HistoryController::typeAtLocation(int i, int j) {
  return 0;
}

void HistoryController::scrollToCell(int i, int j) {
  m_selectableTableView.scrollToCell(i, j);
}

}
