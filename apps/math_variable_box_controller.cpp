#include "math_variable_box_controller.h"
#include "shared/global_context.h"
#include "shared/continuous_function.h"
#include <escher/metric.h>
#include <escher/message_table_cell_with_message.h>
#include <ion/unicode/utf8_decoder.h>
#include <poincare/exception_checkpoint.h>
#include <poincare/expression.h>
#include <poincare/layout_helper.h>
#include <poincare/matrix_layout.h>
#include <poincare/preferences.h>
#include <poincare/print.h>
#include <assert.h>
#include <algorithm>
#include <apps/shared/sequence.h>
#include <apps/apps_container.h>
#include "global_preferences.h"

using namespace Poincare;
using namespace Shared;
using namespace Ion;
using namespace Escher;

MathVariableBoxController::MathVariableBoxController() :
  AlternateEmptyNestedMenuController(I18n::Message::Variables),
  m_currentPage(Page::RootMenu),
  m_lockPageDelete(Page::RootMenu),
  m_firstMemoizedLayoutIndex(0)
{
  for (int i = 0; i < k_maxNumberOfDisplayedRows; i++) {
    m_leafCells[i].setParentResponder(&m_selectableTableView);
    m_leafCells[i].setSubLabelFont(k_subLabelFont);
  }
}

void MathVariableBoxController::viewWillAppear() {
  AlternateEmptyNestedMenuController::viewWillAppear();
  displayEmptyControllerIfNeeded();
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
    m_leafCells[i].setSubLabelLayout(Layout());
  }

  /* We need to remove the memoized layouts otherwise we risk leaking them into
   * the pool when quitting the app. */
  resetVarBoxMemoization();
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
    resetMemoization();
    m_selectableTableView.reloadData();
    displayEmptyControllerIfNeeded();
    return true;
  }
  return AlternateEmptyNestedMenuController::handleEvent(event);
}

int MathVariableBoxController::numberOfRows() const {
  return numberOfElements(m_currentPage);
}

