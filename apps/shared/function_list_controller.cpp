#include "function_list_controller.h"
#include "function_app.h"
#include "function_expression_cell.h"
#include <algorithm>

namespace Shared {

constexpr KDCoordinate FunctionListController::k_minTitleColumnWidth;

FunctionListController::FunctionListController(Responder * parentResponder, ButtonRowController * header, ButtonRowController * footer, I18n::Message text) :
  ExpressionModelListController(parentResponder, text),
  ButtonRowDelegate(header, footer),
  m_selectableTableView(this, this, this, this),
  m_emptyCell(),
  m_plotButton(this, I18n::Message::Plot, Invocation([](void * context, void * sender) {
      FunctionListController * list = (FunctionListController *)context;
      TabViewController * tabController = list->tabController();
      tabController->setActiveTab(1);
      return true;
    }, this), KDFont::SmallFont, Palette::ButtonText),
  m_valuesButton(this, I18n::Message::DisplayValues, Invocation([](void * context, void * sender) {
      FunctionListController * list = (FunctionListController *)context;
      TabViewController * tabController = list->tabController();
      tabController->setActiveTab(2);
      return true;
    }, this), KDFont::SmallFont, Palette::ButtonText),
  m_titlesColumnWidth(k_minTitleColumnWidth)
{
  /* m_memoizedCellBaseline is not initialized by the call to
   * resetMemoizationForIndex in ExpressionModelListController's
   * constructor, because it is a virtual method in a constructor. */
  for (int i = 0; i < k_memoizedCellsCount; i++) {
    m_memoizedCellBaseline[i] = -1;
  }
  m_selectableTableView.setMargins(0);
  m_selectableTableView.setVerticalCellOverlap(0);
}

/* TableViewDataSource */

void FunctionListController::viewWillAppear() {
  ExpressionModelListController::viewWillAppear();
  computeTitlesColumnWidth();
}

KDCoordinate FunctionListController::columnWidth(int i) {
  switch (i) {
    case 0:
      return m_titlesColumnWidth;
    case 1:
      return selectableTableView()->bounds().width()-m_titlesColumnWidth;
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
      return m_titlesColumnWidth;
    case 2:
      return selectableTableView()->bounds().width();
    default:
      assert(false);
      return 0;
  }
}

int FunctionListController::indexFromCumulatedWidth(KDCoordinate offsetX) {
  if (offsetX <= m_titlesColumnWidth) {
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
  assert(index < maxNumberOfDisplayableRows());
  switch (type) {
    case 0:
      return titleCells(index);
    case 1:
      return expressionCells(index);
    case 2:
      return &m_emptyCell;
    case 3:
      return &(m_addNewModel);
    default:
      assert(false);
      return nullptr;
  }
}

int FunctionListController::reusableCellCount(int type) {
  if (type > 1) {
    return 1;
  }
  return maxNumberOfDisplayableRows();
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

/* ButtonRowDelegate */

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

/* Responder */

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
  Container::activeApp()->setFirstResponder(selectableTableView());
}

bool FunctionListController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up) {
    if (selectedRow() == -1) {
      footer()->setSelectedButton(-1);
      selectableTableView()->selectCellAtLocation(1, numberOfRows()-1);
      Container::activeApp()->setFirstResponder(selectableTableView());
      return true;
    }
    selectableTableView()->deselectTable();
    assert(selectedRow() == -1);
    Container::activeApp()->setFirstResponder(tabController());
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
    configureFunction(modelStore()->recordAtIndex(modelIndexForRow(selectedRow())));
    return true;
  }
  if (event == Ion::Events::Backspace) {
    Ion::Storage::Record record = modelStore()->recordAtIndex(modelIndexForRow(selectedRow()));
    if (removeModelRow(record)) {
      int newSelectedRow = selectedRow() >= numberOfRows() ? numberOfRows()-1 : selectedRow();
      selectCellAtLocation(selectedColumn(), newSelectedRow);
      selectableTableView()->reloadData();
    }
    return true;
  }
  if (event == Ion::Events::ShiftBack) {
    Ion::Storage::sharedStorage()->reinsertTrash(recordExtension());
    selectableTableView()->reloadData();
    return true;
  }
  return false;
}

void FunctionListController::didEnterResponderChain(Responder * previousFirstResponder) {
  selectableTableView()->reloadData();
}

void FunctionListController::willExitResponderChain(Responder * nextFirstResponder) {
  if (nextFirstResponder == tabController()) {
    assert(tabController() != nullptr);
    selectableTableView()->deselectTable();
    footer()->setSelectedButton(-1);
  }
}

/* SelectableTableViewDelegate */

