#include "history_controller.h"
#include "app.h"
#include "../apps_container.h"
#include <assert.h>

namespace Calculation {

HistoryController::HistoryController(Responder * parentResponder, CalculationStore * calculationStore) :
  DynamicViewController(parentResponder),
  m_calculationStore(calculationStore)
{
}

void HistoryController::reload() {
  selectableTableView()->reloadData();
}

void HistoryController::didBecomeFirstResponder() {
  selectableTableView()->selectCellAtLocation(0, numberOfRows()-1);
  app()->setFirstResponder(selectableTableView());
}

bool HistoryController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Down) {
    selectableTableView()->deselectTable();
    app()->setFirstResponder(parentResponder());
    return true;
  }
  if (event == Ion::Events::Up) {
    return true;
  }
  if (event == Ion::Events::OK) {
    int focusRow = selectableTableView()->selectedRow();
    HistoryViewCell * selectedCell = (HistoryViewCell *)selectableTableView()->cellAtLocation(0, focusRow);
    HistoryViewCell::SubviewType subviewType = selectedCell->selectedSubviewType();
    EditExpressionController * editController = (EditExpressionController *)parentResponder();
    selectableTableView()->deselectTable();
    app()->setFirstResponder(editController);
    Calculation * calculation = m_calculationStore->calculationAtIndex(focusRow);
    if (subviewType == HistoryViewCell::SubviewType::Input) {
      editController->setTextBody(calculation->inputText());
    } else {
      editController->setTextBody(calculation->outputText());
    }
    return true;
  }
  if (event == Ion::Events::EXE) {
    int focusRow = selectableTableView()->selectedRow();
    HistoryViewCell * selectedCell = (HistoryViewCell *)selectableTableView()->cellAtLocation(0, focusRow);
    HistoryViewCell::SubviewType subviewType = selectedCell->selectedSubviewType();
    EditExpressionController * editController = (EditExpressionController *)parentResponder();
    Calculation * calculation = m_calculationStore->calculationAtIndex(focusRow);
    const char * text;
    if (subviewType == HistoryViewCell::SubviewType::Input) {
      text = calculation->inputText();
    } else {
      text = calculation->outputText();
    }
    selectableTableView()->deselectTable();
    App * calculationApp = (App *)app();
    m_calculationStore->push(text, calculationApp->localContext());
    reload();
    selectableTableView()->scrollToCell(0, numberOfRows()-1);
    app()->setFirstResponder(editController);
    return true;
  }
  if (event == Ion::Events::Backspace) {
    int focusRow = selectableTableView()->selectedRow();
    HistoryViewCell * selectedCell = (HistoryViewCell *)selectableTableView()->cellAtLocation(0, focusRow);
    HistoryViewCell::SubviewType subviewType = selectedCell->selectedSubviewType();
    selectableTableView()->deselectTable();
    EditExpressionController * editController = (EditExpressionController *)parentResponder();
    m_calculationStore->deleteCalculationAtIndex(focusRow);
    reload();
    if (numberOfRows()== 0) {
      app()->setFirstResponder(editController);
      return true;
    }
    if (focusRow > 0) {
      selectableTableView()->selectCellAtLocation(0, focusRow-1);
    } else {
      selectableTableView()->selectCellAtLocation(0, 0);
    }
    if (subviewType == HistoryViewCell::SubviewType::Input) {
      tableViewDidChangeSelection(selectableTableView(), 0, selectableTableView()->selectedRow());
    } else {
      tableViewDidChangeSelection(selectableTableView(), 0, -1);
    }
    selectableTableView()->scrollToCell(0, selectableTableView()->selectedRow());
    return true;
  }
  if (event == Ion::Events::Clear) {
    m_calculationStore->deleteAll();
    reload();
    app()->setFirstResponder(parentResponder());
    return true;
  }
  if (event == Ion::Events::Back) {
    EditExpressionController * editController = (EditExpressionController *)parentResponder();
    selectableTableView()->deselectTable();
    app()->setFirstResponder(editController);
    return true;
  }
  return false;
}

void HistoryController::tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) {
  HistoryViewCell * selectedCell = (HistoryViewCell *)(t->selectedCell());
  selectedCell->setParentResponder(t);
  if (selectableTableView()->selectedRow() < previousSelectedCellY) {
    selectedCell->setSelectedSubviewType(HistoryViewCell::SubviewType::Output);
  }
  if (selectableTableView()->selectedRow() >= previousSelectedCellY) {
    selectedCell->setSelectedSubviewType(HistoryViewCell::SubviewType::Input);
  }
  if (previousSelectedCellY == -1) {
    selectedCell->setSelectedSubviewType(HistoryViewCell::SubviewType::Output);
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
  return m_calculationHistory[index];
}

int HistoryController::reusableCellCount(int type) {
  assert(type == 0);
  return k_maxNumberOfDisplayedRows;
}

void HistoryController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  HistoryViewCell * myCell = (HistoryViewCell *)cell;
  myCell->setCalculation(m_calculationStore->calculationAtIndex(index));
  myCell->setEven(index%2 == 0);
}

KDCoordinate HistoryController::rowHeight(int j) {
  Calculation * calculation = m_calculationStore->calculationAtIndex(j);
  KDCoordinate inputHeight = calculation->inputLayout()->size().height();
  KDCoordinate outputHeight = calculation->outputLayout()->size().height();
  return inputHeight + outputHeight + 3*HistoryViewCell::k_digitVerticalMargin;
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

void HistoryController::unloadView() {
  for (int i = 0; i < k_maxNumberOfDisplayedRows; i++) {
    assert(m_calculationHistory[i] != nullptr);
    delete m_calculationHistory[i];
    m_calculationHistory[i] = nullptr;
  }
  DynamicViewController::unloadView();
}

void HistoryController::scrollToCell(int i, int j) {
  selectableTableView()->scrollToCell(i, j);
}

View * HistoryController::createView() {
  for (int i = 0; i < k_maxNumberOfDisplayedRows; i++) {
    assert(m_calculationHistory[i] == nullptr);
    m_calculationHistory[i] = new HistoryViewCell();
  }
  return new CalculationSelectableTableView(this, this, this);
}

CalculationSelectableTableView * HistoryController::selectableTableView() {
  return (CalculationSelectableTableView *)view();
}

}
