#include "storage_function_list_controller.h"
#include "storage_function_app.h"

namespace Shared {

static inline int max(int x, int y) { return x > y ? x : y; }

StorageFunctionListController::StorageFunctionListController(Responder * parentResponder, ButtonRowController * header, ButtonRowController * footer, I18n::Message text) :
  StorageExpressionModelListController(parentResponder, text),
  ButtonRowDelegate(header, footer),
  m_selectableTableView(this, this, this, this),
  m_emptyCell(),
  m_plotButton(this, I18n::Message::Plot, Invocation([](void * context, void * sender) {
      StorageFunctionListController * list = (StorageFunctionListController *)context;
      TabViewController * tabController = list->tabController();
      tabController->setActiveTab(1);
    }, this), KDFont::SmallFont, Palette::PurpleBright),
  m_valuesButton(this, I18n::Message::DisplayValues, Invocation([](void * context, void * sender) {
      StorageFunctionListController * list = (StorageFunctionListController *)context;
      TabViewController * tabController = list->tabController();
      tabController->setActiveTab(2);
    }, this), KDFont::SmallFont, Palette::PurpleBright),
  m_titlesColumnWidth(k_minTitleColumnWidth),
  m_memoizedCellHeight {-1, -1, -1, -1, -1},
  m_cumulatedHeightForSelectedIndex(-1)
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
  if (j < 0) {
    return 0;
  }
  int currentSelectedRow = selectedRow();
  constexpr int halfMemoizationCount = k_memoizedCellHeightsCount/2;
  if (j >= currentSelectedRow - halfMemoizationCount && j <= currentSelectedRow + halfMemoizationCount) {
    int memoizedIndex = j - (currentSelectedRow - halfMemoizationCount);
    if (m_memoizedCellHeight[memoizedIndex] < 0) {
      m_memoizedCellHeight[memoizedIndex] = expressionRowHeight(j);
    }
    return m_memoizedCellHeight[memoizedIndex];
  }
  return expressionRowHeight(j);
}

KDCoordinate StorageFunctionListController::cumulatedHeightFromIndex(int j) {
  if (j <= 0) {
    return 0;
  }
  int currentSelectedRow = selectedRow();
  constexpr int halfMemoizationCount = k_memoizedCellHeightsCount/2;
  /* If j is not easily computable from the memoized values, compute it the hard
   * way. */
  if (j < currentSelectedRow - halfMemoizationCount || j > currentSelectedRow + halfMemoizationCount) {
    return TableViewDataSource::cumulatedHeightFromIndex(j);
  }
  // Recompute the memoized cumulatedHeight if needed
  if (m_cumulatedHeightForSelectedIndex < 0) {
    m_cumulatedHeightForSelectedIndex = TableViewDataSource::cumulatedHeightFromIndex(currentSelectedRow);
  }
  /* Compute the wanted cumulated height by adding/removing memoized cell
   * heights */
  KDCoordinate result = m_cumulatedHeightForSelectedIndex;
  if (j <= currentSelectedRow) {
    /* If j is smaller than the selected row, remove cell heights from the
     * memoized value */
    for (int i = j; i < currentSelectedRow; i++) {
      result -= rowHeight(i);
    }
  } else {
    /* If j is bigger than the selected row, add cell heights to the memoized
     * value */
    assert(j > currentSelectedRow && j <= currentSelectedRow + halfMemoizationCount);
    for (int i = currentSelectedRow; i < j; i++) {
      result += rowHeight(i);
    }
  }
  return result;
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
  constexpr int currentSelectedMemoizedIndex = k_memoizedCellHeightsCount/2 + 1; // Needs k_memoizedCellHeightsCount to be odd, which is static asserted in storage_function_list_controller.h
  int currentSelectedY = selectedRow();

  // The previously selected cell's height might have changed.
  m_memoizedCellHeight[currentSelectedMemoizedIndex] = -1;

  // Update m_cumulatedHeightForSelectedIndex if we scrolled one cell up/down
  if (currentSelectedY == previousSelectedCellY + 1) {
    /* We selected the cell under the previous cell. Shift the memoized cell
     * heights. */
    for (int i = 0; i < k_memoizedCellHeightsCount - 1; i++) {
      m_memoizedCellHeight[i] = m_memoizedCellHeight[i+1];
    }
    m_memoizedCellHeight[k_memoizedCellHeightsCount-1] = -1;
    // Update m_cumulatedHeightForSelectedIndex
    if (previousSelectedCellY >= 0) {
      m_cumulatedHeightForSelectedIndex+= rowHeight(previousSelectedCellY);
    } else {
      assert(currentSelectedY == 0);
      m_cumulatedHeightForSelectedIndex = 0;
    }
  } else if (currentSelectedY == previousSelectedCellY - 1) {
    /* We selected the cell above the previous cell. Shift the memoized cell
     * heights. */
    for (int i = k_memoizedCellHeightsCount - 1; i > 0; i--) {
      m_memoizedCellHeight[i] = m_memoizedCellHeight[i-1];
    }
    m_memoizedCellHeight[0] = -1;
    // Update m_cumulatedHeightForSelectedIndex
    if (currentSelectedY >= 0) {
      m_cumulatedHeightForSelectedIndex-= rowHeight(currentSelectedY);
    } else {
      m_cumulatedHeightForSelectedIndex = 0;
    }
  } else if (previousSelectedCellY != currentSelectedY) {
    resetMemoization();
  }

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

void StorageFunctionListController::resetMemoization() {
  m_cumulatedHeightForSelectedIndex = -1;
  for (int i = 0; i < k_memoizedCellHeightsCount; i++) {
    m_memoizedCellHeight[i] = -1;
  }
}

}
