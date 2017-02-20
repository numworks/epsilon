#include "history_controller.h"
#include "app.h"
#include "../apps_container.h"
#include <assert.h>

namespace Calculation {

HistoryController::HistoryController(Responder * parentResponder, CalculationStore * calculationStore) :
  ViewController(parentResponder),
  m_selectableTableView(CalculationSelectableTableView(this, this, this)),
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
  if (event == Ion::Events::Down) {
    m_selectableTableView.deselectTable();
    app()->setFirstResponder(parentResponder());
    return true;
  }
  if (event == Ion::Events::Up) {
    return true;
  }
  if (event == Ion::Events::OK) {
    int focusRow = m_selectableTableView.selectedRow();
    HistoryViewCell * selectedCell = (HistoryViewCell *)m_selectableTableView.cellAtLocation(0, focusRow);
    HistoryViewCell::SubviewType subviewType = selectedCell->selectedSubviewType();
    EditExpressionController * editController = (EditExpressionController *)parentResponder();
    Calculation * calculation = m_calculationStore->calculationAtIndex(focusRow);
    if (subviewType == HistoryViewCell::SubviewType::Input) {
      editController->setTextBody(calculation->text());
    } else {
      char outputText[Calculation::k_maximalExpressionTextLength];
      calculation->output()->writeTextInBuffer(outputText, Calculation::k_maximalExpressionTextLength);
      editController->setTextBody(outputText);
    }
    m_selectableTableView.deselectTable();
    app()->setFirstResponder(editController);
    return true;
  }
  if (event == Ion::Events::EXE) {
    int focusRow = m_selectableTableView.selectedRow();
    HistoryViewCell * selectedCell = (HistoryViewCell *)m_selectableTableView.cellAtLocation(0, focusRow);
    HistoryViewCell::SubviewType subviewType = selectedCell->selectedSubviewType();
    EditExpressionController * editController = (EditExpressionController *)parentResponder();
    Calculation * calculation = m_calculationStore->calculationAtIndex(focusRow);
    const char * text;
    if (subviewType == HistoryViewCell::SubviewType::Input) {
      text = calculation->text();
    } else {
      char outputText[Calculation::k_maximalExpressionTextLength];
      calculation->output()->writeTextInBuffer(outputText, Calculation::k_maximalExpressionTextLength);
      text = outputText;
    }
    m_selectableTableView.deselectTable();
    App * calculationApp = (App *)app();
    m_calculationStore->push(text, calculationApp->localContext());
    reload();
    m_selectableTableView.scrollToCell(0, numberOfRows()-1);
    app()->setFirstResponder(editController);
    return true;
  }
  if (event == Ion::Events::Backspace) {
    int focusRow = m_selectableTableView.selectedRow();
    HistoryViewCell * selectedCell = (HistoryViewCell *)m_selectableTableView.cellAtLocation(0, focusRow);
    HistoryViewCell::SubviewType subviewType = selectedCell->selectedSubviewType();
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
    if (subviewType == HistoryViewCell::SubviewType::Input) {
      tableViewDidChangeSelection(&m_selectableTableView, 0, m_selectableTableView.selectedRow());
    } else {
      tableViewDidChangeSelection(&m_selectableTableView, 0, -1);
    }
    m_selectableTableView.scrollToCell(0, m_selectableTableView.selectedRow());
    return true;
  }
  if (event == Ion::Events::Clear) {
    m_calculationStore->deleteAll();
    reload();
    app()->setFirstResponder(parentResponder());
    return true;
  }
  return false;
}

void HistoryController::tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) {
  HistoryViewCell * selectedCell = (HistoryViewCell *)(t->selectedCell());
  selectedCell->setParentResponder(t);
  if (m_selectableTableView.selectedRow() < previousSelectedCellY) {
    selectedCell->setSelectedSubviewType(HistoryViewCell::SubviewType::Output);
  }
  if (m_selectableTableView.selectedRow() >= previousSelectedCellY) {
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

}
