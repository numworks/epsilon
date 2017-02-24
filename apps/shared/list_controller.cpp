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

HighlightCell * ListController::reusableCell(int index, int type) {
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

void ListController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
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
  if (event == Ion::Events::OK) {
      switch (m_selectableTableView.selectedColumn()) {
      case 0:
      {
        if (m_functionStore->numberOfFunctions() < m_functionStore->maxNumberOfFunctions() &&
            m_selectableTableView.selectedRow() == numberOfRows() - 1) {
          return true;
        }
        configureFunction(m_functionStore->functionAtIndex(functionIndexForRow(m_selectableTableView.selectedRow())));
        return true;
      }
      case 1:
      {
        if (m_functionStore->numberOfFunctions() < m_functionStore->maxNumberOfFunctions() &&
            m_selectableTableView.selectedRow() == numberOfRows() - 1) {
          addEmptyFunction();
          return true;
        }
        Shared::Function * function = m_functionStore->functionAtIndex(functionIndexForRow(m_selectableTableView.selectedRow()));
        editExpression(function, Ion::Events::OK);
        return true;
      }
      default:
      {
        return false;
      }
    }
  }
  if (event == Ion::Events::Backspace && m_selectableTableView.selectedColumn() == 1) {
    Shared::Function * function = m_functionStore->functionAtIndex(functionIndexForRow(m_selectableTableView.selectedRow()));
    reinitExpression(function);
  }
  if ((event.hasText() || event == Ion::Events::XNT)
      && m_selectableTableView.selectedColumn() == 1
      && (m_selectableTableView.selectedRow() != numberOfRows() - 1
         || m_functionStore->numberOfFunctions() == m_functionStore->maxNumberOfFunctions())) {
    Shared::Function * function = m_functionStore->functionAtIndex(functionIndexForRow(m_selectableTableView.selectedRow()));
    editExpression(function, event);
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

void ListController::reinitExpression(Function * function) {
  function->setContent("");
  m_selectableTableView.reloadData();
}

Responder * ListController::tabController() const{
  return (parentResponder()->parentResponder()->parentResponder());
}

int ListController::functionIndexForRow(int j) {
  return j;
}

void ListController::addEmptyFunction() {
  m_functionStore->addEmptyFunction();
  m_selectableTableView.reloadData();
}

}
