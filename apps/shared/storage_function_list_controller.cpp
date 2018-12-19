#include "storage_function_list_controller.h"
#include "storage_function_app.h"

namespace Shared {

static inline int max(int x, int y) { return x > y ? x : y; }
static inline int min(int x, int y) { return x < y ? x : y; }

StorageFunctionListController::StorageFunctionListController(Responder * parentResponder, ButtonRowController * header, ButtonRowController * footer, I18n::Message text) :
  StorageExpressionModelListController(parentResponder, text),
  ButtonRowDelegate(header, footer),
  m_selectableTableView(this, this, this, this),
  m_emptyCell(),
  m_plotButton(this, I18n::Message::Plot, Invocation([](void * context, void * sender) {
      StorageFunctionListController * list = (StorageFunctionListController *)context;
      TabViewController * tabController = list->tabController();
      tabController->setActiveTab(1);
      return true;
    }, this), KDFont::SmallFont, Palette::PurpleBright),
  m_valuesButton(this, I18n::Message::DisplayValues, Invocation([](void * context, void * sender) {
      StorageFunctionListController * list = (StorageFunctionListController *)context;
      TabViewController * tabController = list->tabController();
      tabController->setActiveTab(2);
      return true;
    }, this), KDFont::SmallFont, Palette::PurpleBright),
  m_titlesColumnWidth(k_minTitleColumnWidth)
{
  m_selectableTableView.setMargins(0);
  m_selectableTableView.setVerticalCellOverlap(0);
  m_selectableTableView.setShowsIndicators(false);
}

/* TableViewDataSource */

void StorageFunctionListController::viewWillAppear() {
  computeTitlesColumnWidth();
}

KDCoordinate StorageFunctionListController::rowHeight(int j) {
  return StorageExpressionModelListController::memoizedRowHeight(j);
}

KDCoordinate StorageFunctionListController::cumulatedHeightFromIndex(int j) {
  return StorageExpressionModelListController::memoizedCumulatedHeightFromIndex(j);
}

KDCoordinate StorageFunctionListController::columnWidth(int i) {
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

KDCoordinate StorageFunctionListController::cumulatedWidthFromIndex(int i) {
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

int StorageFunctionListController::indexFromCumulatedWidth(KDCoordinate offsetX) {
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

int StorageFunctionListController::indexFromCumulatedHeight(KDCoordinate offsetY) {
  if (offsetY == 0) {
    return 0;
  }

  /* We use memoization to speed up this method: if offsetY is "around" the
   * memoized cumulatedHeightForIndex, we can compute its value easily by
   * adding/substracting memoized row heights. */

  int currentSelectedRow = selectedRow();
  int rowsCount = numberOfRows();
  if (rowsCount <= 1 || currentSelectedRow < 1) {
    return TableViewDataSource::indexFromCumulatedHeight(offsetY);
  }

  KDCoordinate currentCumulatedHeight = cumulatedHeightFromIndex(currentSelectedRow);
  if (offsetY > currentCumulatedHeight) {
    int iMax = min(k_memoizedCellHeightsCount/2 + 1, rowsCount - currentSelectedRow);
    for (int i = 0; i < iMax; i++) {
      currentCumulatedHeight+= rowHeight(currentSelectedRow + i);
      if (offsetY <= currentCumulatedHeight) {
        return currentSelectedRow + i;
      }
    }
  } else {
    int iMax = min(k_memoizedCellHeightsCount/2, currentSelectedRow);
    for (int i = 1; i <= iMax; i++) {
      currentCumulatedHeight-= rowHeight(currentSelectedRow-i);
      if (offsetY > currentCumulatedHeight) {
        return currentSelectedRow - i;
      }
    }
  }
  return TableViewDataSource::indexFromCumulatedHeight(offsetY);
}

int StorageFunctionListController::typeAtLocation(int i, int j) {
  if (isAddEmptyRow(j)){
    return i + 2;
  }
  return i;
}

HighlightCell * StorageFunctionListController::reusableCell(int index, int type) {
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

int StorageFunctionListController::reusableCellCount(int type) {
  if (type > 1) {
    return 1;
  }
  return maxNumberOfDisplayableRows();
}

void StorageFunctionListController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
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

int StorageFunctionListController::numberOfButtons(ButtonRowController::Position position) const {
  if (position == ButtonRowController::Position::Bottom) {
    return 2;
  }
  return 0;
}

Button * StorageFunctionListController::buttonAtIndex(int index, ButtonRowController::Position position) const {
  if (position == ButtonRowController::Position::Top) {
    return nullptr;
  }
  const Button * buttons[2] = {&m_plotButton, &m_valuesButton};
  return (Button *)buttons[index];
}

/* Responder */

void StorageFunctionListController::didBecomeFirstResponder() {
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

bool StorageFunctionListController::handleEvent(Ion::Events::Event event) {
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
  return false;
}

void StorageFunctionListController::didEnterResponderChain(Responder * previousFirstResponder) {
  selectableTableView()->reloadData();
}

void StorageFunctionListController::willExitResponderChain(Responder * nextFirstResponder) {
  if (nextFirstResponder == tabController()) {
    selectableTableView()->deselectTable();
    footer()->setSelectedButton(-1);
  }
}

/* SelectableTableViewDelegate */

void StorageFunctionListController::tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) {
  // Update memoization of cell heights
  StorageExpressionModelListController::tableSelectionDidChange(previousSelectedCellY);
  // Do not select the cell left of the "addEmptyFunction" cell
  if (isAddEmptyRow(selectedRow()) && selectedColumn() == 0) {
    t->selectCellAtLocation(1, numberOfRows()-1);
  }
}

/* ExpressionModelListController */

StackViewController * StorageFunctionListController::stackController() const {
  return static_cast<StackViewController *>(parentResponder()->parentResponder()->parentResponder());
}

void StorageFunctionListController::configureFunction(Ion::Storage::Record record) {
  StackViewController * stack = stackController();
  parameterController()->setRecord(record);
  stack->push(parameterController());
}

void StorageFunctionListController::computeTitlesColumnWidth() {
  KDCoordinate maxTitleWidth = maxFunctionNameWidth()+k_functionTitleSumOfMargins;
  m_titlesColumnWidth = maxTitleWidth < k_minTitleColumnWidth ? k_minTitleColumnWidth : maxTitleWidth;
}

TabViewController * StorageFunctionListController::tabController() const {
  return static_cast<TabViewController *>(parentResponder()->parentResponder()->parentResponder()->parentResponder());
}

StorageFunctionStore * StorageFunctionListController::modelStore() {
  StorageFunctionApp * myApp = static_cast<StorageFunctionApp *>(app());
  return myApp->functionStore();
}

InputViewController * StorageFunctionListController::inputController() {
  StorageFunctionApp * myApp = static_cast<StorageFunctionApp *>(app());
  return myApp->inputViewController();
}

KDCoordinate StorageFunctionListController::maxFunctionNameWidth() {
  int maxNameLength = 0;
  int numberOfModels = modelStore()->numberOfModels();
  for (int i = 0; i < numberOfModels; i++) {
    Ion::Storage::Record record = modelStore()->recordAtIndex(i);
    const char * functionName = record.fullName();
    const char * dotPosition = strchr(functionName, Ion::Storage::k_dotChar);
    assert(dotPosition != nullptr);
    maxNameLength = max(maxNameLength, dotPosition-functionName);
  }
  return (maxNameLength + StorageFunction::k_parenthesedArgumentLength) * titleCells(0)->font()->glyphSize().width();
}

void StorageFunctionListController::didChangeModelsList() {
  computeTitlesColumnWidth();
}

KDCoordinate StorageFunctionListController::notMemoizedCumulatedHeightFromIndex(int j) {
  return TableViewDataSource::cumulatedHeightFromIndex(j);
}

}
