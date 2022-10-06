#include "calculation_controller.h"
#include <apps/shared/poincare_helpers.h>
#include <poincare/preferences.h>
#include <poincare/print.h>
#include <apps/i18n.h>
#include <assert.h>

using namespace Shared;
using namespace Poincare;
using namespace Escher;

namespace Statistics {

CalculationController::CalculationController(Responder * parentResponder, ButtonRowController * header, Store * store) :
  DoublePairTableController(parentResponder, header),
  m_store(store)
{
  for (int i = 0; i < k_numberOfSeriesTitleCells; i++) {
    m_seriesTitleCells[i].setSeparatorLeft(true);
  }
  for (int i = 0; i < k_numberOfCalculationTitleCells; i++) {
    m_calculationTitleCells[i].setAlignment(KDContext::k_alignRight, KDContext::k_alignCenter);
    m_calculationTitleCells[i].setMessageFont(KDFont::Size::Small);
    m_calculationSymbolCells[i].setAlignment(KDContext::k_alignCenter, KDContext::k_alignCenter);
    m_calculationSymbolCells[i].setMessageFont(KDFont::Size::Small);
    m_calculationModeTitleCells[i].setAlignment(KDContext::k_alignRight, KDContext::k_alignCenter);
    m_calculationModeTitleCells[i].setFont(KDFont::Size::Small);
    m_calculationModeSymbolCells[i].setAlignment(KDContext::k_alignCenter, KDContext::k_alignCenter);
    m_calculationModeSymbolCells[i].setFont(KDFont::Size::Small);
  }
  for (int i = 0; i < k_numberOfHeaderColumns; i++) {
    m_hideableCell[i].hide();
  }
  resetMemoization();
}

void CalculationController::viewWillAppear() {
  resetMemoization();
  Shared::DoublePairTableController::viewWillAppear();
}

void CalculationController::tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) {
  if (withinTemporarySelection) {
    return;
  }
  /* To prevent selecting cell with no content (top left corner of the table),
   * as soon as the selected cell is the top left corner, we either reselect
   * the previous cell or select the tab controller depending on from which cell
   * the selection comes. This trick does not create an endless loop as the
   * previous cell cannot be the top left corner cell if it also is the
   * selected one. */
  if (t->selectedRow() == 0 && t->selectedColumn() <= 1) {
    if (previousSelectedCellX <= 1 && previousSelectedCellY == 1) {
      selectableTableView()->deselectTable();
      Container::activeApp()->setFirstResponder(tabController());
    } else {
      t->selectCellAtLocation(selectedColumn(), 1);
    }
  }
}

// TableViewDataSource

int CalculationController::numberOfColumns() const {
  return 2 + m_store->numberOfValidSeries();
}

void CalculationController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  EvenOddCell * evenOddCell = static_cast<EvenOddCell *>(cell);
  evenOddCell->setEven(j%2 == 0);
  evenOddCell->setHighlighted(i == selectedColumn() && j == selectedRow());
  int type = typeAtLocation(i, j);
  int numberOfFixedRows = fixedNumberOfRows();
  switch (type) {
  case k_seriesTitleCellType: {
    // Display a series title cell
    int seriesNumber = m_store->indexOfKthValidSeries(i-2);
    char titleBuffer[] = {'V', static_cast<char>('1'+seriesNumber), '/', 'N', static_cast<char>('1'+seriesNumber), 0};
    StoreTitleCell * storeTitleCell = static_cast<StoreTitleCell *>(cell);
    storeTitleCell->setText(titleBuffer);
    storeTitleCell->setColor(DoublePairStore::colorOfSeriesAtIndex(seriesNumber));
    return;
  }
  case k_calculationModeTitleCellType:
  case k_calculationModeSymbolCellType: {
    assert((i == 0 && type == k_calculationModeTitleCellType) || (i == 1 && type == k_calculationModeSymbolCellType));
    // Mode title and symbol cells have an index value
    const char * pattern = (m_store->totalNumberOfModes() == 1 ? "%s" : (i == 0 ? "%s %i" : "%s%i"));
    I18n::Message message = (i == 0 ? I18n::Message::Mode : I18n::Message::ModeSymbol);
    int index = j - numberOfFixedRows + 1;
    assert(j >= 1);
    // The NL "Modus 100" is the longest possible text here.
    constexpr static int bufferSize = sizeof("Modus 100") / sizeof(char);
    char buffer[bufferSize];
    Poincare::Print::CustomPrintf(buffer, bufferSize, pattern, I18n::translate(message), index);
    EvenOddBufferTextCell * bufferCell = static_cast<EvenOddBufferTextCell *>(cell);
    bufferCell->setTextColor(i == 1 ? Palette::GrayDark : KDColorBlack);
    bufferCell->setText(buffer);
    return;
  }
  case k_calculationTitleCellType:
  case k_calculationSymbolCellType: {
    assert(j >= 1 && ((i == 0 && type == k_calculationTitleCellType) || (i == 1 && type == k_calculationSymbolCellType)));
    // Display a calculation title or symbol
    I18n::Message message;
    if (j - 1 < numberOfFixedRows - 1) {
      int messageIndex = findCellIndex(j - 1);
      message = i == 1 ? k_calculationRows[messageIndex].symbol : k_calculationRows[messageIndex].title;
    } else {
      // titles index does not include the Mode Frequency messages
      assert(showModeFrequency() && j == numberOfRows() - 1);
      I18n::Message modeFrequencyTitles[k_numberOfHeaderColumns] = { I18n::Message::ModeFrequency, I18n::Message::ModeFrequencySymbol };
      message = modeFrequencyTitles[i];
    }
    EvenOddMessageTextCell * calcTitleCell = static_cast<EvenOddMessageTextCell *>(cell);
    calcTitleCell->setTextColor(i == 1 ? Palette::GrayDark : KDColorBlack);
    calcTitleCell->setMessage(message);
    return;
  }
  case k_calculationCellType: {
    // Display a calculation cell
    int seriesIndex = m_store->indexOfKthValidSeries(i-2);
    double calculation;
    EvenOddBufferTextCell * calculationCell = static_cast<EvenOddBufferTextCell *>(cell);
    if (j - 1 < (numberOfFixedRows - 1)) {
      int calculationIndex = findCellIndex(j - 1);
      if (std::isnan(m_memoizedCellContent[seriesIndex][calculationIndex])) {
        m_memoizedCellContent[seriesIndex][calculationIndex] = (m_store->*k_calculationRows[calculationIndex].calculationMethod)(seriesIndex);
      }
      calculation = m_memoizedCellContent[seriesIndex][calculationIndex];
      assert(calculation == (m_store->*k_calculationRows[calculationIndex].calculationMethod)(seriesIndex)
          || (std::isnan(calculation) && std::isnan((m_store->*k_calculationRows[calculationIndex].calculationMethod)(seriesIndex))));
    } else if (showModeFrequency() && j == numberOfRows() - 1) {
      calculation = m_store->modeFrequency(seriesIndex);
    } else {
      int modeIndex = j - numberOfFixedRows;
      if (modeIndex < m_store->numberOfModes(seriesIndex)) {
        calculation = m_store->modeAtIndex(seriesIndex, modeIndex);
      } else {
        calculationCell->setText("-");
        return;
      }
    }
    constexpr int precision = Preferences::VeryLargeNumberOfSignificantDigits;
    constexpr int bufferSize = PrintFloat::charSizeForFloatsWithPrecision(precision);
    char buffer[bufferSize];
    PoincareHelpers::ConvertFloatToText<double>(calculation, buffer, bufferSize, precision);
    calculationCell->setText(buffer);
    return;
  }
  }
}

