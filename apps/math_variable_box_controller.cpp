#include "math_variable_box_controller.h"
#include "shared/global_context.h"
#include "shared/continuous_function.h"
#include <escher/metric.h>
#include <ion/unicode/utf8_decoder.h>
#include <poincare/exception_checkpoint.h>
#include <poincare/expression.h>
#include <poincare/layout_helper.h>
#include <poincare/matrix_layout.h>
#include <poincare/preferences.h>
#include <assert.h>
#include <algorithm>
#include <apps/shared/sequence.h>
#include <apps/apps_container.h>
#include "global_preferences.h"

using namespace Poincare;
using namespace Shared;
using namespace Ion;

MathVariableBoxController::MathVariableBoxController() :
  AlternateEmptyNestedMenuController(I18n::Message::Variables),
  m_currentPage(Page::RootMenu),
  m_lockPageDelete(Page::RootMenu),
  m_firstMemoizedLayoutIndex(0)
{
  for (int i = 0; i < k_maxNumberOfDisplayedRows; i++) {
    m_leafCells[i].setParentResponder(&m_selectableTableView);
  }
}

void MathVariableBoxController::viewWillAppear() {
  assert(m_currentPage == Page::RootMenu);
  AlternateEmptyNestedMenuController::viewWillAppear();
}

void MathVariableBoxController::viewDidDisappear() {
  AlternateEmptyNestedMenuController::viewDidDisappear();

  /* NestedMenuController::viewDidDisappear might need cell heights, which would
   * use the MathVariableBoxController cell heights memoization. We thus reset the
   * MathVariableBoxController layouts only after calling the parent's
   * viewDidDisappear. */

  // Tidy the layouts displayed in the MathVariableBoxController to clean TreePool
  for (int i = 0; i < k_maxNumberOfDisplayedRows; i++) {
    m_leafCells[i].setLayout(Layout());
    m_leafCells[i].setAccessoryLayout(Layout());
  }
  /* We reset the page when view disappears rather than when it appears because
   * subview layout is done before viewWillAppear. If the page at that point is
   * wrong, the memoized layouts are going be filled with wrong layouts. */
  setPage(Page::RootMenu);
}

bool MathVariableBoxController::handleEvent(Ion::Events::Event event) {
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
    displayEmptyControllerIfNeeded();
    return true;
  }
  return AlternateEmptyNestedMenuController::handleEvent(event);
}

int MathVariableBoxController::numberOfRows() const {
  switch (m_currentPage) {
    case Page::RootMenu:
      return k_numberOfMenuRows;
    case Page::Expression:
      return Storage::sharedStorage()->numberOfRecordsWithExtension(Ion::Storage::expExtension);
    case Page::Function:
      return Storage::sharedStorage()->numberOfRecordsWithExtension(Ion::Storage::funcExtension);
    case Page::Sequence:
      return Storage::sharedStorage()->numberOfRecordsWithExtension(Ion::Storage::seqExtension);
    default:
      return 0;
  }
}

int MathVariableBoxController::reusableCellCount(int type) {
  assert(type < 2);
  if (type == 0) {
    return k_maxNumberOfDisplayedRows;
  }
  return k_numberOfMenuRows;
}

void MathVariableBoxController::willDisplayCellForIndex(HighlightCell * cell, int index) {
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
  Layout symbolLayout;
  if (m_currentPage == Page::Expression) {
    static_assert(Shared::Function::k_maxNameWithArgumentSize > Poincare::SymbolAbstract::k_maxNameSize, "Forgot argument's size?");
    symbolLength = SymbolAbstract::TruncateExtension(symbolName, record.fullName(), SymbolAbstract::k_maxNameSize);
  } else if (m_currentPage == Page::Function) {
    ContinuousFunction f(record);
    symbolLength = f.nameWithArgument(
        symbolName,
        Shared::Function::k_maxNameWithArgumentSize
    );
  } else {
    assert(m_currentPage == Page::Sequence);
    Shared::Sequence u(record);
    symbolLength = u.nameWithArgumentAndType(
        symbolName,
        Shared::Sequence::k_maxNameWithArgumentSize
    );
    symbolLayout = u.definitionName();
  }
  if (symbolLayout.isUninitialized()) {
    symbolLayout = LayoutHelper::String(symbolName, symbolLength);
  }
  myCell->setLayout(symbolLayout);
  myCell->setAccessoryLayout(expressionLayoutForRecord(record, index));
  myCell->reloadScroll();
  myCell->reloadCell();
}

