#include "list_controller.h"
#include <assert.h>

namespace Shared {

ListController::ListController(Responder * parentResponder, FunctionStore * functionStore, ButtonRowController * header, ButtonRowController * footer, I18n::Message text) :
  DynamicViewController(parentResponder),
  ButtonRowDelegate(header, footer),
  m_functionStore(functionStore),
  m_emptyCell(nullptr),
  m_addNewMessage(text),
  m_addNewFunction(nullptr),
  m_plotButton(this, I18n::Message::Plot, Invocation([](void * context, void * sender) {
    ListController * list = (ListController *)context;
    TabViewController * tabController = list->tabController();
    tabController->setActiveTab(1);
  }, this), KDText::FontSize::Small, Palette::PurpleBright),
  m_valuesButton(this, I18n::Message::DisplayValues, Invocation([](void * context, void * sender) {
    ListController * list = (ListController *)context;
    TabViewController * tabController = list->tabController();
    tabController->setActiveTab(2);
  }, this), KDText::FontSize::Small, Palette::PurpleBright)
{
}

int ListController::numberOfColumns() {
  return 2;
};

KDCoordinate ListController::columnWidth(int i) {
  switch (i) {
    case 0:
      return k_functionNameWidth;
    case 1:
      return selectableTableView()->bounds().width()-k_functionNameWidth;
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
      return selectableTableView()->bounds().width();
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
    if (offsetX <= selectableTableView()->bounds().width())
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
      return m_emptyCell;
    case 3:
      return m_addNewFunction;
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
  myCell->setHighlighted(i == selectedColumn() && j == selectedRow());
  myCell->reloadCell();
}

int ListController::numberOfButtons(ButtonRowController::Position position) const {
  if (position == ButtonRowController::Position::Bottom) {
    return 2;
  }
  return 0;
}

Button * ListController::buttonAtIndex(int index, ButtonRowController::Position position) const {
  if (position == ButtonRowController::Position::Top) {
    return nullptr;
  }
  const Button * buttons[2] = {&m_plotButton, &m_valuesButton};
  return (Button *)buttons[index];
}

void ListController::didBecomeFirstResponder() {
  if (selectedRow() == -1) {
    selectCellAtLocation(1, 0);
  } else {
    selectCellAtLocation(selectedColumn(), selectedRow());
  }
  if (selectedRow() >= numberOfRows()) {
    selectCellAtLocation(selectedColumn(), numberOfRows()-1);
  }
  footer()->setSelectedButton(-1);
  app()->setFirstResponder(selectableTableView());
}

bool ListController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up) {
    if (selectedRow() == -1) {
      footer()->setSelectedButton(-1);
      selectableTableView()->selectCellAtLocation(1, numberOfRows()-1);
      app()->setFirstResponder(selectableTableView());
      return true;
    }
    selectableTableView()->deselectTable();
    assert(selectedRow() == -1);
    app()->setFirstResponder(tabController());
    return true;
  }
  if (event == Ion::Events::Down) {
    if (selectedRow() == -1) {
      return false;
    }
    selectableTableView()->deselectTable();
    footer()->setSelectedButton(0);
    return true;
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
      switch (selectedColumn()) {
      case 0:
      {
        if (m_functionStore->numberOfFunctions() < m_functionStore->maxNumberOfFunctions() &&
            selectedRow() == numberOfRows() - 1) {
          return true;
        }
        configureFunction(m_functionStore->functionAtIndex(functionIndexForRow(selectedRow())));
        return true;
      }
      case 1:
      {
        if (m_functionStore->numberOfFunctions() < m_functionStore->maxNumberOfFunctions() &&
            selectedRow() == numberOfRows() - 1) {
          addEmptyFunction();
          return true;
        }
        Shared::Function * function = m_functionStore->functionAtIndex(functionIndexForRow(selectedRow()));
        editExpression(function, event);
        return true;
      }
      default:
      {
        return false;
      }
    }
  }
  if (event == Ion::Events::Backspace && selectedRow() >= 0 &&
      (selectedRow() < numberOfRows() - 1 || m_functionStore->numberOfFunctions()  == m_functionStore->maxNumberOfFunctions())) {
    Shared::Function * function = m_functionStore->functionAtIndex(functionIndexForRow(selectedRow()));
    if (selectedColumn() == 1 && !function->isEmpty()) {
      reinitExpression(function);
    } else {
      if (removeFunctionRow(function)) {
        selectCellAtLocation(selectedColumn(), selectedRow());
        if (selectedRow() >= numberOfRows()) {
          selectCellAtLocation(selectedColumn(), numberOfRows()-1);
        }
        selectableTableView()->reloadData();
      }
    }
    return true;
  }
  if ((event.hasText() || event == Ion::Events::XNT || event == Ion::Events::Paste || event == Ion::Events::Toolbox || event == Ion::Events::Var)
      && selectedColumn() == 1
      && (selectedRow() != numberOfRows() - 1
        || m_functionStore->numberOfFunctions() == m_functionStore->maxNumberOfFunctions())) {
    Shared::Function * function = m_functionStore->functionAtIndex(functionIndexForRow(selectedRow()));
    editExpression(function, event);
    return true;
  }
  if (event == Ion::Events::Copy && selectedColumn() == 1 &&
      (selectedRow() < numberOfRows() - 1 || m_functionStore->numberOfFunctions()  == m_functionStore->maxNumberOfFunctions())) {
    Clipboard::sharedClipboard()->store(textForRow(selectedRow()));
    return true;
  }
  return false;
}

void ListController::didEnterResponderChain(Responder * previousFirstResponder) {
  selectableTableView()->reloadData();
}

void ListController::willExitResponderChain(Responder * nextFirstResponder) {
  if (nextFirstResponder == tabController()) {
    selectableTableView()->deselectTable();
    footer()->setSelectedButton(-1);
  }
}

void ListController::tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) {
  if (m_functionStore->numberOfFunctions() < m_functionStore->maxNumberOfFunctions() && selectedRow() == numberOfRows() - 1 && selectedColumn() == 0) {
    t->selectCellAtLocation(1, numberOfRows()-1);
  }
}

StackViewController * ListController::stackController() const{
  return (StackViewController *)(parentResponder()->parentResponder()->parentResponder());
}

void ListController::configureFunction(Shared::Function * function) {
  StackViewController * stack = stackController();
  parameterController()->setFunction(function);
  stack->push(parameterController());
}

void ListController::reinitExpression(Function * function) {
  function->setContent("");
  selectableTableView()->reloadData();
}

SelectableTableView * ListController::selectableTableView() {
  return (SelectableTableView *)view();
}

TabViewController * ListController::tabController() const{
  return (TabViewController *)(parentResponder()->parentResponder()->parentResponder()->parentResponder());
}

int ListController::functionIndexForRow(int j) {
  return j;
}

const char * ListController::textForRow(int j) {
  Shared::Function * function = m_functionStore->functionAtIndex(functionIndexForRow(j));
  return function->text();
}

void ListController::addEmptyFunction() {
  m_functionStore->addEmptyFunction();
  selectableTableView()->reloadData();
}

View * ListController::loadView() {
  m_emptyCell = new EvenOddCell();
  m_addNewFunction = new NewFunctionCell(m_addNewMessage);
  return new SelectableTableView(this, this, 0, 0, 0, 0, 0, 0, this, this, false, true);
}

void ListController::unloadView(View * view) {
  delete m_emptyCell;
  m_emptyCell = nullptr;
  delete m_addNewFunction;
  m_addNewFunction = nullptr;
  delete view;
}

}
