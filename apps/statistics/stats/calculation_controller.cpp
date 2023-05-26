#include "calculation_controller.h"

#include <apps/i18n.h>
#include <apps/shared/poincare_helpers.h>
#include <assert.h>
#include <poincare/preferences.h>
#include <poincare/print.h>

using namespace Shared;
using namespace Poincare;
using namespace Escher;

namespace Statistics {

CalculationController::CalculationController(Responder *parentResponder,
                                             ButtonRowController *header,
                                             Store *store)
    : DoublePairTableController(parentResponder, header), m_store(store) {
  for (int i = 0; i < k_numberOfSeriesTitleCells; i++) {
    m_seriesTitleCells[i].setFont(KDFont::Size::Small);
  }
  for (int i = 0; i < k_numberOfCalculationTitleCells; i++) {
    m_calculationTitleCells[i].setAlignment(KDGlyph::k_alignRight,
                                            KDGlyph::k_alignCenter);
    m_calculationTitleCells[i].setMessageFont(KDFont::Size::Small);
    m_calculationSymbolCells[i].setAlignment(KDGlyph::k_alignCenter,
                                             KDGlyph::k_alignCenter);
    m_calculationSymbolCells[i].setMessageFont(KDFont::Size::Small);
    m_calculationModeTitleCells[i].setAlignment(KDGlyph::k_alignRight,
                                                KDGlyph::k_alignCenter);
    m_calculationModeTitleCells[i].setFont(KDFont::Size::Small);
    m_calculationModeSymbolCells[i].setAlignment(KDGlyph::k_alignCenter,
                                                 KDGlyph::k_alignCenter);
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

// TableViewDataSource

int CalculationController::numberOfColumns() const {
  return 2 + m_store->numberOfActiveSeries();
}

void CalculationController::fillCellForLocation(HighlightCell *cell, int column,
                                                int row) {
  EvenOddCell *evenOddCell = static_cast<EvenOddCell *>(cell);
  evenOddCell->setEven(row % 2 == 0);
  int type = typeAtLocation(column, row);
  int numberOfFixedRows = fixedNumberOfRows();
  switch (type) {
    case k_seriesTitleCellType: {
      // Display a series title cell
      int seriesNumber = m_store->seriesIndexFromActiveSeriesIndex(column - 2);
      char titleBuffer[] = {'V', static_cast<char>('1' + seriesNumber), '/',
                            'N', static_cast<char>('1' + seriesNumber), 0};
      BufferFunctionTitleCell *seriesTitleCell =
          static_cast<BufferFunctionTitleCell *>(cell);
      seriesTitleCell->setText(titleBuffer);
      seriesTitleCell->setColor(
          DoublePairStore::colorOfSeriesAtIndex(seriesNumber));
      return;
    }
    case k_calculationModeTitleCellType:
    case k_calculationModeSymbolCellType: {
      assert((column == 0 && type == k_calculationModeTitleCellType) ||
             (column == 1 && type == k_calculationModeSymbolCellType));
      // Mode title and symbol cells have an index value
      const char *pattern = (m_store->totalNumberOfModes() == 1
                                 ? "%s"
                                 : (column == 0 ? "%s %i" : "%s%i"));
      I18n::Message message =
          (column == 0 ? I18n::Message::Mode : I18n::Message::ModeSymbol);
      int index = row - numberOfFixedRows + 1;
      assert(row >= 1);
      // The NL "Modus 100" is the longest possible text here.
      constexpr static int bufferSize = sizeof("Modus 100") / sizeof(char);
      char buffer[bufferSize];
      Poincare::Print::CustomPrintf(buffer, bufferSize, pattern,
                                    I18n::translate(message), index);
      AbstractEvenOddBufferTextCell *bufferCell =
          static_cast<AbstractEvenOddBufferTextCell *>(cell);
      bufferCell->setTextColor(column == 1 ? Palette::GrayDark : KDColorBlack);
      bufferCell->setText(buffer);
      return;
    }
    case k_calculationTitleCellType:
    case k_calculationSymbolCellType: {
      assert(row >= 1 &&
             ((column == 0 && type == k_calculationTitleCellType) ||
              (column == 1 && type == k_calculationSymbolCellType)));
      // Display a calculation title or symbol
      I18n::Message message;
      if (row - 1 < numberOfFixedRows - 1) {
        int messageIndex = findCellIndex(row - 1);
        message = column == 1 ? k_calculationRows[messageIndex].symbol
                              : k_calculationRows[messageIndex].title;
      } else {
        // titles index does not include the Mode Frequency messages
        assert(showModeFrequency() && row == numberOfRows() - 1);
        I18n::Message modeFrequencyTitles[k_numberOfHeaderColumns] = {
            I18n::Message::ModeFrequency, I18n::Message::ModeFrequencySymbol};
        message = modeFrequencyTitles[column];
      }
      EvenOddMessageTextCell *calcTitleCell =
          static_cast<EvenOddMessageTextCell *>(cell);
      calcTitleCell->setTextColor(column == 1 ? Palette::GrayDark
                                              : KDColorBlack);
      calcTitleCell->setMessage(message);
      return;
    }
    case k_calculationCellType: {
      // Display a calculation cell
      int seriesIndex = m_store->seriesIndexFromActiveSeriesIndex(column - 2);
      double calculation;
      AbstractEvenOddBufferTextCell *calculationCell =
          static_cast<AbstractEvenOddBufferTextCell *>(cell);
      if (row - 1 < (numberOfFixedRows - 1)) {
        int calculationIndex = findCellIndex(row - 1);
        if (std::isnan(m_memoizedCellContent[seriesIndex][calculationIndex])) {
          m_memoizedCellContent[seriesIndex][calculationIndex] =
              (m_store->*k_calculationRows[calculationIndex].calculationMethod)(
                  seriesIndex);
        }
        calculation = m_memoizedCellContent[seriesIndex][calculationIndex];
        assert(calculation == (m_store->*k_calculationRows[calculationIndex]
                                             .calculationMethod)(seriesIndex) ||
               (std::isnan(calculation) &&
                std::isnan((m_store->*k_calculationRows[calculationIndex]
                                          .calculationMethod)(seriesIndex))));
      } else if (showModeFrequency() && row == numberOfRows() - 1) {
        calculation = m_store->modeFrequency(seriesIndex);
      } else {
        int modeIndex = row - numberOfFixedRows;
        if (modeIndex < m_store->numberOfModes(seriesIndex)) {
          calculation = m_store->modeAtIndex(seriesIndex, modeIndex);
        } else {
          calculationCell->setText("-");
          return;
        }
      }
      constexpr int bufferSize = PrintFloat::charSizeForFloatsWithPrecision(
          Escher::AbstractEvenOddBufferTextCell::k_defaultPrecision);
      char buffer[bufferSize];
      PoincareHelpers::ConvertFloatToText<double>(
          calculation, buffer, bufferSize,
          Escher::AbstractEvenOddBufferTextCell::k_defaultPrecision);
      calculationCell->setText(buffer);
      return;
    }
  }
}

KDCoordinate CalculationController::nonMemoizedColumnWidth(int column) {
  if (column == 0) {
    return k_calculationTitleCellWidth;
  }
  if (column == 1) {
    int numberOfModes = m_store->totalNumberOfModes();
    static_assert(Store::k_maxNumberOfPairs < 1000,
                  "numberOfChars must be updated");
    // Mod1, Mod10 and Mod100
    int numberOfChars = (numberOfModes < 10      ? 4
                         : (numberOfModes < 100) ? 5
                                                 : 6);
    return CalculationSymbolCellWidth(numberOfChars);
  }
  return k_calculationCellWidth;
}

HighlightCell *CalculationController::reusableCell(int index, int type) {
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

int CalculationController::typeAtLocation(int column, int row) {
  assert(column >= 0 && column < numberOfColumns());
  assert(row >= 0 && row < numberOfRows());
  if (column <= 1 && row == 0) {
    return k_hideableCellType;
  }
  if (column <= 1) {
    assert(row > 0);
    if (row < fixedNumberOfRows() ||
        (showModeFrequency() && row == numberOfRows() - 1)) {
      return column == 0 ? k_calculationTitleCellType
                         : k_calculationSymbolCellType;
    }
    return column == 0 ? k_calculationModeTitleCellType
                       : k_calculationModeSymbolCellType;
  }
  if (row == 0) {
    return k_seriesTitleCellType;
  }
  return k_calculationCellType;
}

KDCoordinate CalculationController::separatorBeforeColumn(int column) {
  return typeAtLocation(column, 0) == k_seriesTitleCellType
             ? Escher::Metric::TableSeparatorThickness
             : 0;
}

int CalculationController::findCellIndex(int i) const {
  int returnIndex = 0;
  while (returnIndex <= fixedNumberOfRows()) {
    if (i == (GlobalPreferences::sharedGlobalPreferences->statsRowsLayout() ==
                      CountryPreferences::StatsRowsLayout::Variant1
                  ? k_calculationRows[returnIndex].variant1Index
                  : k_calculationRows[returnIndex].defaultIndex)) {
      return returnIndex;
    }
    returnIndex++;
  }
  assert(false);
  return 0;
}

int CalculationController::fixedNumberOfRows() const {
  // Hide SampleMean under default StatsRowLayout
  return GlobalPreferences::sharedGlobalPreferences->statsRowsLayout() ==
                 CountryPreferences::StatsRowsLayout::Variant1
             ? k_fixedMaxNumberOfRows
             : k_fixedMaxNumberOfRows - 1;
}

void CalculationController::resetMemoization(bool force) {
  DoublePairTableController::resetMemoization(force);
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    for (int j = 0; j < k_numberOfCalculations; j++) {
      m_memoizedCellContent[i][j] = NAN;
    }
  }
}

}  // namespace Statistics
