#include "variable_box_controller.h"
#include "shared/global_context.h"
#include "shared/continuous_function.h"
#include <escher/metric.h>
#include <ion/unicode/utf8_decoder.h>
#include <poincare/exception_checkpoint.h>
#include <poincare/layout_helper.h>
#include <poincare/matrix_layout.h>
#include <poincare/preferences.h>
#include <assert.h>
#include <algorithm>

using namespace Poincare;
using namespace Shared;
using namespace Ion;

VariableBoxController::VariableBoxController() :
  NestedMenuController(nullptr, I18n::Message::Variables),
  m_currentPage(Page::RootMenu),
  m_lockPageDelete(Page::RootMenu),
  m_firstMemoizedLayoutIndex(0)
{
  for (int i = 0; i < k_maxNumberOfDisplayedRows; i++) {
    m_leafCells[i].setParentResponder(&m_selectableTableView);
  }
}

void VariableBoxController::viewWillAppear() {
  assert(m_currentPage == Page::RootMenu);
  NestedMenuController::viewWillAppear();
}

void VariableBoxController::viewDidDisappear() {
  if (isDisplayingEmptyController()) {
    pop();
  }

  NestedMenuController::viewDidDisappear();

  /* NestedMenuController::viewDidDisappear might need cell heights, which would
   * use the VariableBoxController cell heights memoization. We thus reset the
   * VariableBoxController layouts only after calling the parent's
   * viewDidDisappear. */

  // Tidy the layouts displayed in the VariableBoxController to clean TreePool
  for (int i = 0; i < k_maxNumberOfDisplayedRows; i++) {
    m_leafCells[i].setLayout(Layout());
    m_leafCells[i].setAccessoryLayout(Layout());
  }
  /* We reset the page when view disappears rather than when it appears because
   * subview layout is done before viewWillAppear. If the page at that point is
   * wrong, the memoized layouts are going be filled with wrong layouts. */
  setPage(Page::RootMenu);
}

bool VariableBoxController::handleEvent(Ion::Events::Event event) {
  /* We do not want to handle backspace event if:
   * - On the root menu page
   *   The deletion on the current page is locked
   * - The empty controller is displayed
   */
  if (event == Ion::Events::Backspace && m_currentPage != Page::RootMenu && m_lockPageDelete != m_currentPage && !isDisplayingEmptyController()) {
    int rowIndex = selectedRow();
    m_selectableTableView.deselectTable();
    destroyRecordAtRowIndex(rowIndex);
    int newSelectedRow = rowIndex >= numberOfRows() ? numberOfRows()-1 : rowIndex;
    selectCellAtLocation(selectedColumn(), newSelectedRow);
    m_selectableTableView.reloadData();
    displayEmptyController();
    return true;
  }
  return NestedMenuController::handleEvent(event);
}

int VariableBoxController::numberOfRows() const {
  switch (m_currentPage) {
    case Page::RootMenu:
      return k_numberOfMenuRows;
    case Page::Expression:
      return Storage::sharedStorage()->numberOfRecordsWithExtension(Ion::Storage::expExtension);
    case Page::Function:
      return Storage::sharedStorage()->numberOfRecordsWithExtension(Ion::Storage::funcExtension);
    default:
      return 0;
  }
}

int VariableBoxController::reusableCellCount(int type) {
  assert(type < 2);
  if (type == 0) {
    return k_maxNumberOfDisplayedRows;
  }
  return k_numberOfMenuRows;
}

