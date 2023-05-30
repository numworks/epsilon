#include "store_controller.h"

#include <apps/constant.h>
#include <apps/shared/poincare_helpers.h>
#include <assert.h>
#include <float.h>
#include <poincare/print.h>

#include <cmath>

#include "../app.h"

using namespace Poincare;
using namespace Shared;
using namespace Escher;

namespace Statistics {

StoreController::StoreController(Responder *parentResponder, Store *store,
                                 ButtonRowController *header,
                                 Context *parentContext)
    : Shared::StoreController(parentResponder, store, header, parentContext),
      m_store(store),
      m_storeParameterController(this, this, store) {}

void StoreController::sortSelectedColumn() {
  int relativeIndex = m_store->relativeColumn(selectedColumn());
  // Also sort the values if the cumulated frequency is selected
  m_store->sortColumn(
      selectedSeries(),
      relativeIndex != k_cumulatedFrequencyRelativeColumn ? relativeIndex : 0);
}

int StoreController::fillColumnName(int column, char *buffer) {
  if (isCumulatedFrequencyColumn(column)) {
    // FC column options doesn't specify the column name.
    buffer[0] = 0;
    return 0;
  }
  return Shared::StoreController::fillColumnName(column, buffer);
}

int StoreController::numberOfColumns() const {
  int result = Shared::StoreController::numberOfColumns();
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    result += m_store->displayCumulatedFrequenciesForSeries(i);
  }
  return result;
}

void StoreController::fillCellForLocation(HighlightCell *cell, int column,
                                          int row) {
  if (typeAtLocation(column, row) != k_nonEditableCellType) {
    return Shared::StoreController::fillCellForLocation(cell, column, row);
  }
  // Handle hidden cells
  const int numberOfElementsInCol = numberOfElementsInColumn(column);
  AbstractEvenOddBufferTextCell *myCell =
      static_cast<AbstractEvenOddBufferTextCell *>(cell);
  if (row > numberOfElementsInCol + 1) {
    myCell->setText("");
    myCell->setVisible(false);
    return;
  }
  myCell->setVisible(true);
  myCell->setEven(row % 2 == 0);

  double value =
      (row == numberOfElementsInCol + 1) ? NAN : dataAtLocation(column, row);
  if (std::isnan(value)) {
    // Special case : last row and NaN
    myCell->setText("");
  } else {
    constexpr int bufferSize = PrintFloat::charSizeForFloatsWithPrecision(
        AbstractEvenOddBufferTextCell::k_defaultPrecision);
    char buffer[bufferSize];
    Shared::PoincareHelpers::ConvertFloatToTextWithDisplayMode<double>(
        value, buffer, bufferSize,
        AbstractEvenOddBufferTextCell::k_defaultPrecision,
        Preferences::sharedPreferences->displayMode());
    myCell->setText(buffer);
    KDColor textColor = m_store->seriesIsActive(m_store->seriesAtColumn(column))
                            ? KDColorBlack
                            : Palette::GrayDark;
    myCell->setTextColor(textColor);
  }
}

bool StoreController::setDataAtLocation(double floatBody, int column, int row) {
  if (!Shared::StoreController::setDataAtLocation(floatBody, column, row)) {
    return false;
  }
  int series = m_store->seriesAtColumn(column);
  if (m_store->displayCumulatedFrequenciesForSeries(series)) {
    // Cumulated frequencies must be re-computed
    reloadSeriesVisibleCells(series, k_cumulatedFrequencyRelativeColumn);
  }
  return true;
}

double StoreController::dataAtLocation(int column, int row) {
  if (isCumulatedFrequencyColumn(column)) {
    int series = m_store->seriesAtColumn(column);
    double value = m_store->get(series, 0, row - 1);
    return m_store->sumOfValuesBetween(series, -DBL_MAX, value, false);
  }
  return Shared::StoreController::dataAtLocation(column, row);
}

void StoreController::setTitleCellText(HighlightCell *cell, int column) {
  assert(typeAtLocation(column, 0) == k_titleCellType);
  BufferFunctionTitleCell *myTitleCell =
      static_cast<BufferFunctionTitleCell *>(cell);
  if (isCumulatedFrequencyColumn(column)) {
    myTitleCell->setText(
        I18n::translate(I18n::Message::CumulatedFrequencyColumnName));
  } else {
    char columnName[Shared::ClearColumnHelper::k_maxSizeOfColumnName];
    fillColumnName(column, columnName);
    /* 50 is an ad-hoc value. A title cell can contain max 15 glyphs but the
     * glyph can take more space than 1 byte in memory. */
    char columnTitle[k_columnTitleSize];
    I18n::Message titleType = m_store->relativeColumn(column) % 2 == 1
                                  ? I18n::Message::Frequencies
                                  : I18n::Message::Values;
    Poincare::Print::CustomPrintf(columnTitle, k_columnTitleSize,
                                  I18n::translate(titleType), columnName);
    myTitleCell->setText(columnTitle);
  }
}

void StoreController::clearSelectedColumn() {
  int series = m_store->seriesAtColumn(selectedColumn());
  int column = m_store->relativeColumn(selectedColumn());
  if (column == 0) {
    m_store->deleteAllPairsOfSeries(series);
    selectCellAtLocation(selectedColumn(), 1);
    resetMemoizedFormulasOfEmptyColumns(series);
  } else {
    m_store->resetColumn(series, column);
  }
}

void StoreController::setClearPopUpContent() {
  int column = m_store->relativeColumn(selectedColumn());
  assert(column == 0 || column == 1);
  int series = m_store->seriesAtColumn(selectedColumn());
  if (column == 0) {
    char tableName[k_tableNameSize];
    FillSeriesName(series, tableName, true);
    m_confirmPopUpController.setMessageWithPlaceholders(
        I18n::Message::ClearTableConfirmation, tableName);
  } else {
    char columnNameBuffer[Shared::ColumnParameterController::k_titleBufferSize];
    fillColumnName(selectedColumn(), columnNameBuffer);
    m_confirmPopUpController.setMessageWithPlaceholders(
        I18n::Message::ResetFreqConfirmation, columnNameBuffer);
  }
}

void StoreController::FillSeriesName(int series, char *buffer,
                                     bool withFinalSpace) {
  /* We have to add a space in some cases because we use this table name in the
   * message for deleting the table in Graph and Sequence, but the table name is
   * empty in Sequence.
   */
  char tableIndex = static_cast<char>('1' + series);
  Poincare::Print::CustomPrintf(buffer, k_tableNameSize, k_tableName,
                                tableIndex, tableIndex);
  if (!withFinalSpace) {
    buffer[5] = 0;
  }
}

bool StoreController::deleteCellValue(int series, int i, int j,
                                      bool authorizeNonEmptyRowDeletion) {
  Escher::HighlightCell *selectedCell = nullptr;
  if (isCumulatedFrequencyColumn(i)) {
    selectedCell = selectableTableView()->selectedCell();
  }
  bool result = Shared::StoreController::deleteCellValue(
      series, i, j, authorizeNonEmptyRowDeletion);
  if (selectedCell && result && m_store->numberOfPairsOfSeries(series) == 0) {
    assert(j == 1);
    /* Deleting the last cumulated frequency value will remove its column.
     * The previously selected cell needs to be un-highlighted and selection
     * must shift to the left. */
    selectedCell->setHighlighted(false);
    assert(i > 0);
    selectCellAtLocation(i - 1, j);
  }
  return result;
}

InputViewController *StoreController::inputViewController() {
  return Statistics::App::app()->inputViewController();
}

}  // namespace Statistics
