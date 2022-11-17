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
  NestedMenuController(nullptr, I18n::Message::Variables),
  m_currentPage(Page::RootMenu),
  m_defineVariableCell(I18n::Message::DefineVariable),
  m_firstMemoizedLayoutIndex(0)
{
  for (int i = 0; i < k_maxNumberOfDisplayedRows; i++) {
    m_leafCells[i].setParentResponder(&m_selectableTableView);
    m_leafCells[i].setSubLabelFont(k_subLabelFont);
  }
}

void MathVariableBoxController::viewDidDisappear() {
  NestedMenuController::viewDidDisappear();

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
  if (event == Ion::Events::Backspace && m_currentPage != Page::RootMenu) {
    int rowIndex = selectedRow();
    if (destroyRecordAtRowIndex(rowIndex)) {
      m_selectableTableView.deselectTable();
      int newSelectedRow = rowIndex >= numberOfRows() ? numberOfRows()-1 : rowIndex;
      selectCellAtLocation(selectedColumn(), newSelectedRow);
      resetMemoization();
      m_selectableTableView.reloadData();
      if (numberOfRows() == 0) {
        returnToRootMenu();
      }
      return true;
    } else {
      // TODO : The record deletion has been denied. Add a warning.
    }
  }
  if (m_currentPage == Page::RootMenu && m_defineVariableCell.ShouldEnterOnEvent(event) && selectedRow() == defineVariableCellIndex()) {
    Container::activeApp()->modalViewController()->dismissModalViewController();
    sender()->handleStoreEvent();
    return true;
  }
  return NestedMenuController::handleEvent(event);
}

int MathVariableBoxController::numberOfRows() const {
  return numberOfElements(m_currentPage);
}

int MathVariableBoxController::numberOfElements(Page page) const {
  switch (page) {
    case Page::RootMenu:
      return (numberOfElements(Page::Expression) > 0) + (numberOfElements(Page::Function) > 0) + (numberOfElements(Page::Sequence) > 0) + (numberOfElements(Page::List) > 0) + (numberOfElements(Page::Matrix) > 0) + 1;
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
  assert(type < 3);
  if (type == k_defineVariableCellType) {
    assert(m_currentPage == Page::RootMenu);
    return 1;
  }
  if (type == k_leafCellType) {
    return k_maxNumberOfDisplayedRows;
  }
  return k_numberOfMenuRows;
}

void MathVariableBoxController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (m_currentPage == Page::RootMenu) {
    if (index == defineVariableCellIndex()) {
      return;
    }
    MessageTableCellWithChevron * myCell = static_cast<MessageTableCellWithChevron *>(cell);
    myCell->setMessage(nodeLabel(pageAtIndex(index)));
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
    if (index == defineVariableCellIndex()) {
      MessageTableCell tempCell;
      return heightForCellAtIndexWithWidthInit(&tempCell, index);
    }
    MessageTableCellWithChevron tempCell;
    return heightForCellAtIndexWithWidthInit(&tempCell, index);
  }
  ExpressionTableCellWithExpression tempCell;
  return heightForCellAtIndexWithWidthInit(&tempCell, index);
}

int MathVariableBoxController::typeAtIndex(int index) const {
  if (m_currentPage == Page::RootMenu) {
    if (index == defineVariableCellIndex()) {
      return k_defineVariableCellType;
    }
    return k_nodeCellType;
  }
  return k_leafCellType;
}

HighlightCell * MathVariableBoxController::reusableCell(int index, int type) {
  assert(index >= 0);
  if (type == k_defineVariableCellType) {
    return &m_defineVariableCell;
  }
  if (type == k_leafCellType) {
    return leafCellAtIndex(index);
  }
  return nodeCellAtIndex(index);
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
  return nodeLabel(m_currentPage);
}

MathVariableBoxController::Page MathVariableBoxController::pageAtIndex(int index) {
  assert(index >= 0 && index < numberOfElements(Page::RootMenu));
  for (int pageId = static_cast<int>(Page::Expression); pageId < static_cast<int>(Page::sizeOfEnum); pageId++) {
    if (numberOfElements(static_cast<Page>(pageId)) > 0) {
      if (index == 0) {
        return static_cast<Page>(pageId);
      }
      index--;
    }
  }
  assert(false);
  return static_cast<Page>(Page::Function);
}

void MathVariableBoxController::setPage(Page page) {
  m_currentPage = page;
  resetVarBoxMemoization();
}

bool MathVariableBoxController::selectSubMenu(int selectedRow) {
  m_selectableTableView.deselectTable();
  setPage(pageAtIndex(selectedRow));
  return NestedMenuController::selectSubMenu(selectedRow);
}

bool MathVariableBoxController::returnToPreviousMenu() {
  m_selectableTableView.deselectTable();
  setPage(Page::RootMenu);
  return NestedMenuController::returnToPreviousMenu();
}

bool MathVariableBoxController::returnToRootMenu() {
  assert(stackDepth() == 1);
  return returnToPreviousMenu();
}

bool MathVariableBoxController::selectLeaf(int selectedRow) {
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
    if (m_currentPage == Page::Sequence) {
      nameLength += UTF8Decoder::CodePointToChars(UCodePointSystem, nameToHandle + nameLength, nameToHandleMaxSize - nameLength - 1);
    }
    nameLength += UTF8Decoder::CodePointToChars(openingChar, nameToHandle + nameLength, nameToHandleMaxSize - nameLength - 1);
    nameLength += UTF8Decoder::CodePointToChars(UCodePointEmpty, nameToHandle + nameLength, nameToHandleMaxSize - nameLength - 1);
    if (m_currentPage == Page::Sequence) {
      nameLength += UTF8Decoder::CodePointToChars(UCodePointSystem, nameToHandle + nameLength, nameToHandleMaxSize - nameLength - 1);
    }
    nameLength += UTF8Decoder::CodePointToChars(closingChar, nameToHandle + nameLength, nameToHandleMaxSize - nameLength - 1);
    assert(nameLength < nameToHandleMaxSize);
    nameToHandle[nameLength] = 0;
  }

  // Handle the text
  sender()->handleEventWithText(nameToHandle);
  Container::activeApp()->modalViewController()->dismissModalViewController();
  return true;
}

I18n::Message MathVariableBoxController::nodeLabel(Page page) {
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

void MathVariableBoxController::resetVarBoxMemoization() {
  for (int i = 0; i < k_maxNumberOfDisplayedRows; i++) {
    m_layouts[i] = Layout();
  }
  m_firstMemoizedLayoutIndex = 0;
}

bool MathVariableBoxController::destroyRecordAtRowIndex(int rowIndex) {
  {
    Storage::Record record = recordAtIndex(rowIndex);
    if (!Container::activeApp()->storageWillChangeForRecordName(record.name())) {
      return false;
    }
    record.destroy();
  }
  // Shift the memoization if needed
  if (rowIndex >= m_firstMemoizedLayoutIndex + k_maxNumberOfDisplayedRows) {
    // The deleted row is after the memoization
    return true;
  }
  for (int i = std::max(0, rowIndex - m_firstMemoizedLayoutIndex); i < k_maxNumberOfDisplayedRows - 1; i++) {
    m_layouts[i] = m_layouts[i+1];
  }
  m_layouts[k_maxNumberOfDisplayedRows - 1] = Layout();
  return true;
}
