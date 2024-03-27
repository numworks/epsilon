#include "values_controller.h"

#include <assert.h>
#include <limits.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>
#include <poincare/preferences.h>
#include <poincare/print.h>
#include <stdlib.h>

#include <algorithm>

#include "function_app.h"

using namespace Poincare;
using namespace Escher;

namespace Shared {

// Constructor and helpers

ValuesController::ValuesController(Responder *parentResponder,
                                   ButtonRowController *header)
    : EditableCellTableViewController(parentResponder,
                                      &m_prefacedTwiceTableView),
      ButtonRowDelegate(header, nullptr),
      m_numberOfColumns(0),
      m_prefacedTwiceTableView(0, 0, this, &m_selectableTableView, this, this),
      m_firstMemoizedColumn(INT_MAX),
      m_firstMemoizedRow(INT_MAX),
      m_abscissaParameterController(this, this) {
  m_prefacedTwiceTableView.setBackgroundColor(Palette::WallScreenDark);
  m_prefacedTwiceTableView.setCellOverlap(0, 0);
  m_prefacedTwiceTableView.setMargins(k_margins);
}

void ValuesController::setupSelectableTableViewAndCells() {
  int numberOfAbscissaCells = abscissaCellsCount();
  for (int i = 0; i < numberOfAbscissaCells; i++) {
    AbstractEvenOddEditableTextCell *c = abscissaCells(i);
    c->setParentResponder(selectableTableView());
    c->editableTextCell()->textField()->setDelegate(this);
  }
}

// View Controller

void ValuesController::initView() {
  EditableCellTableViewController::initView();
  updateNumberOfColumns();
}

void ValuesController::viewWillAppear() {
  EditableCellTableViewController::viewWillAppear();
  header()->setSelectedButton(-1);
}

void ValuesController::viewDidDisappear() {
  resetLayoutMemoization();
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
      selectableTableView()->selectCellAtLocation(0, 0);
      App::app()->setFirstResponder(selectableTableView());
      return true;
    }
    return false;
  }
  if (event == Ion::Events::Up) {
    if (selectedRow() == -1) {
      header()->setSelectedButton(-1);
      tabController()->selectTab();
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
    intervalAtColumn(column)->deleteElementAtIndex(row - k_numberOfTitleRows);
    // Reload memoization
    rowWasDeleted(row, column);
    selectableTableView()->reloadData(true, false);
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

void ValuesController::willExitResponderChain(Responder *nextFirstResponder) {
  if (nextFirstResponder == tabController()) {
    assert(tabController() != nullptr);
    selectableTableView()->deselectTable();
    selectableTableView()->scrollToCell(0, 0);
    header()->setSelectedButton(-1);
  }
}

// TableViewDataSource

int ValuesController::numberOfColumns() const { return m_numberOfColumns; }

void ValuesController::fillCellForLocation(HighlightCell *cell, int column,
                                           int row) {
  fillCellForLocationWithDisplayMode(
      cell, column, row, Preferences::SharedPreferences()->displayMode(),
      Preferences::SharedPreferences()->numberOfSignificantDigits());
  // The cell is not a title cell and not editable
  if (typeAtLocation(column, row) == k_notEditableValueCellType) {
    EvenOddExpressionCell *myCell = static_cast<EvenOddExpressionCell *>(cell);
    // Special case: last row
    if (row == numberOfElementsInColumn(column) + k_numberOfTitleRows) {
      myCell->setLayout(KRackL());
    } else {
      myCell->setLayout(memoizedLayoutForCell(column, row));
    }
    return;
  }
}

HighlightCell *ValuesController::reusableCell(int index, int type) {
  assert(0 <= index && index < reusableCellCount(type));
  switch (type) {
    case k_abscissaTitleCellType:
      return abscissaTitleCells(index);
    case k_functionTitleCellType:
      return functionTitleCells(index);
    case k_editableValueCellType:
      return abscissaCells(index);
    case k_notEditableValueCellType:
      return valueCells(index);
    default:
      assert(false);
      return nullptr;
  }
}

int ValuesController::reusableCellCount(int type) const {
  switch (type) {
    case k_abscissaTitleCellType:
      return abscissaTitleCellsCount();
    case k_functionTitleCellType:
      return k_maxNumberOfDisplayableColumns;
    case k_editableValueCellType:
      return abscissaCellsCount();
    case k_notEditableValueCellType:
      return k_maxNumberOfDisplayableCells;
    default:
      assert(false);
      return 0;
  }
}

int ValuesController::typeAtLocation(int column, int row) const {
  static_assert(k_abscissaTitleCellType == 0 && k_functionTitleCellType == 1 &&
                    k_editableValueCellType == 2 &&
                    k_notEditableValueCellType == 3,
                "ValuesController::typeAtLocation formula is wrong.");
  return (column > 0) + 2 * (row > 0);
}

Responder *ValuesController::responderWhenEmpty() {
  tabController()->selectTab();
  return tabController();
}

SelectableViewController *ValuesController::columnParameterController() {
  if (typeAtLocation(selectedColumn(), 0) == k_abscissaTitleCellType) {
    return &m_abscissaParameterController;
  }
  return functionParameterController();
}

ColumnParameters *ValuesController::columnParameters() {
  if (typeAtLocation(selectedColumn(), 0) == k_abscissaTitleCellType) {
    return &m_abscissaParameterController;
  }
  return functionParameters();
}

bool ValuesController::setDataAtLocation(double floatBody, int column,
                                         int row) {
  assert(checkDataAtLocation(floatBody, column, row));
  intervalAtColumn(column)->setElement(row - k_numberOfTitleRows, floatBody);
  return true;
}

bool ValuesController::cellAtLocationIsEditable(int column, int row) {
  return typeAtLocation(column, row) == k_editableValueCellType;
}

double ValuesController::dataAtLocation(int column, int row) {
  return intervalAtColumn(column)->element(row - k_numberOfTitleRows);
}

int ValuesController::valuesColumnForAbsoluteColumn(int column) {
  // Subtract the abscissa columns
  assert(column < numberOfColumns() &&
         column - numberOfAbscissaColumnsBeforeAbsoluteColumn(column) >= 0);
  return column - numberOfAbscissaColumnsBeforeAbsoluteColumn(column);
}

int ValuesController::valuesRowForAbsoluteRow(int row) {
  // Subtract the title row
  assert(row - k_numberOfTitleRows >= 0);
  return row - k_numberOfTitleRows;
}

int ValuesController::absoluteColumnForValuesColumn(int column) {
  // Add the abscissa columns
  assert(column >= 0 &&
         column + numberOfAbscissaColumnsBeforeValuesColumn(column) <
             numberOfColumns());
  return column + numberOfAbscissaColumnsBeforeValuesColumn(column);
}

int ValuesController::absoluteRowForValuesRow(int row) {
  // Add the title row
  assert(row >= 0);
  return row + k_numberOfTitleRows;
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
  int nbOfMemoizedColumns = k_maxNumberOfDisplayableColumns;
  int nbOfColumnsForAbscissa = numberOfColumnsForAbscissaColumn(abscissaColumn);
  for (int i = abscissaColumn + 1; i < abscissaColumn + nbOfColumnsForAbscissa;
       i++) {
    int memoizedI = valuesColumnForAbsoluteColumn(i) - m_firstMemoizedColumn;
    if (memoizedI < 0 || memoizedI >= nbOfMemoizedColumns) {
      // The changed column is out of the memoized table
      continue;
    }
    /* Recomputing the layout might change the column width. To avoid reseting
     * all the memoization, it's only updated by knowing the difference between
     * the previous and the new width.
     * */
    KDCoordinate previousWidth = columnWidth(i);
    createMemoizedLayout(i, row, nbOfMemoizedColumns * memoizedRow + memoizedI);
    if (previousWidth != TableSize1DManager::k_undefinedSize) {
      updateSizeMemoizationForColumnAfterIndexChanged(i, previousWidth, row);
    }
  }
}

int ValuesController::numberOfElementsInColumn(int column) const {
  return const_cast<ValuesController *>(this)
      ->intervalAtColumn(column)
      ->numberOfElements();
}

KDCoordinate ValuesController::defaultColumnWidth() {
  KDCoordinate width =
      PrintFloat::glyphLengthForFloatWithPrecision(
          Preferences::SharedPreferences()->numberOfSignificantDigits()) *
          KDFont::GlyphWidth(k_cellFont) +
      2 * Escher::Metric::SmallCellMargin;
  return std::max(EditableCellTableViewController::defaultColumnWidth(), width);
}

// Parent controller getters

Escher::TabViewController *ValuesController::tabController() const {
  return static_cast<Escher::TabViewController *>(parentResponder()
                                                      ->parentResponder()
                                                      ->parentResponder()
                                                      ->parentResponder());
}

StackViewController *ValuesController::stackController() const {
  return (StackViewController
              *)(parentResponder()->parentResponder()->parentResponder());
}

// Model getters

Ion::Storage::Record ValuesController::recordAtColumn(int i) {
  assert(typeAtLocation(i, 0) == k_functionTitleCellType);
  return functionStore()->activeRecordAtIndex(
      i - numberOfAbscissaColumnsBeforeAbsoluteColumn(i));
}

FunctionStore *ValuesController::functionStore() const {
  return FunctionApp::app()->functionStore();
}

// Function evaluation memoization

void ValuesController::resetLayoutMemoization() {
  const int numberOfCells = k_maxNumberOfDisplayableCells;
  for (int i = 0; i < numberOfCells; i++) {
    *memoizedLayoutAtIndex(i) = Layout();
    EvenOddExpressionCell *valueCell = valueCells(i);
    assert(valueCell);
    valueCell->setLayout(Layout());
  }
  const int numberOfFunctionColumns = k_maxNumberOfDisplayableColumns;
  for (int i = 0; i < numberOfFunctionColumns; i++) {
    ExpressionFunctionTitleCell *titleCell = functionTitleCells(i);
    assert(titleCell);
    titleCell->setLayout(Layout());
  }
  m_firstMemoizedColumn = INT_MAX;
  m_firstMemoizedRow = INT_MAX;
}

Layout ValuesController::memoizedLayoutForCell(int column, int row) {
  const int nbOfMemoizedColumns = k_maxNumberOfDisplayableColumns;
  // Conversion of coordinates from absolute table to values table
  int valuesCol = valuesColumnForAbsoluteColumn(column);
  int valuesRow = valuesRowForAbsoluteRow(row);
  /* Compute the required offset to apply to the memoized table in order to
   * display cell (col,row) */
  int offsetCol = 0;
  int offsetRow = 0;
  if (valuesCol < m_firstMemoizedColumn) {
    offsetCol = valuesCol - m_firstMemoizedColumn;
  } else if (valuesCol >= m_firstMemoizedColumn + nbOfMemoizedColumns) {
    offsetCol = valuesCol - nbOfMemoizedColumns - m_firstMemoizedColumn + 1;
  }
  if (valuesRow < m_firstMemoizedRow) {
    offsetRow = valuesRow - m_firstMemoizedRow;
  } else if (valuesRow >= m_firstMemoizedRow + k_maxNumberOfDisplayableRows) {
    offsetRow =
        valuesRow - k_maxNumberOfDisplayableRows - m_firstMemoizedRow + 1;
  }
  int offset = -offsetRow * nbOfMemoizedColumns - offsetCol;

  // Apply the offset
  if (offset != 0) {
    m_firstMemoizedColumn = m_firstMemoizedColumn + offsetCol;
    m_firstMemoizedRow = m_firstMemoizedRow + offsetRow;
    // Shift already memoized cells
    const int numberOfMemoizedCell =
        k_maxNumberOfDisplayableRows * nbOfMemoizedColumns;
    const int numberOfLayoutsToMove = numberOfMemoizedCell - abs(offset);
    if (offset > 0 && offset < numberOfMemoizedCell) {
      for (int i = numberOfLayoutsToMove - 1; i >= 0; i--) {
        *memoizedLayoutAtIndex(offset + i) = *memoizedLayoutAtIndex(i);
      }
    } else if (offset < 0 && offset > -numberOfMemoizedCell) {
      for (int i = -offset; i < -offset + numberOfLayoutsToMove; i++) {
        *memoizedLayoutAtIndex(offset + i) = *memoizedLayoutAtIndex(i);
      }
    }
    // Compute the buffer of the new cells of the memoized table
    int maxCol = std::min(nbOfMemoizedColumns,
                          numberOfValuesColumns() - m_firstMemoizedColumn);
    int maxRow[maxCol];
    int maxOfMaxRow = -1;
    for (int col = 0; col < maxCol; col++) {
      maxRow[col] = std::min(
          k_maxNumberOfDisplayableRows,
          numberOfElementsInColumn(
              absoluteColumnForValuesColumn(col + m_firstMemoizedColumn)) -
              m_firstMemoizedRow);
      maxOfMaxRow = std::max(maxOfMaxRow, maxRow[col]);
    }
    /* We first loop on rows to step all sequences at the same time.
     * TODO: split this behavior between app grapher and app sequences? For
     * sequences, the number of rows is the same for all column, and for
     * grapher, this would enable us to restore previous loops (avoid row >=
     * maxRow[col] etc). */
    for (int row = 0; row < maxOfMaxRow; row++) {
      for (int col = 0; col < maxCol; col++) {
        if (row >= maxRow[col]) {
          continue;
        }
        // Escape if already filled
        if (col >= -offsetCol && col < -offsetCol + nbOfMemoizedColumns &&
            row >= -offsetRow &&
            row < -offsetRow + k_maxNumberOfDisplayableRows) {
          continue;
        }
        createMemoizedLayout(
            absoluteColumnForValuesColumn(m_firstMemoizedColumn + col),
            absoluteRowForValuesRow(m_firstMemoizedRow + row),
            row * nbOfMemoizedColumns + col);
      }
    }
  }
  return *memoizedLayoutAtIndex((valuesRow - m_firstMemoizedRow) *
                                    nbOfMemoizedColumns +
                                (valuesCol - m_firstMemoizedColumn));
}

void ValuesController::rowWasDeleted(int row, int column) {
  // Shift layout memoization

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

  // Update the memoization of rows below the deleted one:
  int nbOfColumnsForAbscissa = numberOfColumnsForAbscissaColumn(abscissaColumn);
  for (int r = memoizedRow; r < k_maxNumberOfDisplayableRows; r++) {
    for (int i = abscissaColumn + 1;
         i < abscissaColumn + nbOfColumnsForAbscissa; i++) {
      int memoizedI = valuesColumnForAbsoluteColumn(i) - m_firstMemoizedColumn;
      if (memoizedI < 0 || memoizedI >= k_maxNumberOfDisplayableColumns) {
        // The changed column is out of the memoized table
        continue;
      }
      if (r + 1 < k_maxNumberOfDisplayableRows) {
        *memoizedLayoutAtIndex(r * k_maxNumberOfDisplayableColumns +
                               memoizedI) =
            *memoizedLayoutAtIndex((r + 1) * k_maxNumberOfDisplayableColumns +
                                   memoizedI);
      } else {
        int currentAbsoluteRow = row + r - memoizedRow;
        if (currentAbsoluteRow < numberOfRowsAtColumn(i) - 1) {
          createMemoizedLayout(i, currentAbsoluteRow,
                               r * k_maxNumberOfDisplayableColumns + memoizedI);
        }
      }
    }
  }
}

void ValuesController::clearSelectedColumn() {
  intervalAtColumn(selectedColumn())->clear();
  selectCellAtLocation(selectedColumn(), 1);
  m_selectableTableView.resetSizeAndOffsetMemoization();
}

size_t ValuesController::fillColumnName(int column, char *buffer) {
  assert(typeAtLocation(column, 0) == k_abscissaTitleCellType);
  I18n::Message message = valuesParameterMessageAtColumn(column);
  return Print::CustomPrintf(buffer, k_maxSizeOfColumnName,
                             I18n::translate(message));
}

void ValuesController::setTitleCellText(HighlightCell *cell, int column) {
  if (typeAtLocation(column, 0) == k_functionTitleCellType) {
    Shared::ExpressionFunctionTitleCell *myCell =
        static_cast<Shared::ExpressionFunctionTitleCell *>(cell);
    myCell->setLayout(functionTitleLayout(column));
    return;
  }
  if (typeAtLocation(column, 0) == k_abscissaTitleCellType) {
    EvenOddMessageTextCell *myTitleCell =
        static_cast<EvenOddMessageTextCell *>(cell);
    myTitleCell->setMessage(valuesParameterMessageAtColumn(column));
    return;
  }
}

void ValuesController::setTitleCellStyle(HighlightCell *cell, int column) {
  if (typeAtLocation(column, 0) != k_functionTitleCellType) {
    return;
  }
  Shared::Function *function =
      functionStore()->modelForRecord(recordAtColumn(column)).pointer();
  static_cast<FunctionTitleCell *>(cell)->setColor(function->color());
}

void ValuesController::reloadEditedCell(int column, int row) {
  // Reload the row, if an existing value was edited.
  for (int i = 0; i < numberOfColumns(); i++) {
    selectableTableView()->reloadCellAtLocation(i, row);
  }
}

void ValuesController::initializeInterval() {
  intervalParameterController()->setInterval(
      intervalAtColumn(selectedColumn()));
  setStartEndMessages(intervalParameterController(), selectedColumn());
}

void ValuesController::initValueCells() {
  int numberOfValueCells = k_maxNumberOfDisplayableCells;
  for (int i = 0; i < numberOfValueCells; i++) {
    EvenOddExpressionCell *valueCell = valueCells(i);
    assert(valueCell);
    valueCell->setFont(k_cellFont);
    valueCell->setAlignment(KDGlyph::k_alignRight, KDGlyph::k_alignCenter);
    // TODO: Factorize margin computation
    valueCell->setMargins(
        {EvenOddCell::k_horizontalMargin, EvenOddCell::k_horizontalMargin});
  }
}

}  // namespace Shared