void VariableBoxController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (m_currentPage == Page::RootMenu) {
    I18n::Message label = nodeLabelAtIndex(index);
    MessageTableCell * myCell = (MessageTableCell *)cell;
    myCell->setMessage(label);
    myCell->reloadCell();
    return;
  }
  ExpressionTableCellWithExpression * myCell = (ExpressionTableCellWithExpression *)cell;
  Storage::Record record = recordAtIndex(index);
  char symbolName[Shared::Function::k_maxNameWithArgumentSize];
  size_t symbolLength = 0;
  if (m_currentPage == Page::Expression) {
    static_assert(Shared::Function::k_maxNameWithArgumentSize > Poincare::SymbolAbstract::k_maxNameSize, "Forgot argument's size?");
    symbolLength = SymbolAbstract::TruncateExtension(symbolName, record.fullName(), SymbolAbstract::k_maxNameSize);
  } else {
    assert(m_currentPage == Page::Function);
    ContinuousFunction f(record);
    symbolLength = f.nameWithArgument(
        symbolName,
        Shared::Function::k_maxNameWithArgumentSize
    );
  }
  Layout symbolLayout = LayoutHelper::String(symbolName, symbolLength);
  myCell->setLayout(symbolLayout);
  myCell->setAccessoryLayout(expressionLayoutForRecord(record, index));
  myCell->reloadScroll();
  myCell->reloadCell();
}

KDCoordinate VariableBoxController::rowHeight(int index) {
  if (m_currentPage != Page::RootMenu) {
    Layout layoutR = expressionLayoutForRecord(recordAtIndex(index), index);
    if (!layoutR.isUninitialized()) {
      return std::max<KDCoordinate>(layoutR.layoutSize().height()+k_leafMargin, Metric::ToolboxRowHeight);
    }
  }
  return NestedMenuController::rowHeight(index);
}

int VariableBoxController::typeAtLocation(int i, int j) {
  if (m_currentPage == Page::RootMenu) {
    return 1;
  }
  return 0;
}

ExpressionTableCellWithExpression * VariableBoxController::leafCellAtIndex(int index) {
  assert(index >= 0 && index < k_maxNumberOfDisplayedRows);
  return &m_leafCells[index];
}

MessageTableCellWithChevron * VariableBoxController::nodeCellAtIndex(int index) {
  assert(index >= 0 && index < k_numberOfMenuRows);
  return &m_nodeCells[index];
}

VariableBoxController::Page VariableBoxController::pageAtIndex(int index) {
  Page pages[2] = {Page::Expression, Page::Function};
  return pages[index];
}

void VariableBoxController::setPage(Page page) {
  m_currentPage = page;
  resetMemoization();
}

bool VariableBoxController::selectSubMenu(int selectedRow) {
  m_selectableTableView.deselectTable();
  setPage(pageAtIndex(selectedRow));
  bool selectSubMenu = NestedMenuController::selectSubMenu(selectedRow);
  if (displayEmptyController()) {
    return true;
  }
  return selectSubMenu;
}

bool VariableBoxController::returnToPreviousMenu() {
  if (isDisplayingEmptyController()) {
    pop();
  } else {
    m_selectableTableView.deselectTable();
  }
  setPage(Page::RootMenu);
  return NestedMenuController::returnToPreviousMenu();
}

bool VariableBoxController::selectLeaf(int selectedRow) {
  if (isDisplayingEmptyController()) {
    /* We do not want to handle OK/EXE events in that case. */
    return false;
  }

  // Deselect the table
  assert(selectedRow >= 0 && selectedRow < numberOfRows());
  m_selectableTableView.deselectTable();

  // Get the name text to insert
  Storage::Record record = recordAtIndex(selectedRow);
  constexpr size_t nameToHandleMaxSize = Shared::Function::k_maxNameWithArgumentSize;
  char nameToHandle[nameToHandleMaxSize];
  size_t nameLength = SymbolAbstract::TruncateExtension(nameToHandle, record.fullName(), nameToHandleMaxSize);

  if (m_currentPage == Page::Function) {
    // Add parentheses to a function name
    assert(nameLength < nameToHandleMaxSize);
    nameLength += UTF8Decoder::CodePointToChars('(', nameToHandle+nameLength, nameToHandleMaxSize - nameLength);
    assert(nameLength < nameToHandleMaxSize);
    nameLength+= UTF8Decoder::CodePointToChars(UCodePointEmpty, nameToHandle+nameLength, nameToHandleMaxSize - nameLength);
    assert(nameLength < nameToHandleMaxSize);
    nameLength += UTF8Decoder::CodePointToChars(')', nameToHandle+nameLength, nameToHandleMaxSize - nameLength);
    assert(nameLength < nameToHandleMaxSize);
    nameToHandle[nameLength] = 0;
  }

  // Handle the text
  sender()->handleEventWithText(nameToHandle);
  Container::activeApp()->dismissModalViewController();
  return true;
}

