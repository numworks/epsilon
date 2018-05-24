#include "function_list_controller.h"
#include <assert.h>

namespace Shared {

FunctionListController::FunctionListController(Responder * parentResponder, FunctionStore * functionStore, ButtonRowController * header, ButtonRowController * footer, I18n::Message text) :
  ExpressionModelListController(parentResponder, text),
  ButtonRowDelegate(header, footer),
  m_functionStore(functionStore),
  m_emptyCell(nullptr),
  m_plotButton(this, I18n::Message::Plot, Invocation([](void * context, void * sender) {
    FunctionListController * list = (FunctionListController *)context;
    TabViewController * tabController = list->tabController();
    tabController->setActiveTab(1);
  }, this), KDText::FontSize::Small, Palette::PurpleBright),
  m_valuesButton(this, I18n::Message::DisplayValues, Invocation([](void * context, void * sender) {
    FunctionListController * list = (FunctionListController *)context;
    TabViewController * tabController = list->tabController();
    tabController->setActiveTab(2);
  }, this), KDText::FontSize::Small, Palette::PurpleBright)
{
}

int FunctionListController::numberOfColumns() {
  return 2;
};

KDCoordinate FunctionListController::columnWidth(int i) {
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

KDCoordinate FunctionListController::cumulatedWidthFromIndex(int i) {
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

int FunctionListController::indexFromCumulatedWidth(KDCoordinate offsetX) {
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

int FunctionListController::typeAtLocation(int i, int j) {
  if (isAddEmptyRow(j)){
    return i + 2;
  }
  return i;
}

HighlightCell * FunctionListController::reusableCell(int index, int type) {
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
      return m_addNewModel;
    default:
      assert(false);
      return nullptr;
  }
}

int FunctionListController::reusableCellCount(int type) {
  if (type > 1) {
    return 1;
  }
  return maxNumberOfRows();
}

void FunctionListController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  if (!isAddEmptyRow(j)) {
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

int FunctionListController::numberOfButtons(ButtonRowController::Position position) const {
  if (position == ButtonRowController::Position::Bottom) {
    return 2;
  }
  return 0;
}

Button * FunctionListController::buttonAtIndex(int index, ButtonRowController::Position position) const {
  if (position == ButtonRowController::Position::Top) {
    return nullptr;
  }
  const Button * buttons[2] = {&m_plotButton, &m_valuesButton};
  return (Button *)buttons[index];
}

void FunctionListController::didBecomeFirstResponder() {
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

bool FunctionListController::handleEvent(Ion::Events::Event event) {
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
  if (selectedRow() < 0) {
    return false;
  }
  if (selectedColumn() == 1) {
    return handleEventOnExpression(event);
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    assert(selectedColumn() == 0);
    configureFunction(m_functionStore->modelAtIndex(modelIndexForRow(selectedRow())));
    return true;
  }
  if (event == Ion::Events::Backspace) {
    Shared::Function * function = m_functionStore->modelAtIndex(modelIndexForRow(selectedRow()));
    if (removeModelRow(function)) {
      int newSelectedRow = selectedRow() >= numberOfRows() ? numberOfRows()-1 : selectedRow();
      selectCellAtLocation(selectedColumn(), newSelectedRow);
      selectableTableView()->reloadData();
    }
    return true;
  }
  return false;
}

void FunctionListController::didEnterResponderChain(Responder * previousFirstResponder) {
  selectableTableView()->reloadData();
}

void FunctionListController::willExitResponderChain(Responder * nextFirstResponder) {
  if (nextFirstResponder == tabController()) {
    selectableTableView()->deselectTable();
    footer()->setSelectedButton(-1);
  }
}

void FunctionListController::tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) {
  if (isAddEmptyRow(selectedRow()) && selectedColumn() == 0) {
    t->selectCellAtLocation(1, numberOfRows()-1);
  }
}

StackViewController * FunctionListController::stackController() const{
  return (StackViewController *)(parentResponder()->parentResponder()->parentResponder());
}

void FunctionListController::configureFunction(Shared::Function * function) {
  StackViewController * stack = stackController();
  parameterController()->setFunction(function);
  stack->push(parameterController());
}

TabViewController * FunctionListController::tabController() const{
  return (TabViewController *)(parentResponder()->parentResponder()->parentResponder()->parentResponder());
}

View * FunctionListController::loadView() {
  m_emptyCell = new EvenOddCell();
  return ExpressionModelListController::loadView();
}

void FunctionListController::unloadView(View * view) {
  delete m_emptyCell;
  m_emptyCell = nullptr;
  Shared::ExpressionModelListController::unloadView(view);
}

}
