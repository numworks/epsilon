#include "store_controller.h"

#include <apps/apps_container.h>
#include <apps/constant.h>
#include <apps/shared/poincare_helpers.h>
#include <apps/shared/store_app.h>
#include <assert.h>
#include <escher/metric.h>

#include <algorithm>

using namespace Poincare;
using namespace Escher;

namespace Shared {

StoreController::StoreController(Responder* parentResponder,
                                 DoublePairStore* store,
                                 ButtonRowController* header,
                                 Context* parentContext)
    : EditableCellTableViewController(parentResponder, &m_prefacedTableView),
      ButtonRowDelegate(header, nullptr),
      StoreColumnHelper(this, parentContext, this),
      m_prefacedTableView(0, this, &m_selectableTableView, this, this),
      m_store(store),
      m_widthManager(this) {
  m_prefacedTableView.setBackgroundColor(Palette::WallScreenDark);
  m_prefacedTableView.setCellOverlap(0, 0);
  m_prefacedTableView.setMargins(k_margins);
  for (int i = 0; i < k_maxNumberOfDisplayableCells; i++) {
    m_editableCells[i].setParentResponder(&m_selectableTableView);
    m_editableCells[i].editableTextCell()->textField()->setDelegate(this);
  }
}

bool StoreController::textFieldDidFinishEditing(AbstractTextField* textField,
                                                Ion::Events::Event event) {
  // First row is not editable.
  assert(selectedRow() != 0);
  int series = m_store->seriesAtColumn(selectedColumn());
  bool wasSeriesValid = m_store->seriesIsActive(series);
  if (textField->draftText()[0] == 0) {  // If text = "", delete the cell
    bool didDeleteRow = false;
    handleDeleteEvent(true, &didDeleteRow);
    if (event == Ion::Events::Up || event == Ion::Events::Left ||
        event == Ion::Events::Right ||
        (event == Ion::Events::Down && !didDeleteRow)) {
      /* Do nothing if down was pressed and the row is deleted since
       * it already selects the next row. */
      selectableTableView()->handleEvent(event);
    }
    return true;
  }
  bool result = EditableCellTableViewController::textFieldDidFinishEditing(
      textField, event);
  if (wasSeriesValid != m_store->seriesIsActive(series)) {
    // Series changed validity, series' cells have changed color.
    reloadSeriesVisibleCells(series);
  }
  return result;
}

int StoreController::numberOfColumns() const {
  return DoublePairStore::k_numberOfColumnsPerSeries *
         DoublePairStore::k_numberOfSeries;
}

HighlightCell* StoreController::reusableCell(int index, int type) {
  assert(index >= 0);
  switch (type) {
    case k_titleCellType:
      assert(index < k_maxNumberOfDisplayableColumns);
      return &m_titleCells[index];
    case k_editableCellType:
      assert(index < k_maxNumberOfDisplayableCells);
      return &m_editableCells[index];
    default:
      assert(false);
      return nullptr;
  }
}

int StoreController::reusableCellCount(int type) const {
  return type == k_titleCellType ? k_maxNumberOfDisplayableColumns
                                 : k_maxNumberOfDisplayableCells;
}

int StoreController::typeAtLocation(int column, int row) const {
  return row == 0 ? k_titleCellType : k_editableCellType;
}

void StoreController::fillCellForLocation(HighlightCell* cell, int column,
                                          int row) {
  // Handle hidden cells
  const int numberOfElementsInCol = numberOfElementsInColumn(column);
  if (row > numberOfElementsInCol + 1) {
    Escher::AbstractEvenOddEditableTextCell* myCell =
        static_cast<Escher::AbstractEvenOddEditableTextCell*>(cell);
    myCell->editableTextCell()->textField()->setText("");
    myCell->setVisible(false);
    return;
  }
  if (typeAtLocation(column, row) == k_editableCellType) {
    Escher::AbstractEvenOddEditableTextCell* myCell =
        static_cast<Escher::AbstractEvenOddEditableTextCell*>(cell);
    myCell->setVisible(true);
    KDColor textColor =
        (m_store->seriesIsActive(m_store->seriesAtColumn(column)) ||
         m_store->numberOfPairsOfSeries(m_store->seriesAtColumn(column)) == 0)
            ? KDColorBlack
            : Palette::GrayDark;
    myCell->editableTextCell()->textField()->setTextColor(textColor);
  }
  fillCellForLocationWithDisplayMode(
      cell, column, row, Preferences::SharedPreferences()->displayMode(),
      AbstractEvenOddBufferTextCell::k_defaultPrecision);
}

KDCoordinate StoreController::separatorBeforeColumn(int column) const {
  return column > 0 && m_store->relativeColumn(column) == 0
             ? Escher::Metric::TableSeparatorThickness
             : 0;
}

void StoreController::viewWillAppear() {
  // Reset size memoization in case the number of columns changed
  m_selectableTableView.resetSizeAndOffsetMemoization();
  EditableCellTableViewController::viewWillAppear();
}

void StoreController::setTitleCellText(HighlightCell* cell, int column) {
  // Default : put column name in titleCell
  BufferFunctionTitleCell* myTitleCell =
      static_cast<BufferFunctionTitleCell*>(cell);
  fillColumnName(column, const_cast<char*>(myTitleCell->text()));
}

void StoreController::setTitleCellStyle(HighlightCell* cell, int column) {
  int seriesIndex = m_store->seriesAtColumn(column);
  Shared::BufferFunctionTitleCell* myCell =
      static_cast<Shared::BufferFunctionTitleCell*>(cell);
  // TODO Share GrayDark with graph/list_controller
  myCell->setColor(!m_store->seriesIsActive(seriesIndex)
                       ? Palette::GrayDark
                       : DoublePairStore::colorOfSeriesAtIndex(seriesIndex));
  myCell->setFont(KDFont::Size::Small);
}

bool StoreController::handleEvent(Ion::Events::Event event) {
  if (EditableCellTableViewController::handleEvent(event)) {
    return true;
  }
  if (event == Ion::Events::Up) {
    selectableTableView()->deselectTable();
    assert(selectedRow() == -1);
    tabController()->selectTab();
    return true;
  }
  if (event == Ion::Events::Backspace) {
    handleDeleteEvent();
    return true;
  }
  return false;
}

void StoreController::handleDeleteEvent(bool authorizeNonEmptyRowDeletion,
                                        bool* didDeleteRow) {
  int col = selectedColumn();
  int row = selectedRow();
  assert(col >= 0 && col < numberOfColumns());
  int series = m_store->seriesAtColumn(col);
  assert(row >= 0);
  if (row == 0 || row > numberOfElementsInColumn(col)) {
    return;
  }
  if (deleteCellValue(series, col, row, authorizeNonEmptyRowDeletion)) {
    // A row has been deleted
    if (didDeleteRow) {
      *didDeleteRow = true;
    }
    selectableTableView()->reloadData();
  } else {
    if (didDeleteRow) {
      *didDeleteRow = false;
    }
    reloadSeriesVisibleCells(series);
  }
  resetMemoizedFormulasOfEmptyColumns(series);
}

void StoreController::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::HasBecomeFirst) {
    if (selectedRow() < 0 || selectedColumn() < 0) {
      selectCellAtLocation(0, 0);
    }
    EditableCellTableViewController::handleResponderChainEvent(event);
  } else {
    EditableCellTableViewController::handleResponderChainEvent(event);
  }
}