I18n::Message VariableBoxController::nodeLabelAtIndex(int index) {
  assert(m_currentPage == Page::RootMenu);
  I18n::Message labels[2] = {I18n::Message::Expressions, I18n::Message::Functions};
  return labels[index];
}

Layout VariableBoxController::expressionLayoutForRecord(Storage::Record record, int index) {
  assert(m_currentPage != Page::RootMenu);
  assert(index >= 0);
  if (index >= m_firstMemoizedLayoutIndex+k_maxNumberOfDisplayedRows || index < m_firstMemoizedLayoutIndex) {
    // Change range of layout memoization
    int deltaIndex = index >= m_firstMemoizedLayoutIndex + k_maxNumberOfDisplayedRows ? index - k_maxNumberOfDisplayedRows + 1 - m_firstMemoizedLayoutIndex : index - m_firstMemoizedLayoutIndex;
    for (int i = 0; i < k_maxNumberOfDisplayedRows; i++) {
      int j = deltaIndex + i;
      m_layouts[i] = (j >= m_firstMemoizedLayoutIndex && j < k_maxNumberOfDisplayedRows) ? m_layouts[j] : Layout();
    }
    m_firstMemoizedLayoutIndex += deltaIndex;
    assert(m_firstMemoizedLayoutIndex >= 0);
  }
  assert(index >= m_firstMemoizedLayoutIndex && index < m_firstMemoizedLayoutIndex + k_maxNumberOfDisplayedRows);
  if (m_layouts[index-m_firstMemoizedLayoutIndex].isUninitialized()) {
    /* Creating the layout of a very long variable might throw a pool exception.
     * We want to catch it and return a dummy layout instead, otherwise the user
     * won't be able to open the variable box again, until she deletes the
     * problematic variable -> and she has no help to remember its name, as she
     * can't open the variable box. */
    Layout result;
    Poincare::ExceptionCheckpoint ecp;
    if (ExceptionRun(ecp)) {
      result = GlobalContext::LayoutForRecord(record);
    }
    m_layouts[index-m_firstMemoizedLayoutIndex] = result;
  }
  return m_layouts[index-m_firstMemoizedLayoutIndex];
}

const char * VariableBoxController::extension() const {
  assert(m_currentPage != Page::RootMenu);
  return m_currentPage == Page::Function ? Ion::Storage::funcExtension : Ion::Storage::expExtension;
}

Storage::Record VariableBoxController::recordAtIndex(int rowIndex) {
  assert(m_currentPage != Page::RootMenu);
  assert(!Storage::sharedStorage()->recordWithExtensionAtIndex(extension(), rowIndex).isNull());
  return Storage::sharedStorage()->recordWithExtensionAtIndex(extension(), rowIndex);
}

bool VariableBoxController::displayEmptyController() {
  assert(!isDisplayingEmptyController());
  // If the content is empty, we push above an empty controller.
  if (numberOfRows() == 0) {
    m_emptyViewController.setType((VariableBoxEmptyController::Type)m_currentPage);
    push(&m_emptyViewController);
    return true;
  }
  return false;
}

void VariableBoxController::resetMemoization() {
  for (int i = 0; i < k_maxNumberOfDisplayedRows; i++) {
    m_layouts[i] = Layout();
  }
  m_firstMemoizedLayoutIndex = 0;
}

void VariableBoxController::destroyRecordAtRowIndex(int rowIndex) {
  // Destroy the record
  recordAtIndex(rowIndex).destroy();
  // Shift the memoization if needed
  if (rowIndex >= m_firstMemoizedLayoutIndex + k_maxNumberOfDisplayedRows) {
    // The deleted row is after the memoization
    return;
  }
  for (int i = std::max(0, rowIndex - m_firstMemoizedLayoutIndex); i < k_maxNumberOfDisplayedRows - 1; i++) {
    m_layouts[i] = m_layouts[i+1];
  }
  m_layouts[k_maxNumberOfDisplayedRows - 1] = Layout();
}
