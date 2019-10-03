#include "values_controller.h"
#include "function_app.h"
#include <poincare/preferences.h>
#include <assert.h>
#include <limits.h>

using namespace Poincare;

namespace Shared {

static inline int minInt(int x, int y) { return x < y ? x : y; }

// Constructor and helpers

ValuesController::ValuesController(Responder * parentResponder, ButtonRowController * header) :
  EditableCellTableViewController(parentResponder),
  ButtonRowDelegate(header, nullptr),
  m_numberOfColumns(0),
  m_numberOfColumnsNeedUpdate(true),
  m_firstMemoizedColumn(INT_MAX),
  m_firstMemoizedRow(INT_MAX),
  m_abscissaParameterController(this)
{
}

void ValuesController::setupSelectableTableViewAndCells(InputEventHandlerDelegate * inputEventHandlerDelegate) {
  selectableTableView()->setVerticalCellOverlap(0);
  selectableTableView()->setMargins(k_margin, k_scrollBarMargin, k_scrollBarMargin, k_margin);
  selectableTableView()->setBackgroundColor(Palette::WallScreenDark);

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
  EditableCellTableViewController::viewWillAppear();
  header()->setSelectedButton(-1);
  resetMemoization();
}

void ValuesController::viewDidDisappear() {
  m_numberOfColumnsNeedUpdate = true;
  EditableCellTableViewController::viewDidDisappear();
}

// Responder

bool ValuesController::handleEvent(Ion::Events::Event event) {
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
    intervalAtColumn(selectedColumn())->deleteElementAtIndex(selectedRow()-1);
    selectableTableView()->reloadData();
    return true;
  }
  if (selectedRow() == -1) {
    return header()->handleEvent(event);
  }
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && selectedRow() == 0) {
    ViewController * parameterController = nullptr;
    if (typeAtLocation(selectedColumn(), 0) == k_abscissaTitleCellType) {
      m_abscissaParameterController.setPageTitle(valuesParameterMessageAtColumn(selectedColumn()));
      intervalParameterController()->setInterval(intervalAtColumn(selectedColumn()));
      setStartEndMessages(intervalParameterController(), selectedColumn());
      parameterController = &m_abscissaParameterController;
    } else {
      parameterController = functionParameterController();
    }
    if (parameterController) {
      stackController()->push(parameterController);
    }
    return true;
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
  // The cell is not a title cell and not editable
  if (typeAtLocation(i,j) == k_notEditableValueCellType) {
    // Special case: last row
    if (j == numberOfElementsInColumn(i) + 1) {
      static_cast<EvenOddBufferTextCell *>(cell)->setText("");
    } else {
      static_cast<EvenOddBufferTextCell *>(cell)->setText(memoizedBufferForCell(i, j));
    }
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

bool ValuesController::setDataAtLocation(double floatBody, int columnIndex, int rowIndex) {
  intervalAtColumn(columnIndex)->setElement(rowIndex-1, floatBody);
  return true;
}

bool ValuesController::cellAtLocationIsEditable(int columnIndex, int rowIndex) {
  return typeAtLocation(columnIndex, rowIndex) == k_editableValueCellType;
}

double ValuesController::dataAtLocation(int columnIndex, int rowIndex) {
  return intervalAtColumn(columnIndex)->element(rowIndex-1);
}

void ValuesController::didChangeRow(int row) {
  /* Update the row memoization if it exists */
  // Conversion of coordinates from absolute table to values table
  int valuesRow = valuesRowForAbsoluteRow(row);
  if (m_firstMemoizedRow > valuesRow || valuesRow >= m_firstMemoizedRow + k_maxNumberOfRows) {
    // The changed row is out of the memoized table
    return;
  }

  int memoizedRow = valuesRow - m_firstMemoizedRow;
  int maxI = numberOfValuesColumns() - m_firstMemoizedColumn;
  int nbOfMemoizedColumns = numberOfMemoizedColumn();
  for (int i = 0; i < minInt(nbOfMemoizedColumns, maxI); i++) {
    fillMemoizedBuffer(absoluteColumnForValuesColumn(m_firstMemoizedColumn + i), row, nbOfMemoizedColumns*memoizedRow+i);
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

void ValuesController::resetMemoization() {
  m_firstMemoizedColumn = INT_MAX;
  m_firstMemoizedRow = INT_MAX;
}

void ValuesController::moveMemoizedBuffer(int destinationI, int destinationJ, int sourceI, int sourceJ) {
  int nbOfMemoizedColumns = numberOfMemoizedColumn();
  strlcpy(memoizedBufferAtIndex(destinationJ*nbOfMemoizedColumns + destinationI), memoizedBufferAtIndex(sourceJ*nbOfMemoizedColumns + sourceI), valuesCellBufferSize());
}

char * ValuesController::memoizedBufferForCell(int i, int j) {
  // Conversion of coordinates from absolute table to values table
  int valuesI = valuesColumnForAbsoluteColumn(i);
  int valuesJ = valuesRowForAbsoluteRow(j);
  int nbOfMemoizedColumns = numberOfMemoizedColumn();
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
  } else if (valuesJ >= m_firstMemoizedRow + k_maxNumberOfRows) {
    offsetJ = valuesJ - k_maxNumberOfRows - m_firstMemoizedRow + 1;
  }

  // Apply the offset
  if (offsetI != 0 || offsetJ != 0) {
    m_firstMemoizedColumn = m_firstMemoizedColumn + offsetI;
    m_firstMemoizedRow = m_firstMemoizedRow + offsetJ;
    // Translate already memoized cells
    int maxI = numberOfValuesColumns() - m_firstMemoizedColumn;
    for (int ii = offsetI > 0 ? 0 : minInt(nbOfMemoizedColumns, maxI)-1;  offsetI > 0 ? ii < minInt(-offsetI + nbOfMemoizedColumns, maxI) : ii >= -offsetI; ii += offsetI > 0 ? 1 : -1) {
      int maxJ = numberOfElementsInColumn(absoluteColumnForValuesColumn(ii+m_firstMemoizedColumn)) - m_firstMemoizedRow;
      for (int jj = offsetJ > 0 ? 0 : minInt(k_maxNumberOfRows, maxJ)-1; offsetJ > 0 ? jj < minInt(-offsetJ+k_maxNumberOfRows, maxJ) : jj >= -offsetJ; jj += offsetJ > 0 ? 1 : -1) {
        moveMemoizedBuffer(ii, jj, ii+offsetI, jj+offsetJ);
      }
    }
    // Compute the buffer of the new cells of the memoized table
    for (int ii = 0; ii < minInt(nbOfMemoizedColumns, maxI); ii++) {
      int maxJ = numberOfElementsInColumn(absoluteColumnForValuesColumn(ii+m_firstMemoizedColumn)) - m_firstMemoizedRow;
      for (int jj = 0; jj < minInt(k_maxNumberOfRows, maxJ); jj++) {
        // Escape if already filled
        if (ii >= -offsetI && ii < -offsetI + nbOfMemoizedColumns && jj >= -offsetJ && jj < -offsetJ + k_maxNumberOfRows) {
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

}