bool StoreController::deleteCellValue(int series, int i, int j,
                                      bool authorizeNonEmptyRowDeletion) {
  return m_store->deleteValueAtIndex(series, m_store->relativeColumn(i), j - 1,
                                     authorizeNonEmptyRowDeletion);
}

StackViewController* StoreController::stackController() const {
  return static_cast<StackViewController*>(
      parentResponder()->parentResponder());
}

Escher::TabViewController* StoreController::tabController() const {
  return static_cast<Escher::TabViewController*>(
      parentResponder()->parentResponder()->parentResponder());
}

bool StoreController::cellAtLocationIsEditable(int column, int row) {
  return typeAtLocation(column, row) == k_editableCellType;
}

bool StoreController::checkDataAtLocation(double floatBody, int column,
                                          int row) const {
  return m_store->valueValidInColumn(floatBody,
                                     m_store->relativeColumn(column));
}

bool StoreController::setDataAtLocation(double floatBody, int column, int row) {
  assert(checkDataAtLocation(floatBody, column, row));
  return m_store->set(floatBody, m_store->seriesAtColumn(column),
                      m_store->relativeColumn(column), row - 1, false, true);
}

double StoreController::dataAtLocation(int column, int row) {
  return m_store->get(m_store->seriesAtColumn(column),
                      m_store->relativeColumn(column), row - 1);
}

int StoreController::numberOfElementsInColumn(int column) const {
  return m_store->numberOfPairsOfSeries(m_store->seriesAtColumn(column));
}

void StoreController::resetMemoizedFormulaOfColumn(int series, int column) {
  memoizeFormula(Layout(),
                 series * DoublePairStore::k_numberOfColumnsPerSeries + column);
}

void StoreController::resetMemoizedFormulasOfEmptyColumns(int series) {
  assert(series >= 0 && series < DoublePairStore::k_numberOfSeries);
  for (int i = 0; i < DoublePairStore::k_numberOfColumnsPerSeries; i++) {
    if (m_store->lengthOfColumn(series, i) == 0) {
      resetMemoizedFormulaOfColumn(series, i);
    }
  }
}

void StoreController::loadMemoizedFormulasFromSnapshot() {
  for (int i = 0; i < StoreApp::Snapshot::k_numberOfMemoizedFormulas; i++) {
    if (m_store->numberOfPairsOfSeries(m_store->seriesAtColumn(i)) == 0) {
      /* The series could have been emptied outside of the app. If it's the
       * case, reset the memoized formula. */
      m_memoizedFormulas[i] = Layout();
    } else {
      m_memoizedFormulas[i] =
          StoreApp::storeApp()->storeAppSnapshot()->memoizedFormula(i);
    }
  }
}

void StoreController::memoizeFormula(const Poincare::Layout& formula,
                                     int index) {
  m_memoizedFormulas[index] = formula;
  StoreApp::storeApp()->storeAppSnapshot()->memoizeFormula(formula, index);
}

void StoreController::setClearPopUpContent() {
  constexpr size_t bufferSize = Shared::BufferPopUpController::MaxTextSize();
  char buffer[bufferSize];
  clearPopUpText(selectedColumn(), buffer, bufferSize);
  m_confirmPopUpController.setContentText(buffer);
}

}  // namespace Shared