KDCoordinate CalculationController::nonMemoizedColumnWidth(int i) {
  if (i == 0) {
    return k_calculationTitleCellWidth;
  }
  if (i == 1) {
    int numberOfModes = m_store->totalNumberOfModes();
    static_assert(Store::k_maxNumberOfPairs < 1000, "numberOfChars must be updated");
    // Mod1, Mod10 and Mod100
    int numberOfChars = (numberOfModes < 10 ? 4 : (numberOfModes < 100) ? 5 : 6);
    return CalculationSymbolCellWidth(numberOfChars);
  }
  return k_calculationCellWidth;
}

HighlightCell * CalculationController::reusableCell(int index, int type) {
  assert(index >= 0 && index < reusableCellCount(type));
  switch (type) {
  case k_hideableCellType:
    return &m_hideableCell[index];
  case k_calculationTitleCellType:
    return &m_calculationTitleCells[index];
  case k_calculationSymbolCellType:
    return &m_calculationSymbolCells[index];
  case k_calculationModeTitleCellType:
    return &m_calculationModeTitleCells[index];
  case k_calculationModeSymbolCellType:
    return &m_calculationModeSymbolCells[index];
  case k_seriesTitleCellType:
    return &m_seriesTitleCells[index];
  default:
    assert(type == k_calculationCellType);
    return &m_calculationCells[index];
  }
}

int CalculationController::reusableCellCount(int type) {
  switch (type) {
  case k_hideableCellType:
    return 2;
  case k_calculationTitleCellType:
    return k_numberOfCalculationTitleCells;
  case k_calculationSymbolCellType:
    return k_numberOfCalculationTitleCells;
  case k_calculationModeTitleCellType:
    return k_numberOfCalculationTitleCells;
  case k_calculationModeSymbolCellType:
    return k_numberOfCalculationTitleCells;
  case k_seriesTitleCellType:
    return k_numberOfSeriesTitleCells;
  default:
    assert(type == k_calculationCellType);
    return k_numberOfCalculationCells;
  }
}

int CalculationController::typeAtLocation(int i, int j) {
  assert(i >= 0 && i < numberOfColumns());
  assert(j >= 0 && j < numberOfRows());
  if (i <= 1 && j == 0) {
    return k_hideableCellType;
  }
  if (i <= 1) {
    assert(j > 0);
    if (j < fixedNumberOfRows() || (showModeFrequency() && j == numberOfRows() - 1)) {
      return i == 0 ? k_calculationTitleCellType : k_calculationSymbolCellType;
    }
    return i == 0 ? k_calculationModeTitleCellType : k_calculationModeSymbolCellType;
  }
  if (j == 0) {
    return k_seriesTitleCellType;
  }
  return k_calculationCellType;
}

int CalculationController::findCellIndex(int i) const {
  int returnIndex = 0;
  while (returnIndex <= fixedNumberOfRows()) {
    if (i == (GlobalPreferences::sharedGlobalPreferences()->statsRowsLayout() == CountryPreferences::StatsRowsLayout::Variant1 ? k_calculationRows[returnIndex].variant1Index : k_calculationRows[returnIndex].defaultIndex)) {
      return returnIndex;
    }
    returnIndex++;
  }
  assert(false);
  return 0;
}

int CalculationController::fixedNumberOfRows() const {
  // Hide SampleMean under default StatsRowLayout
  return GlobalPreferences::sharedGlobalPreferences()->statsRowsLayout() == CountryPreferences::StatsRowsLayout::Variant1 ? k_fixedMaxNumberOfRows : k_fixedMaxNumberOfRows - 1;
}

void CalculationController::resetMemoization(bool force) {
  DoublePairTableController::resetMemoization(force);
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    for (int j = 0; j < k_numberOfCalculations; j++) {
      m_memoizedCellContent[i][j] = NAN;
    }
  }
}

}