int MathVariableBoxController::numberOfElements(Page page) const {
  switch (page) {
    case Page::RootMenu:
      return k_numberOfMenuRows;
    case Page::Expression:
      return Storage::FileSystem::sharedFileSystem()->numberOfRecordsWithExtension(Ion::Storage::expExtension);
    case Page::Function:
      return Storage::FileSystem::sharedFileSystem()->numberOfRecordsStartingWithout(ContinuousFunction::k_unnamedRecordFirstChar, Ion::Storage::funcExtension);
    case Page::Sequence:
      return Storage::FileSystem::sharedFileSystem()->numberOfRecordsWithExtension(Ion::Storage::seqExtension);
    case Page::List:
      return Storage::FileSystem::sharedFileSystem()->numberOfRecordsWithExtension(Ion::Storage::lisExtension);
    case Page::Matrix:
      return Storage::FileSystem::sharedFileSystem()->numberOfRecordsWithExtension(Ion::Storage::matExtension);
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
    MessageTableCellWithChevronAndBuffer * myCell = static_cast<MessageTableCellWithChevronAndBuffer *>(cell);
    int nb = numberOfElements(static_cast<Page>(index + 1));
    constexpr size_t bufferSize = 20;
    char buffer[bufferSize];
    Print::CustomPrintf(buffer, bufferSize, "%i elements", nb);
    myCell->setMessage(label);
    myCell->setSubLabelText(buffer);
    myCell->reloadCell();
    return;
  }
  ExpressionTableCellWithExpression * myCell = static_cast<ExpressionTableCellWithExpression *>(cell);
  Storage::Record record = recordAtIndex(index);
  char symbolName[Shared::Function::k_maxNameWithArgumentSize];
  size_t symbolLength = 0;
  Layout symbolLayout;
  if (m_currentPage == Page::Expression || m_currentPage == Page::List || m_currentPage == Page::Matrix) {
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
  myCell->setSubLabelLayout(expressionLayoutForRecord(record, index));
  myCell->reloadScroll();
  myCell->reloadCell();
}

KDCoordinate MathVariableBoxController::nonMemoizedRowHeight(int index) {
  if (m_currentPage == Page::RootMenu) {
    MessageTableCellWithChevronAndBuffer tempCell;
    return heightForCellAtIndexWithWidthInit(&tempCell, index);
  }
  ExpressionTableCellWithExpression tempCell;
  return heightForCellAtIndexWithWidthInit(&tempCell, index);
}

int MathVariableBoxController::typeAtIndex(int index) const {
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

I18n::Message MathVariableBoxController::subTitle() {
  return nodeLabelAtIndex(static_cast<int>(m_currentPage) - 1);
}

MathVariableBoxController::Page MathVariableBoxController::pageAtIndex(int index) {
  assert(index >= 0 && index < k_numberOfMenuRows);
  return static_cast<Page>(index + 1);
}

void MathVariableBoxController::setPage(Page page) {
  m_currentPage = page;
  resetVarBoxMemoization();
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

bool MathVariableBoxController::returnToRootMenu() {
  assert(stackDepth() == 1);
  return returnToPreviousMenu();
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
    // Add parentheses to a function name, and braces to a sequence
    char openingChar = m_currentPage == Page::Function ? '(' : '{';
    char closingChar = m_currentPage == Page::Function ? ')' : '}';
    assert(nameLength < nameToHandleMaxSize);
    nameLength += UTF8Decoder::CodePointToChars(openingChar, nameToHandle + nameLength, nameToHandleMaxSize - nameLength - 1);
    nameLength += UTF8Decoder::CodePointToChars(UCodePointEmpty, nameToHandle + nameLength, nameToHandleMaxSize - nameLength - 1);
    nameLength += UTF8Decoder::CodePointToChars(closingChar, nameToHandle + nameLength, nameToHandleMaxSize - nameLength - 1);
    assert(nameLength < nameToHandleMaxSize);
    nameToHandle[nameLength] = 0;
  }

  // Handle the text
  sender()->handleEventWithText(nameToHandle);
  Container::activeApp()->dismissModalViewController();
  return true;
}

I18n::Message MathVariableBoxController::nodeLabelAtIndex(int index) {
  assert(index >= 0 && index < k_numberOfMenuRows);
  Page page = pageAtIndex(index);
  switch (page) {
    case Page::Expression:
      return I18n::Message::Expressions;
    case Page::Function:
      return I18n::Message::Functions;
    case Page::Sequence:
      return I18n::Message::Sequences;
    case Page::List:
      return I18n::Message::Lists;
    case Page::Matrix:
      return I18n::Message::Matrices;
    default:
      assert(false);
      return (I18n::Message)0;
  }
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
  } else if (m_currentPage == Page::List) {
    return Ion::Storage::lisExtension;
  } else if (m_currentPage == Page::Matrix) {
    return Ion::Storage::matExtension;
  } else {
    assert(m_currentPage == Page::Sequence);
    return Ion::Storage::seqExtension;
  }
}

Storage::Record MathVariableBoxController::recordAtIndex(int rowIndex) {
  assert(m_currentPage != Page::RootMenu);
  Storage::Record record;
  if (m_currentPage == Page::Function) {
    record = Storage::FileSystem::sharedFileSystem()->recordWithExtensionAtIndexStartingWithout(ContinuousFunction::k_unnamedRecordFirstChar, extension(), rowIndex);
  } else {
    record = Storage::FileSystem::sharedFileSystem()->recordWithExtensionAtIndex(extension(), rowIndex);
  }
  assert(!record.isNull());
  return record;
}

ViewController * MathVariableBoxController::emptyViewController() {
  m_emptyViewController.setPage(static_cast<int>(m_currentPage));
  return &m_emptyViewController;
}

void MathVariableBoxController::resetVarBoxMemoization() {
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
