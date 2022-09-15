#include "values_controller.h"
#include "function_app.h"
#include "separable.h"
#include <poincare/preferences.h>
#include <assert.h>
#include <limits.h>
#include <algorithm>

using namespace Poincare;
using namespace Escher;

namespace Shared {

constexpr int ValuesController::k_maxNumberOfDisplayableRows;

// TODO: use std::abs
static inline int absInt(int x) { return x < 0 ? -x : x; }

// Constructor and helpers

ValuesController::ValuesController(Responder * parentResponder, ButtonRowController * header) :
  EditableCellTableViewController(parentResponder),
  ButtonRowDelegate(header, nullptr),
  m_numberOfColumns(0),
  m_numberOfColumnsNeedUpdate(true),
  m_firstMemoizedColumn(INT_MAX),
  m_firstMemoizedRow(INT_MAX),
  m_abscissaParameterController(this, this)
{}

void ValuesController::setupSelectableTableViewAndCells(InputEventHandlerDelegate * inputEventHandlerDelegate) {
  prefacedView()->setCellOverlap(0, 0);
  prefacedView()->setMargins(k_margin, k_scrollBarMargin, k_scrollBarMargin, k_margin);
  prefacedView()->setBackgroundColor(Palette::WallScreenDark);

  int numberOfAbscissaCells = abscissaCellsCount();
  for (int i = 0; i < numberOfAbscissaCells; i++) {
    EvenOddEditableTextCell * c = abscissaCells(i);
    c->setParentResponder(selectableTableView());
    c->editableTextCell()->textField()->setDelegates(inputEventHandlerDelegate, this);
    c->editableTextCell()->textField()->setFont(k_font);
  }
  int numberOfAbscissaTitleCells = abscissaTitleCellsCount();
  for (int i = 0; i < numberOfAbscissaTitleCells; i++) {
    EvenOddMessageTextCell * c = abscissaTitleCells(i);
    c->setMessageFont(k_font);
  }
}

// View Controller

const char * ValuesController::title() {
  return I18n::translate(I18n::Message::ValuesTab);
}

void ValuesController::viewWillAppear() {
  // Reset memoization before any call to willDisplayCellAtLocation
  resetValuesMemoization();
  EditableCellTableViewController::viewWillAppear();
  header()->setSelectedButton(-1);
}

void ValuesController::viewDidDisappear() {
  m_numberOfColumnsNeedUpdate = true;
  EditableCellTableViewController::viewDidDisappear();
}

// Responder

bool ValuesController::handleEvent(Ion::Events::Event event) {
  if (EditableCellTableViewController::handleEvent(event)) {
    return true;
  }
  if (event == Ion::Events::Down) {
    if (selectedRow() == -1) {
      header()->setSelectedButton(-1);
      selectableTableView()->selectCellAtLocation(0,0);
      Container::activeApp()->setFirstResponder(selectableTableView());
      return true;
    }
    return false;
  }
  if (event == Ion::Events::Up) {
    if (selectedRow() == -1) {
      header()->setSelectedButton(-1);
      Container::activeApp()->setFirstResponder(tabController());
      return true;
    }
    selectableTableView()->deselectTable();
    header()->setSelectedButton(0);
    return true;
  }
  if (event == Ion::Events::Backspace && selectedRow() > 0 &&
    selectedRow() <= numberOfElementsInColumn(selectedColumn())) {
    int row = selectedRow();
    int column = selectedColumn();
    intervalAtColumn(column)->deleteElementAtIndex(row-1);
    // Reload memoization
    for (int i = row; i < numberOfElementsInColumn(column)+1; i++) {
      didChangeCell(column, i);
    }
    selectableTableView()->reloadData();
    return true;
  }
  if (selectedRow() == -1) {
    return header()->handleEvent(event);
  }
  return false;
}

void ValuesController::didBecomeFirstResponder() {
  EditableCellTableViewController::didBecomeFirstResponder();
  if (selectedRow() == -1) {
    selectableTableView()->deselectTable();
    header()->setSelectedButton(0);
  } else {
    header()->setSelectedButton(-1);
  }
}

void ValuesController::willExitResponderChain(Responder * nextFirstResponder) {
  if (nextFirstResponder == tabController()) {
    assert(tabController() != nullptr);
    selectableTableView()->deselectTable();
    selectableTableView()->scrollToCell(0,0);
    header()->setSelectedButton(-1);
  }
}

// TableViewDataSource

int ValuesController::numberOfColumns() const {
  if (m_numberOfColumnsNeedUpdate) {
    updateNumberOfColumns();
    m_numberOfColumnsNeedUpdate = false;
  }
  return m_numberOfColumns;
}

void ValuesController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  willDisplayCellAtLocationWithDisplayMode(cell, i, j, Preferences::sharedPreferences()->displayMode());
  int typeAtLoc = typeAtLocation(i,j);
  // The cell is not a title cell and not editable
  if (typeAtLoc == k_notEditableValueCellType) {
    // Special case: last row
    if (j == numberOfElementsInColumn(i) + 1) {
      static_cast<EvenOddBufferTextCell *>(cell)->setText("");
    } else {
      static_cast<EvenOddBufferTextCell *>(cell)->setText(memoizedBufferForCell(i, j));
    }
    return;
  }
}

