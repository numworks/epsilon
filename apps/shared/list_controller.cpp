#include "list_controller.h"
#include <assert.h>

namespace Shared {

ListController::ListController(Responder * parentResponder, FunctionStore * functionStore, HeaderViewController * header, const char * text) :
  ViewController(parentResponder),
  HeaderViewDelegate(header),
  m_selectableTableView(SelectableTableView(this, this, 0, 0, 0, 0, nullptr, false, true)),
  m_functionStore(functionStore),
  m_addNewFunction(text)
{
}

View * ListController::view() {
  return &m_selectableTableView;
}

int ListController::numberOfColumns() {
  return 2;
};

KDCoordinate ListController::columnWidth(int i) {
  switch (i) {
    case 0:
      return k_functionNameWidth;
    case 1:
      return m_selectableTableView.bounds().width()-k_functionNameWidth;
    default:
      assert(false);
      return 0;
  }
}

KDCoordinate ListController::cumulatedWidthFromIndex(int i) {
  switch (i) {
    case 0:
      return 0;
    case 1:
      return k_functionNameWidth;
    case 2:
      return m_selectableTableView.bounds().width();
    default:
      assert(false);
      return 0;
  }
}

KDCoordinate ListController::cumulatedHeightFromIndex(int j) {
  int result = 0;
  for (int k = 0; k < j; k++) {
    result += rowHeight(k);
  }
  return result;
}

int ListController::indexFromCumulatedWidth(KDCoordinate offsetX) {
  if (offsetX <= k_functionNameWidth) {
    return 0;
  } else {
    if (offsetX <= m_selectableTableView.bounds().width())
      return 1;
    else {
      return 2;
    }
  }
}

int ListController::indexFromCumulatedHeight(KDCoordinate offsetY) {
  int result = 0;
  int j = 0;
  while (result < offsetY && j < numberOfRows()) {
    result += rowHeight(j++);
  }
  return (result < offsetY || offsetY == 0) ? j : j - 1;
}

int ListController::typeAtLocation(int i, int j) {
  if (m_functionStore->numberOfFunctions() < m_functionStore->maxNumberOfFunctions()
      && j == numberOfRows() - 1) {
    return i + 2;
  }
  return i;
}

TableViewCell * ListController::reusableCell(int index, int type) {
  assert(index >= 0);
  assert(index < maxNumberOfRows());
  switch (type) {
    case 0:
      return titleCells(index);
    case 1:
      return expressionCells(index);
    case 2:
      return &m_emptyCell;
    case 3:
      return &m_addNewFunction;
    default:
      assert(false);
      return nullptr;
  }
}

int ListController::reusableCellCount(int type) {
  if (type > 1) {
    return 1;
  }
  return maxNumberOfRows();
}

void ListController::willDisplayCellAtLocation(TableViewCell * cell, int i, int j) {
  if (j < numberOfRows() - 1 || m_functionStore->numberOfFunctions() == m_functionStore->maxNumberOfFunctions()) {
    if (i == 0) {
      willDisplayTitleCellAtIndex(cell, j);
    } else {
      willDisplayExpressionCellAtIndex(cell, j);
    }
  }
  EvenOddCell * myCell = (EvenOddCell *)cell;
  myCell->setEven(j%2 == 0);
  myCell->setHighlighted(i == m_selectableTableView.selectedColumn() && j == m_selectableTableView.selectedRow());
}

void ListController::didBecomeFirstResponder() {
  if (m_selectableTableView.selectedRow() == -1) {
    m_selectableTableView.selectCellAtLocation(1, 0);
  } else {
    m_selectableTableView.selectCellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
  }
  if (m_selectableTableView.selectedRow() >= numberOfRows()) {
    m_selectableTableView.selectCellAtLocation(m_selectableTableView.selectedColumn(), numberOfRows()-1);
  }
  app()->setFirstResponder(&m_selectableTableView);
}

bool ListController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up) {
    m_selectableTableView.deselectTable();
    assert(m_selectableTableView.selectedRow() == -1);
    app()->setFirstResponder(tabController());
    return true;
  }
  return false;
}

void ListController::viewWillAppear() {
  m_selectableTableView.reloadData();
}

StackViewController * ListController::stackController() const{
  return (StackViewController *)(parentResponder()->parentResponder());
}

void ListController::configureFunction(Shared::Function * function) {
  StackViewController * stack = stackController();
  parameterController()->setFunction(function);
  stack->push(parameterController());
}

Responder * ListController::tabController() const{
  return (parentResponder()->parentResponder()->parentResponder());
}

}
