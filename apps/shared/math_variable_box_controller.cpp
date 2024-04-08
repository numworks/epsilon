#include "math_variable_box_controller.h"

#include <apps/apps_container.h>
#include <apps/global_preferences.h>
#include <apps/graph/list/list_controller.h>
#include <apps/shared/app_with_store_menu.h>
#include <apps/shared/sequence.h>
#include <assert.h>
#include <escher/metric.h>
#include <poincare/exception_checkpoint.h>
#include <poincare/expression.h>
#include <poincare/layout.h>
#include <poincare/preferences.h>
#include <poincare/serialization_helper.h>

#include <algorithm>

#include "continuous_function.h"
#include "global_context.h"

using namespace Poincare;
using namespace Ion;
using namespace Escher;

namespace Shared {

MathVariableBoxController::MathVariableBoxController()
    : NestedMenuController(nullptr, I18n::Message::Variables),
      m_currentPage(Page::RootMenu),
      m_firstMemoizedLayoutIndex(0) {
  m_defineVariableCell.label()->setMessage(I18n::Message::DefineVariable);
  for (int i = 0; i < k_maxNumberOfDisplayedRows; i++) {
    m_leafCells[i].subLabel()->setParentResponder(&m_selectableListView);
    m_leafCells[i].subLabel()->setFont(k_subLabelFont);
  }
}

void MathVariableBoxController::viewDidDisappear() {
  NestedMenuController::viewDidDisappear();

  /* NestedMenuController::viewDidDisappear might need cell heights, which would
   * use the MathVariableBoxController cell heights memoization. We thus reset
   * the MathVariableBoxController layouts only after calling the parent's
   * viewDidDisappear. */

  /* Tidy the layouts displayed in the MathVariableBoxController to clean
   * Pool */
  for (int i = 0; i < k_maxNumberOfDisplayedRows; i++) {
    m_leafCells[i].label()->setLayout(Layout());
    m_leafCells[i].subLabel()->setLayout(Layout());
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
    int row = selectedRow();
    if (destroyRecordAtRow(row)) {
      if (App::app()->modalViewController()->currentModalViewController() !=
          static_cast<const ViewController *>(this)) {
        // The varbox was dismissed by prepareForIntrusiveStorageChange
        return true;
      }
      int newSelectedRow = row >= numberOfRows() ? numberOfRows() - 1 : row;
      selectRow(newSelectedRow);
      m_selectableListView.reloadData();
      if (numberOfRows() == 0) {
        returnToRootMenu();
      }
      return true;
    } else {
      // TODO: The record deletion has been denied. Add a warning.
    }
  }
  if (m_currentPage == Page::RootMenu &&
      m_defineVariableCell.canBeActivatedByEvent(event) &&
      selectedRow() == defineVariableCellIndex()) {
    App::app()->modalViewController()->dismissModal();
    sender()->handleStoreEvent();
    return true;
  }
  return NestedMenuController::handleEvent(event);
}

int MathVariableBoxController::numberOfRows() const {
  return numberOfElements(m_currentPage);
}

static int NumberOfFunctions() {
  return Storage::FileSystem::sharedFileSystem->numberOfRecordsStartingWithout(
      ContinuousFunction::k_unnamedRecordFirstChar,
      Ion::Storage::functionExtension);
}

static int NumberOfParametricComponents() {
  return Storage::FileSystem::sharedFileSystem->numberOfRecordsWithExtension(
      Storage::parametricComponentExtension);
}

int MathVariableBoxController::numberOfElements(Page page) const {
  if (page == Page::RootMenu) {
    int numberOfRows = 1;  // Define a variable
    for (int i = 1; i < static_cast<int>(Page::NumberOfPages); i++) {
      Page p = static_cast<Page>(i);
      assert(p != Page::RootMenu);
      numberOfRows += (numberOfElements(static_cast<Page>(p)) > 0);
    }
    return numberOfRows;
  }
  if (page == Page::Function) {
    return NumberOfFunctions() + NumberOfParametricComponents() +
           Storage::FileSystem::sharedFileSystem->numberOfRecordsWithExtension(
               Storage::regressionExtension);
  }
  return Storage::FileSystem::sharedFileSystem->numberOfRecordsWithExtension(
      Extension(page));
}

int MathVariableBoxController::reusableCellCount(int type) const {
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

void MathVariableBoxController::fillCellForRow(HighlightCell *cell, int row) {
  int type = typeAtRow(row);
  if (type == k_defineVariableCellType) {
    return;
  }
  if (type == k_nodeCellType) {
    Escher::NestedMenuController::NodeCell *myCell =
        static_cast<Escher::NestedMenuController::NodeCell *>(cell);
    myCell->label()->setMessage(nodeLabel(pageAtIndex(row)));
    myCell->reloadCell();
    return;
  }
  assert(type == k_leafCellType);
  LeafCell *myCell = static_cast<LeafCell *>(cell);
  Storage::Record record = recordAtIndex(row);
  char symbolName[Shared::Function::k_maxNameWithArgumentSize];
  size_t symbolLength = 0;
  Layout symbolLayout;
  if (m_currentPage == Page::Expression || m_currentPage == Page::List ||
      m_currentPage == Page::Matrix) {
    static_assert(Shared::Function::k_maxNameWithArgumentSize >
                      SymbolAbstractNode::k_maxNameSize,
                  "Forgot argument's size?");
    symbolLength = record.nameWithoutExtension(
        symbolName, SymbolAbstractNode::k_maxNameSize);
  } else if (m_currentPage == Page::Function) {
    CodePoint symbol = UCodePointNull;
    if (record.hasExtension(Storage::functionExtension)) {
      symbol = GlobalContext::continuousFunctionStore->modelForRecord(record)
                   ->symbol();
    } else if (record.hasExtension(Storage::parametricComponentExtension)) {
      symbol = ContinuousFunctionProperties::k_parametricSymbol;
    } else {
      assert(record.hasExtension(Storage::regressionExtension));
      symbol = ContinuousFunctionProperties::k_cartesianSymbol;
    }
    symbolLength = Function::NameWithArgument(
        record, symbol, symbolName, Function::k_maxNameWithArgumentSize);

  } else {
    assert(m_currentPage == Page::Sequence);
    Shared::Sequence u(record);
    symbolLength = u.nameWithArgumentAndType(
        symbolName, Shared::Sequence::k_maxNameWithArgumentSize);
    symbolLayout = u.definitionName();
  }
  if (symbolLayout.isUninitialized()) {
    symbolLayout = Layout::String(symbolName, symbolLength);
  }
  myCell->label()->setLayout(symbolLayout);
  myCell->subLabel()->setLayout(expressionLayoutForRecord(record, row));
  myCell->subLabel()->resetScroll();
  myCell->reloadCell();
}

KDCoordinate MathVariableBoxController::nonMemoizedRowHeight(int row) {
  if (m_currentPage == Page::RootMenu) {
    if (row == defineVariableCellIndex()) {
      MenuCell<MessageTextView> tempCell;
      return protectedNonMemoizedRowHeight(&tempCell, row);
    }
    Escher::NestedMenuController::NodeCell tempCell;
    return protectedNonMemoizedRowHeight(&tempCell, row);
  }
  LeafCell tempCell;
  return protectedNonMemoizedRowHeight(&tempCell, row);
}

int MathVariableBoxController::typeAtRow(int row) const {
  if (m_currentPage == Page::RootMenu) {
    if (row == defineVariableCellIndex()) {
      return k_defineVariableCellType;
    }
    return k_nodeCellType;
  }
  return k_leafCellType;
}

HighlightCell *MathVariableBoxController::reusableCell(int index, int type) {
  assert(index >= 0);
  if (type == k_defineVariableCellType) {
    return &m_defineVariableCell;
  }
  if (type == k_leafCellType) {
    return leafCellAtIndex(index);
  }
  return nodeCellAtIndex(index);
}

MathVariableBoxController::LeafCell *MathVariableBoxController::leafCellAtIndex(
    int index) {
  assert(index >= 0 && index < k_maxNumberOfDisplayedRows);
  return &m_leafCells[index];
}

Escher::NestedMenuController::NodeCell *
MathVariableBoxController::nodeCellAtIndex(int index) {
  assert(index >= 0 && index < k_numberOfMenuRows);
  return &m_nodeCells[index];
}

I18n::Message MathVariableBoxController::subTitle() {
  return nodeLabel(m_currentPage);
}

MathVariableBoxController::Page MathVariableBoxController::pageAtIndex(
    int index) {
  assert(index >= 0 && index < numberOfElements(Page::RootMenu));
  for (int pageId = static_cast<int>(Page::Expression);
       pageId < static_cast<int>(Page::NumberOfPages); pageId++) {
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
  setPage(pageAtIndex(selectedRow));
  return NestedMenuController::selectSubMenu(selectedRow);
}

bool MathVariableBoxController::returnToPreviousMenu() {
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

  // Get the name text to insert
  Storage::Record record = recordAtIndex(selectedRow);
  constexpr size_t nameToHandleMaxSize =
      Shared::Function::k_maxNameWithArgumentSize;
  char nameToHandle[nameToHandleMaxSize];
  size_t nameLength =
      record.nameWithoutExtension(nameToHandle, nameToHandleMaxSize);

  if (m_currentPage == Page::Function || m_currentPage == Page::Sequence) {
    // Add parentheses to a function name, and braces to a sequence
    char openingChar = m_currentPage == Page::Function ? '(' : '{';
    char closingChar = m_currentPage == Page::Function ? ')' : '}';
    assert(nameLength < nameToHandleMaxSize);
    if (m_currentPage == Page::Sequence) {
      nameLength += SerializationHelper::CodePoint(
          nameToHandle + nameLength, nameToHandleMaxSize - nameLength - 1,
          UCodePointSystem);
    }
    nameLength += SerializationHelper::CodePoint(
        nameToHandle + nameLength, nameToHandleMaxSize - nameLength - 1,
        openingChar);
    nameLength += SerializationHelper::CodePoint(
        nameToHandle + nameLength, nameToHandleMaxSize - nameLength - 1,
        UCodePointEmpty);
    if (m_currentPage == Page::Sequence) {
      nameLength += SerializationHelper::CodePoint(
          nameToHandle + nameLength, nameToHandleMaxSize - nameLength - 1,
          UCodePointSystem);
    }
    nameLength += SerializationHelper::CodePoint(
        nameToHandle + nameLength, nameToHandleMaxSize - nameLength - 1,
        closingChar);
    assert(nameLength < nameToHandleMaxSize);
  }

  // Handle the text
  App::app()->modalViewController()->dismissModal();
  sender()->handleEventWithText(nameToHandle);
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

Layout MathVariableBoxController::expressionLayoutForRecord(
    Storage::Record record, int index) {
  assert(m_currentPage != Page::RootMenu);
  assert(index >= 0);
  if (index >= m_firstMemoizedLayoutIndex + k_maxNumberOfDisplayedRows ||
      index < m_firstMemoizedLayoutIndex) {
    // Change range of layout memoization
    int deltaIndex =
        index >= m_firstMemoizedLayoutIndex + k_maxNumberOfDisplayedRows
            ? index - k_maxNumberOfDisplayedRows + 1 -
                  m_firstMemoizedLayoutIndex
            : index - m_firstMemoizedLayoutIndex;
    for (int i = 0; i < k_maxNumberOfDisplayedRows; i++) {
      int j = deltaIndex + i;
      m_layouts[i] =
          (j >= m_firstMemoizedLayoutIndex && j < k_maxNumberOfDisplayedRows)
              ? m_layouts[j]
              : Layout();
    }
    m_firstMemoizedLayoutIndex += deltaIndex;
    assert(m_firstMemoizedLayoutIndex >= 0);
  }
  assert(index >= m_firstMemoizedLayoutIndex &&
         index < m_firstMemoizedLayoutIndex + k_maxNumberOfDisplayedRows);
  if (m_layouts[index - m_firstMemoizedLayoutIndex].isUninitialized()) {
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
    m_layouts[index - m_firstMemoizedLayoutIndex] = result;
  }
  return m_layouts[index - m_firstMemoizedLayoutIndex];
}

const char *MathVariableBoxController::Extension(Page page) {
  // Function contains two extensions (func and reg)
  assert(page != Page::RootMenu && page != Page::Function);
  switch (page) {
    case Page::Expression:
      return Ion::Storage::expressionExtension;
    case Page::List:
      return Ion::Storage::listExtension;
    case Page::Matrix:
      return Ion::Storage::matrixExtension;
    default:
      assert(page == Page::Sequence);
      return Ion::Storage::sequenceExtension;
  }
}

Storage::Record MathVariableBoxController::recordAtIndex(int row) {
  assert(m_currentPage != Page::RootMenu);
  Storage::Record record;
  if (m_currentPage == Page::Function) {
    int numberOfFunctions = NumberOfFunctions();
    int numberOfParametricComponents = NumberOfParametricComponents();
    if (row < numberOfFunctions) {
      record = Storage::FileSystem::sharedFileSystem
                   ->recordWithExtensionAtIndexStartingWithout(
                       ContinuousFunction::k_unnamedRecordFirstChar,
                       Storage::functionExtension, row);
    } else if (row < numberOfFunctions + numberOfParametricComponents) {
      record =
          Storage::FileSystem::sharedFileSystem->recordWithExtensionAtIndex(
              Storage::parametricComponentExtension, row - numberOfFunctions);
    } else {
      record =
          Storage::FileSystem::sharedFileSystem->recordWithExtensionAtIndex(
              Storage::regressionExtension,
              row - numberOfFunctions - numberOfParametricComponents);
    }
  } else {
    record = Storage::FileSystem::sharedFileSystem->recordWithExtensionAtIndex(
        Extension(m_currentPage), row);
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

bool MathVariableBoxController::destroyRecordAtRow(int row) {
  {
    Storage::Record record = recordAtIndex(row);
    if (record.hasExtension(Ion::Storage::parametricComponentExtension) ||
        record.hasExtension(Ion::Storage::regressionExtension)) {
      return false;
    }
    bool isParametricFunction = false;
    constexpr size_t bufferSize = SymbolAbstractNode::k_maxNameSize;
    char buffer[bufferSize];
    size_t length = 0;
    if (record.hasExtension(Storage::functionExtension)) {
      ExpiringPointer<ContinuousFunction> f =
          GlobalContext::continuousFunctionStore->modelForRecord(record);
      if (f->properties().isEnabledParametric()) {
        isParametricFunction = true;
        length = f->name(buffer, bufferSize);
      }
    }
    Shared::AppWithStoreMenu *app =
        static_cast<Shared::AppWithStoreMenu *>(App::app());
    app->prepareForIntrusiveStorageChange();
    bool canDestroy = record.tryToDestroy();
    app->concludeIntrusiveStorageChange();
    if (!canDestroy) {
      return false;
    }
    if (isParametricFunction) {
      GlobalContext::DeleteParametricComponentsWithBaseName(buffer, length,
                                                            bufferSize);
    }
  }
  // Shift the memoization if needed
  if (row >= m_firstMemoizedLayoutIndex + k_maxNumberOfDisplayedRows) {
    // The deleted row is after the memoization
    return true;
  }
  for (int i = std::max(0, row - m_firstMemoizedLayoutIndex);
       i < k_maxNumberOfDisplayedRows - 1; i++) {
    m_layouts[i] = m_layouts[i + 1];
  }
  m_layouts[k_maxNumberOfDisplayedRows - 1] = Layout();
  return true;
}

}  // namespace Shared