HighlightCell * ValuesController::reusableCell(int index, int type) {
  assert(0 <= index && index < reusableCellCount(type));
  switch (type) {
    case k_abscissaTitleCellType:
      return abscissaTitleCells(index);
    case k_functionTitleCellType:
      return functionTitleCells(index);
    case k_editableValueCellType:
      return abscissaCells(index);
    case k_notEditableValueCellType:
      return floatCells(index);
    default:
      assert(false);
      return nullptr;
  }
}

int ValuesController::reusableCellCount(int type) {
  switch (type) {
    case k_abscissaTitleCellType:
      return abscissaTitleCellsCount();
    case k_functionTitleCellType:
      return maxNumberOfFunctions();
    case k_editableValueCellType:
      return abscissaCellsCount();
    case k_notEditableValueCellType:
      return maxNumberOfCells();
    default:
      assert(false);
      return 0;
  }
}

int ValuesController::typeAtLocation(int i, int j) {
  return (i > 0) + 2 * (j > 0);
}

// ButtonRowDelegate

int ValuesController::numberOfButtons(ButtonRowController::Position) const {
  if (isEmpty()) {
    return 0;
  }
  return 1;
}

// AlternateEmptyViewDelegate

bool ValuesController::isEmpty() const {
  if (functionStore()->numberOfActiveFunctions() == 0) {
    return true;
  }
  return false;
}

Responder * ValuesController::defaultController() {
  return tabController();
}

// EditableCellTableViewController

ColumnParameterController * ValuesController::columnParameterController() {
  if (typeAtLocation(selectedColumn(), 0) == k_abscissaTitleCellType) {
    return &m_abscissaParameterController;
  }
  return functionParameterController();
}

bool ValuesController::setDataAtLocation(double floatBody, int columnIndex, int rowIndex) {
  assert(checkDataAtLocation(floatBody, columnIndex, rowIndex));
  intervalAtColumn(columnIndex)->setElement(rowIndex-1, floatBody);
  return true;
}

bool ValuesController::cellAtLocationIsEditable(int columnIndex, int rowIndex) {
  return typeAtLocation(columnIndex, rowIndex) == k_editableValueCellType;
}

double ValuesController::dataAtLocation(int columnIndex, int rowIndex) {
  return intervalAtColumn(columnIndex)->element(rowIndex-1);
}

void ValuesController::didChangeCell(int column, int row) {
  /* Update the row memoization if it exists */
  // the first row is never reloaded as it corresponds to title row
  assert(row > 0);
  // Conversion of coordinates from absolute table to values table
  int memoizedRow = valuesRowForAbsoluteRow(row) - m_firstMemoizedRow;
  if (0 > memoizedRow || memoizedRow >= k_maxNumberOfDisplayableRows) {
    // The changed row is out of the memoized table
    return;
  }

  // Find the abscissa column corresponding to column
  int abscissaColumn = 0;
  int nbOfColumns = numberOfColumnsForAbscissaColumn(abscissaColumn);
   while (column >= nbOfColumns) {
    abscissaColumn = nbOfColumns;
    nbOfColumns += numberOfColumnsForAbscissaColumn(abscissaColumn);
  }

  // Update the memoization of rows linked to the changed cell
  int nbOfMemoizedColumns = numberOfMemoizedColumn();
  int nbOfColumnsForAbscissa = numberOfColumnsForAbscissaColumn(abscissaColumn);
  for (int i = abscissaColumn+1; i < abscissaColumn+nbOfColumnsForAbscissa; i++) {
    int memoizedI = valuesColumnForAbsoluteColumn(i) - m_firstMemoizedColumn;
    if (memoizedI < 0 || memoizedI >= nbOfMemoizedColumns) {
      // The changed column is out of the memoized table
      continue;
    }
    fillMemoizedBuffer(i, row, nbOfMemoizedColumns*memoizedRow+memoizedI);
  }
}

int ValuesController::numberOfElementsInColumn(int columnIndex) const {
  return const_cast<ValuesController *>(this)->intervalAtColumn(columnIndex)->numberOfElements();
}

// Parent controller getters

Responder * ValuesController::tabController() const {
  return (parentResponder()->parentResponder()->parentResponder()->parentResponder());
}

StackViewController * ValuesController::stackController() const {
  return (StackViewController *)(parentResponder()->parentResponder()->parentResponder());
}

// Model getters

Ion::Storage::Record ValuesController::recordAtColumn(int i) {
  assert(typeAtLocation(i, 0) == k_functionTitleCellType);
  return functionStore()->activeRecordAtIndex(i-1);
}

// Number of columns memoization

void ValuesController::updateNumberOfColumns() const {
  m_numberOfColumns = 1+functionStore()->numberOfActiveFunctions();
}

FunctionStore * ValuesController::functionStore() const {
  return FunctionApp::app()->functionStore();
}

// Function evaluation memoization

void ValuesController::resetValuesMemoization() {
  m_firstMemoizedColumn = INT_MAX;
  m_firstMemoizedRow = INT_MAX;
}