KDCoordinate MathVariableBoxController::rowHeight(int index) {
  if (m_currentPage != Page::RootMenu) {
    Layout layoutR = expressionLayoutForRecord(recordAtIndex(index), index);
    if (!layoutR.isUninitialized()) {
      return std::max<KDCoordinate>(layoutR.layoutSize().height()+k_leafMargin, Metric::ToolboxRowHeight);
    }
  }
  return AlternateEmptyNestedMenuController::rowHeight(index);
}

int MathVariableBoxController::typeAtLocation(int i, int j) {
  if (m_currentPage == Page::RootMenu) {
    return 1;
  }
  return 0;
}

ExpressionTableCellWithExpression * MathVariableBoxController::leafCellAtIndex(int index) {
  assert(index >= 0 && index < k_maxNumberOfDisplayedRows);
  return &m_leafCells[index];
}

MessageTableCellWithChevron * MathVariableBoxController::nodeCellAtIndex(int index) {
  assert(index >= 0 && index < k_numberOfMenuRows);
  return &m_nodeCells[index];
}

MathVariableBoxController::Page MathVariableBoxController::pageAtIndex(int index) {
  Page pages[k_numberOfMenuRows] = {Page::Expression, Page::Function, Page::Sequence};
  return pages[index];
}

void MathVariableBoxController::setPage(Page page) {
  m_currentPage = page;
  resetMemoization();
}

bool MathVariableBoxController::selectSubMenu(int selectedRow) {
  m_selectableTableView.deselectTable();
  setPage(pageAtIndex(selectedRow));
  bool selectSubMenu = AlternateEmptyNestedMenuController::selectSubMenu(selectedRow);
  if (displayEmptyControllerIfNeeded()) {
    return true;
  }
  return selectSubMenu;
}

bool MathVariableBoxController::returnToPreviousMenu() {
  if (isDisplayingEmptyController()) {
    pop();
  } else {
    m_selectableTableView.deselectTable();
  }
  setPage(Page::RootMenu);
  return AlternateEmptyNestedMenuController::returnToPreviousMenu();
}

bool MathVariableBoxController::selectLeaf(int selectedRow) {
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

  if (m_currentPage == Page::Function || m_currentPage == Page::Sequence) {
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

I18n::Message MathVariableBoxController::nodeLabelAtIndex(int index) {
  assert(m_currentPage == Page::RootMenu);
  I18n::Message labels[k_numberOfMenuRows] = {I18n::Message::Expressions, I18n::Message::Functions, I18n::Message::Sequences};
  return labels[index];
}

Layout MathVariableBoxController::expressionLayoutForRecord(Storage::Record record, int index) {
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

const char * MathVariableBoxController::extension() const {
  assert(m_currentPage != Page::RootMenu);
  if (m_currentPage == Page::Function) {
    return Ion::Storage::funcExtension;
  } else if (m_currentPage == Page::Expression) {
    return Ion::Storage::expExtension;
  } else {
    assert(m_currentPage == Page::Sequence);
    return Ion::Storage::seqExtension;
  }
}

Storage::Record MathVariableBoxController::recordAtIndex(int rowIndex) {
  assert(m_currentPage != Page::RootMenu);
  assert(!Storage::sharedStorage()->recordWithExtensionAtIndex(extension(), rowIndex).isNull());
  return Storage::sharedStorage()->recordWithExtensionAtIndex(extension(), rowIndex);
}

ViewController * MathVariableBoxController::emptyViewController() {
  m_emptyViewController.setType((MathVariableBoxEmptyController::Type)m_currentPage);
  return &m_emptyViewController;
}

void MathVariableBoxController::resetMemoization() {
  for (int i = 0; i < k_maxNumberOfDisplayedRows; i++) {
    m_layouts[i] = Layout();
  }
  m_firstMemoizedLayoutIndex = 0;
}

void MathVariableBoxController::destroyRecordAtRowIndex(int rowIndex) {
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
