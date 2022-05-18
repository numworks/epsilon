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
  m_tableView(1, this, &m_selectableTableView, this),
  m_store(store)
{
  m_tableView.setCellOverlap(0, 0);
  m_tableView.setBackgroundColor(Palette::WallScreenDark);
  m_tableView.setMargins(k_margin, k_scrollBarMargin, k_scrollBarMargin, k_margin);
  m_tableView.setMarginDelegate(this);
  for (int i = 0; i < k_numberOfSeriesTitleCells; i++) {
    m_seriesTitleCells[i].setSeparatorLeft(true);
  }
  for (int i = 0; i < k_numberOfCalculationTitleCells; i++) {
    m_calculationTitleCells[i].setAlignment(KDContext::k_alignRight, KDContext::k_alignCenter);
    m_calculationTitleCells[i].setMessageFont(KDFont::SmallFont);
    m_calculationSymbolCells[i].setAlignment(KDContext::k_alignCenter, KDContext::k_alignCenter);
    m_calculationSymbolCells[i].setMessageFont(KDFont::SmallFont);
    m_calculationModeTitleCells[i].setAlignment(KDContext::k_alignRight, KDContext::k_alignCenter);
    m_calculationModeTitleCells[i].setFont(KDFont::SmallFont);
    m_calculationModeSymbolCells[i].setAlignment(KDContext::k_alignCenter, KDContext::k_alignCenter);
    m_calculationModeSymbolCells[i].setFont(KDFont::SmallFont);
  }
  for (int i = 0; i < k_numberOfHeaderColumns; i++) {
    m_hideableCell[0].setHide(true);
    m_hideableCell[1].setHide(true);
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
    const char * pattern = (i == 0 ? "%s %i" : "%s%i");
    I18n::Message message = (i == 0 ? I18n::Message::Mode : I18n::Message::ModeSymbol);
    int index = j - k_fixedNumberOfRows + 2;
    assert(j >= 1);
    // The NL "Modus 100" is the longest possible text here.
    constexpr static int bufferSize = sizeof("Modus 100") / sizeof(char);
    char buffer[bufferSize];
    Poincare::Print::customPrintf(buffer, bufferSize, pattern, I18n::translate(message), index);
    EvenOddBufferTextCell * bufferCell = static_cast<EvenOddBufferTextCell *>(cell);
    bufferCell->setTextColor(i == 1 ? Palette::GrayDark : KDColorBlack);
    bufferCell->setText(buffer);
    return;
  }
  case k_calculationTitleCellType:
  case k_calculationSymbolCellType: {
    assert(j >= 1 && ((i == 0 && type == k_calculationTitleCellType) || (i == 1 && type == k_calculationSymbolCellType)));
    // Display a calculation title or symbol
    I18n::Message titles[k_fixedNumberOfRows - 2][k_numberOfHeaderColumns] = {
      { I18n::Message::TotalFrequency, I18n::Message::TotalFrequencySymbol },
      { I18n::Message::Minimum, I18n::Message::MinimumSymbol },
      { I18n::Message::Maximum, I18n::Message::MaximumSymbol },
      { I18n::Message::Range, I18n::Message::RangeSymbol },
      { I18n::Message::Mean, I18n::Message::MeanSymbol },
      { I18n::Message::StandardDeviation, I18n::Message::StandardDeviationSigmaSymbol },
      { I18n::Message::Deviation, I18n::Message::DeviationSymbol },
      { I18n::Message::FirstQuartile, I18n::Message::FirstQuartileSymbol },
      { I18n::Message::ThirdQuartile, I18n::Message::ThirdQuartileSymbol },
      { I18n::Message::Median, I18n::Message::MedianSymbol },
      { I18n::Message::InterquartileRange, I18n::Message::InterquartileRangeSymbol },
      { I18n::Message::SumValues, I18n::Message::SumValuesSymbol },
      { I18n::Message::SumSquareValues, I18n::Message::SumSquareValuesSymbol },
      { I18n::Message::SampleStandardDeviationS, I18n::Message::SampleStandardDeviationSSymbol },
      { I18n::Message::SampleVariance, I18n::Message::SampleVarianceSymbol },
    };
    I18n::Message message;
    int messageIndex = j - 1;
    if (messageIndex < k_fixedNumberOfRows - 2) {
      message = titles[messageIndex][i];
    } else {
      // titles index does not include the Mode Frequency messages
      assert(j == numberOfRows() - 1);
      I18n::Message modeFrequencyTitles[k_numberOfHeaderColumns] = { I18n::Message::ModeFrequency, I18n::Message::ModeFrequencySymbol };
      message = modeFrequencyTitles[i];
    }
    EvenOddMessageTextCell * calcTitleCell = static_cast<EvenOddMessageTextCell *>(cell);
    calcTitleCell->setMessage(message, (i == 1 ? Palette::GrayDark : KDColorBlack));
    return;
  }
  case k_calculationCellType: {
    // Display a calculation cell
    Store::CalculPointer calculationMethods[k_fixedNumberOfRows - 2] = {&Store::sumOfOccurrences, &Store::minValue,
      &Store::maxValue, &Store::range, &Store::mean, &Store::standardDeviation, &Store::variance, &Store::firstQuartile,
      &Store::thirdQuartile, &Store::median, &Store::quartileRange, &Store::sum, &Store::squaredValueSum, &Store::sampleStandardDeviation, &Store::sampleVariance};
    int seriesIndex = m_store->indexOfKthValidSeries(i-2);
    double calculation;
    EvenOddBufferTextCell * calculationCell = static_cast<EvenOddBufferTextCell *>(cell);
    if (j - 1 < (k_fixedNumberOfRows - 2)) {
      calculation = (m_store->*calculationMethods[j-1])(seriesIndex);
    } else if (j == numberOfRows() - 1) {
      calculation = m_store->modeFrequency(seriesIndex);
    } else {
      int modeIndex = j - k_fixedNumberOfRows + 1;
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

KDCoordinate CalculationController::columnWidth(int i) {
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
    if (j < k_fixedNumberOfRows - 1 || j == numberOfRows() - 1) {
      return i == 0 ? k_calculationTitleCellType : k_calculationSymbolCellType;
    }
    return i == 0 ? k_calculationModeTitleCellType : k_calculationModeSymbolCellType;
  }
  if (j == 0) {
    return k_seriesTitleCellType;
  }
  return k_calculationCellType;
}

// MarginDelegate

KDCoordinate CalculationController::prefaceMargin(Escher::TableView * preface) {
  KDCoordinate prefaceRightSide = offset().x() + (preface->bounds().isEmpty() ? preface->minimalSizeForOptimalDisplay().width() : 0);

  for (int i = 0; i < numberOfColumns(); i++) {
    constexpr KDCoordinate maxMargin = Escher::Metric::TableSeparatorThickness;
    KDCoordinate delta = prefaceRightSide - cumulatedWidthFromIndex(i);
    if (delta < 0) {
      return maxMargin;
    } else if (delta <= maxMargin) {
      return delta;
    }
  }
  assert(false);
  return 0;
}

}