char * ValuesController::memoizedBufferForCell(int i, int j) {
  const int nbOfMemoizedColumns = numberOfMemoizedColumn();
  // Conversion of coordinates from absolute table to values table
  int valuesI = valuesColumnForAbsoluteColumn(i);
  int valuesJ = valuesRowForAbsoluteRow(j);
  /* Compute the required offset to apply to the memoized table in order to
   * display cell (i,j) */
  int offsetI = 0;
  int offsetJ = 0;
  if (valuesI < m_firstMemoizedColumn) {
    offsetI = valuesI - m_firstMemoizedColumn;
  } else if (valuesI >= m_firstMemoizedColumn + nbOfMemoizedColumns) {
    offsetI = valuesI - nbOfMemoizedColumns - m_firstMemoizedColumn + 1;
  }
  if (valuesJ < m_firstMemoizedRow) {
    offsetJ = valuesJ - m_firstMemoizedRow;
  } else if (valuesJ >= m_firstMemoizedRow + k_maxNumberOfDisplayableRows) {
    offsetJ = valuesJ - k_maxNumberOfDisplayableRows - m_firstMemoizedRow + 1;
  }
  int offset = -offsetJ*nbOfMemoizedColumns-offsetI;

  // Apply the offset
  if (offset != 0) {
    m_firstMemoizedColumn = m_firstMemoizedColumn + offsetI;
    m_firstMemoizedRow = m_firstMemoizedRow + offsetJ;
    // Shift already memoized cells
    const int numberOfMemoizedCell = k_maxNumberOfDisplayableRows * nbOfMemoizedColumns;
    size_t moveLength = (numberOfMemoizedCell - absInt(offset))*valuesCellBufferSize()*sizeof(char);
    if (offset > 0 && offset < numberOfMemoizedCell) {
      memmove(memoizedBufferAtIndex(offset), memoizedBufferAtIndex(0), moveLength);
    } else if (offset < 0 && offset > -numberOfMemoizedCell) {
      memmove(memoizedBufferAtIndex(0), memoizedBufferAtIndex(-offset), moveLength);
    }
    // Compute the buffer of the new cells of the memoized table
    int maxI = numberOfValuesColumns() - m_firstMemoizedColumn;
    for (int ii = 0; ii < std::min(nbOfMemoizedColumns, maxI); ii++) {
      int maxJ = numberOfElementsInColumn(absoluteColumnForValuesColumn(ii+m_firstMemoizedColumn)) - m_firstMemoizedRow;
      for (int jj = 0; jj < std::min(k_maxNumberOfDisplayableRows, maxJ); jj++) {
        // Escape if already filled
        if (ii >= -offsetI && ii < -offsetI + nbOfMemoizedColumns && jj >= -offsetJ && jj < -offsetJ + k_maxNumberOfDisplayableRows) {
          continue;
        }
        fillMemoizedBuffer(absoluteColumnForValuesColumn(m_firstMemoizedColumn + ii),
            absoluteRowForValuesRow(m_firstMemoizedRow + jj),
            jj * nbOfMemoizedColumns + ii);
      }
    }
  }
  return memoizedBufferAtIndex((valuesJ-m_firstMemoizedRow)*nbOfMemoizedColumns + (valuesI-m_firstMemoizedColumn));
}

void ValuesController::clearSelectedColumn() {
   intervalAtColumn(selectedColumn())->clear();
}

void ValuesController::setClearPopUpContent() {
  m_confirmPopUpController.setMessageWithPlaceholders(I18n::Message::ClearTableConfirmation);
}

int ValuesController::fillColumnName(int columnIndex, char * buffer) {
  assert(typeAtLocation(columnIndex, 0) ==  k_abscissaTitleCellType);
  return fillColumnNameWithMessage(buffer, valuesParameterMessageAtColumn(columnIndex));
}

void ValuesController::setTitleCellText(HighlightCell * cell, int columnIndex) {
  if (typeAtLocation(columnIndex,0) == k_abscissaTitleCellType) {
    EvenOddMessageTextCell * myTitleCell = static_cast<EvenOddMessageTextCell *>(cell);
    myTitleCell->setMessage(valuesParameterMessageAtColumn(columnIndex));
    return;
  }
}

void ValuesController::setTitleCellStyle(HighlightCell * cell, int columnIndex) {
  if (typeAtLocation(columnIndex,0) == k_functionTitleCellType ) {
    FunctionTitleCell * myCell = static_cast<FunctionTitleCell *>(cell);
    Shared::Function * function = functionStore()->modelForRecord(recordAtColumn(columnIndex)).pointer();
    myCell->setColor(function->color());
    return;
  }
}

void ValuesController::reloadEditedCell(int column, int row) {
  // Reload the row, if an existing value was edited.
  for (int i = 0; i < numberOfColumns(); i++) {
    selectableTableView()->reloadCellAtLocation(i, row);
  }
}

void ValuesController::initializeInterval() {
  intervalParameterController()->setInterval(intervalAtColumn(selectedColumn()));
  setStartEndMessages(intervalParameterController(), selectedColumn());
}

}