void FunctionListController::tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) {
  // Update memoization of cell heights
  ExpressionModelListController::tableViewDidChangeSelection(t, previousSelectedCellX, previousSelectedCellY, withinTemporarySelection);
  // Do not select the cell left of the "addEmptyFunction" cell
  if (isAddEmptyRow(selectedRow()) && selectedColumn() == 0) {
    t->selectCellAtLocation(1, numberOfRows()-1);
  }
}

/* ExpressionModelListController */

StackViewController * FunctionListController::stackController() const {
  return static_cast<StackViewController *>(parentResponder()->parentResponder()->parentResponder());
}

void FunctionListController::configureFunction(Ion::Storage::Record record) {
  StackViewController * stack = stackController();
  parameterController()->setRecord(record);
  stack->push(parameterController());
}

void FunctionListController::computeTitlesColumnWidth(bool forceMax) {
  if (forceMax) {
    m_titlesColumnWidth = nameWidth(Poincare::SymbolAbstract::k_maxNameSize + Function::k_parenthesedArgumentCodePointLength - 1)+k_functionTitleSumOfMargins;
    return;
  }
  KDCoordinate maxTitleWidth = maxFunctionNameWidth()+k_functionTitleSumOfMargins;
  m_titlesColumnWidth = std::max(maxTitleWidth, k_minTitleColumnWidth);
}

TabViewController * FunctionListController::tabController() const {
  return static_cast<TabViewController *>(parentResponder()->parentResponder()->parentResponder()->parentResponder());
}

InputViewController * FunctionListController::inputController() {
  return FunctionApp::app()->inputViewController();
}

KDCoordinate FunctionListController::maxFunctionNameWidth() {
  int maxNameLength = 0;
  int numberOfModels = modelStore()->numberOfModels();
  for (int i = 0; i < numberOfModels; i++) {
    Ion::Storage::Record record = modelStore()->recordAtIndex(i);
    const char * functionName = record.fullName();
    const char * dotPosition = strchr(functionName, Ion::Storage::k_dotChar);
    assert(dotPosition != nullptr);
    maxNameLength = std::max(maxNameLength, static_cast<int>(dotPosition-functionName));
  }
  return nameWidth(maxNameLength + Function::k_parenthesedArgumentCodePointLength);
}

void FunctionListController::didChangeModelsList() {
  ExpressionModelListController::didChangeModelsList();
  computeTitlesColumnWidth();
}

KDCoordinate FunctionListController::baseline(int j) {
  if (j < 0) {
    return -1;
  }
  int currentSelectedRow = selectedRow();
  constexpr int halfMemoizationCount = k_memoizedCellsCount/2;
  if (j >= currentSelectedRow - halfMemoizationCount && j <= currentSelectedRow + halfMemoizationCount) {
    int memoizedIndex = j - (currentSelectedRow - halfMemoizationCount);
    if (m_memoizedCellBaseline[memoizedIndex] < 0) {
      m_memoizedCellBaseline[memoizedIndex] = privateBaseline(j);
    }
    return m_memoizedCellBaseline[memoizedIndex];
  }
  return privateBaseline(j);
}

void FunctionListController::resetMemoizationForIndex(int index) {
  assert(index >= 0 && index < k_memoizedCellsCount);
  m_memoizedCellBaseline[index] = -1;
  ExpressionModelListController::resetMemoizationForIndex(index);
}

void FunctionListController::shiftMemoization(bool newCellIsUnder) {
  if (newCellIsUnder) {
    for (int i = 0; i < k_memoizedCellsCount - 1; i++) {
      m_memoizedCellBaseline[i] = m_memoizedCellBaseline[i+1];
    }
  } else {
    for (int i = k_memoizedCellsCount - 1; i > 0; i--) {
      m_memoizedCellBaseline[i] = m_memoizedCellBaseline[i-1];
    }
  }
  ExpressionModelListController::shiftMemoization(newCellIsUnder);
}

KDCoordinate FunctionListController::nameWidth(int nameLength) const {
  assert(nameLength >= 0);
  return nameLength * const_cast<FunctionListController *>(this)->titleCells(0)->font()->glyphSize().width();
}

KDCoordinate FunctionListController::privateBaseline(int j) const {
  assert(j >= 0 && j < const_cast<FunctionListController *>(this)->numberOfExpressionRows());
  FunctionExpressionCell * cell = static_cast<Shared::FunctionExpressionCell *>((const_cast<SelectableTableView *>(&m_selectableTableView))->cellAtLocation(1, j));
  Poincare::Layout layout = cell->layout();
  if (layout.isUninitialized()) {
    return -1; // Baseline < 0 triggers default behaviour (centered alignment)
  }
  return 0.5*(const_cast<FunctionListController *>(this)->rowHeight(j)-layout.layoutSize().height())+layout.baseline();
}

}
