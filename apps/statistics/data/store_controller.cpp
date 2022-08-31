#include "store_controller.h"
#include "../app.h"
#include <apps/constant.h>
#include <apps/shared/range_1D.h>
#include <apps/shared/poincare_helpers.h>
#include <assert.h>
#include <float.h>
#include <cmath>
#include <poincare/print.h>

using namespace Poincare;
using namespace Shared;
using namespace Escher;

namespace Statistics {

StoreController::StoreController(Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Store * store, ButtonRowController * header, Context * parentContext) :
  Shared::StoreController(parentResponder, inputEventHandlerDelegate, store, header, parentContext),
  m_store(store),
  m_storeParameterController(this, this, store)
{}

void StoreController::sortSelectedColumn() {
  int relativeIndex = m_store->relativeColumnIndex(selectedColumn());
  // Also sort the values if the cumulated frequency is selected
  m_store->sortColumn(selectedSeries(), relativeIndex != k_cumulatedFrequencyRelativeColumnIndex ? relativeIndex : 0);
}

int StoreController::fillColumnName(int columnIndex, char * buffer) {
  if (isCumulatedFrequencyColumn(columnIndex)) {
    // FC column options doesn't specify the column name.
    buffer[0] = 0;
    return 0;
  }
  return Shared::StoreController::fillColumnName(columnIndex, buffer);
}

int StoreController::numberOfColumns() const {
  int result = Shared::StoreController::numberOfColumns();
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    result += m_store->displayCumulatedFrequenciesForSeries(i);
  }
  return result;
}

void StoreController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  if (!isCumulatedFrequencyCell(i, j)) {
    return Shared::StoreController::willDisplayCellAtLocation(cell, i, j);
  }
  // Handle hidden cells
  const int numberOfElementsInCol = numberOfElementsInColumn(i);
  Shared::HideableEvenOddBufferTextCell * myCell = static_cast<Shared::HideableEvenOddBufferTextCell *>(cell);
  if (j > numberOfElementsInCol + 1) {
    myCell->setText("");
    myCell->setHide(true);
    return;
  }
  myCell->setHide(false);
  myCell->setEven(j%2 == 0);

  double value = (j == numberOfElementsInCol + 1) ? NAN : dataAtLocation(i, j);
  if (std::isnan(value)) {
    // Special case : last row and NaN
    myCell->setText("");
  } else {
    const int bufferSize = PrintFloat::charSizeForFloatsWithPrecision(Preferences::VeryLargeNumberOfSignificantDigits);
    char buffer[bufferSize];
    Shared::PoincareHelpers::ConvertFloatToTextWithDisplayMode<double>(value, buffer, bufferSize, Preferences::VeryLargeNumberOfSignificantDigits, Preferences::sharedPreferences()->displayMode());
    myCell->setText(buffer);
    KDColor textColor = m_store->seriesIsValid(m_store->seriesAtColumn(i)) ? KDColorBlack : Palette::GrayDark;
    myCell->setTextColor(textColor);
  }
}

bool StoreController::setDataAtLocation(double floatBody, int columnIndex, int rowIndex) {
  if (!Shared::StoreController::setDataAtLocation(floatBody, columnIndex, rowIndex)) {
    return false;
  }
  int series = m_store->seriesAtColumn(columnIndex);
  if (m_store->displayCumulatedFrequenciesForSeries(series)) {
    // Cumulated frequencies must be re-computed
    reloadSeriesVisibleCells(series, k_cumulatedFrequencyRelativeColumnIndex);
  }
  return true;
}

double StoreController::dataAtLocation(int columnIndex, int rowIndex) {
  if (isCumulatedFrequencyColumn(columnIndex)) {
    int series = m_store->seriesAtColumn(columnIndex);
    double value = m_store->get(series, 0, rowIndex - 1);
    return m_store->sumOfValuesBetween(series, -DBL_MAX, value, false);
  }
  return Shared::StoreController::dataAtLocation(columnIndex, rowIndex);
}

void StoreController::setTitleCellText(HighlightCell * cell, int columnIndex) {
  assert(typeAtLocation(columnIndex, 0) == k_titleCellType);
  StoreTitleCell * myTitleCell = static_cast<StoreTitleCell *>(cell);
  if (isCumulatedFrequencyColumn(columnIndex)) {
    myTitleCell->setText(I18n::translate(I18n::Message::CumulatedFrequencyColumnName));
  } else {
    char columnName[Shared::ClearColumnHelper::k_maxSizeOfColumnName];
    fillColumnName(columnIndex, columnName);
    char columnTitle[k_columnTitleSize]; // 50 is an ad-hoc value. A title cell can contain max 15 glyphs but the glyph can take more space than 1 byte in memory.
    I18n::Message titleType = m_store->relativeColumnIndex(columnIndex) % 2 == 1 ? I18n::Message::Frequencies : I18n::Message::Values;
    Poincare::Print::customPrintf(columnTitle, k_columnTitleSize, I18n::translate(titleType), columnName);
    myTitleCell->setText(columnTitle);
  }
}

void StoreController::clearSelectedColumn() {
  int series = m_store->seriesAtColumn(selectedColumn());
  int column = m_store->relativeColumnIndex(selectedColumn());
  if (column == 0) {
    m_store->deleteAllPairsOfSeries(series);
    selectCellAtLocation(selectedColumn(), 1);
  } else {
    m_store->resetColumn(series, column);
  }
}

void StoreController::setClearPopUpContent() {
  int column = m_store->relativeColumnIndex(selectedColumn());
  assert(column == 0 || column == 1);
  int series = m_store->seriesAtColumn(selectedColumn());
  if (column == 0) {
    char tableName[k_tableNameSize];
    FillSeriesName(series, tableName, true);
    m_confirmPopUpController.setMessageWithPlaceholder(I18n::Message::ClearTableConfirmation, tableName);
  } else {
    char columnNameBuffer[Shared::ColumnParameterController::k_titleBufferSize];
    fillColumnName(selectedColumn(), columnNameBuffer);
    m_confirmPopUpController.setMessageWithPlaceholder(I18n::Message::ResetFreqConfirmation, columnNameBuffer);
  }
}

void StoreController::FillSeriesName(int series, char * buffer, bool withFinalSpace) {
   /* We have to add a space in some cases because we use this table name in the message for
    * deleting the table in Graph and Sequence, but the table name is empty in Sequence.
    */
  char tableIndex = static_cast<char>('1' + series);
  Poincare::Print::customPrintf(buffer, k_tableNameSize, k_tableName, tableIndex, tableIndex);
  if (!withFinalSpace) {
    buffer[5] = 0;
  }
}

bool StoreController::deleteCellValue(int series, int i, int j, bool authorizeNonEmptyRowDeletion) {
  Escher::HighlightCell * selectedCell = nullptr;
  if (isCumulatedFrequencyColumn(i)) {
    selectedCell = selectableTableView()->selectedCell();
  }
  bool result = Shared::StoreController::deleteCellValue(series, i, j, authorizeNonEmptyRowDeletion);
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

InputViewController * StoreController::inputViewController() {
  return Statistics::App::app()->inputViewController();
}

}
