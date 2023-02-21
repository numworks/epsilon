#ifndef STATISTICS_CALCULATION_CONTROLLER_H
#define STATISTICS_CALCULATION_CONTROLLER_H

#include <apps/shared/double_pair_table_controller.h>
#include <apps/shared/separator_even_odd_buffer_text_cell.h>
#include <apps/shared/store_title_cell.h>
#include <poincare/preferences.h>
#include <poincare/print_float.h>

#include "../store.h"
#include "calculation_selectable_table_view.h"

namespace Statistics {

class CalculationController : public Shared::DoublePairTableController {
 public:
  CalculationController(Escher::Responder* parentResponder,
                        Escher::ButtonRowController* header, Store* store);

  void viewWillAppear() override;

  // TableViewDataSource
  int numberOfRows() const override {
    return fixedNumberOfRows() + m_store->totalNumberOfModes() +
           showModeFrequency();
  }
  bool showModeFrequency() const { return m_store->totalNumberOfModes() > 0; }
  int numberOfColumns() const override;
  void willDisplayCellAtLocation(Escher::HighlightCell* cell, int i,
                                 int j) override;
  Escher::HighlightCell* reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;

  // ViewController
  TELEMETRY_ID("Calculation");

  // SelectableTableViewDelegate
  void tableViewDidChangeSelection(Escher::SelectableTableView* t,
                                   int previousSelectedCellX,
                                   int previousSelectedCellY,
                                   bool withinTemporarySelection) override;

 private:
  constexpr static int k_fixedMaxNumberOfRows = 17;
  constexpr static int k_numberOfCalculationCells =
      3 * k_maxNumberOfDisplayableRows;
  constexpr static int k_numberOfSeriesTitleCells = 3;
  constexpr static int k_numberOfCalculationTitleCells =
      k_maxNumberOfDisplayableRows;
  constexpr static int k_numberOfHeaderColumns = 2;  // Title & Symbol

  constexpr static int k_calculationTitleCellType = 0;
  constexpr static int k_calculationSymbolCellType = 1;
  constexpr static int k_calculationModeTitleCellType = 2;
  constexpr static int k_calculationModeSymbolCellType = 3;
  constexpr static int k_calculationCellType = 4;
  constexpr static int k_seriesTitleCellType = 5;
  constexpr static int k_hideableCellType = 6;
  constexpr static KDCoordinate k_calculationTitleCellWidth =
      Escher::Metric::SmallFontCellWidth(
          k_titleNumberOfChars, Escher::Metric::CellVerticalElementMargin);
  constexpr static KDCoordinate CalculationSymbolCellWidth(int maxChars) {
    return Escher::Metric::SmallFontCellWidth(
        maxChars, Escher::Metric::CellVerticalElementMargin);
  }
  /* Margins from EvenOddCell::layoutSubviews (and derived classes
   * implementations) must be accounted for here, along with the separator
   * width from SeparatorEvenOddBufferTextCell. */
  constexpr static KDCoordinate k_calculationCellWidth =
      Escher::Metric::SmallFontCellWidth(
          Poincare::PrintFloat::glyphLengthForFloatWithPrecision(
              Poincare::Preferences::VeryLargeNumberOfSignificantDigits),
          Escher::EvenOddCell::k_horizontalMargin) +
      Escher::EvenOddCell::k_separatorWidth;

  typedef struct {
    I18n::Message title;
    I18n::Message symbol;
    Store::CalculPointer calculationMethod;
    uint8_t defaultIndex;
    uint8_t variant1Index;
  } CalculationRow;

  constexpr static CalculationRow k_calculationRows[] = {
      // Message, symbol, function, defaultLayout, variant1Layout
      {I18n::Message::TotalFrequency, I18n::Message::TotalFrequencySymbol,
       &Store::sumOfOccurrences, 0, 0},
      {I18n::Message::Minimum, I18n::Message::MinimumSymbol, &Store::minValue,
       1, 1},
      {I18n::Message::Maximum, I18n::Message::MaximumSymbol, &Store::maxValue,
       2, 5},
      {I18n::Message::Range, I18n::Message::RangeSymbol, &Store::range, 3, 6},
      {I18n::Message::Mean, I18n::Message::MeanSymbol, &Store::mean, 4,
       16},  // Not displayed under variant1Layout
      {I18n::Message::StandardDeviation,
       I18n::Message::StandardDeviationSigmaSymbol, &Store::standardDeviation,
       5, 9},
      {I18n::Message::Deviation, I18n::Message::DeviationSymbol,
       &Store::variance, 6, 10},
      {I18n::Message::FirstQuartile, I18n::Message::FirstQuartileSymbol,
       &Store::firstQuartile, 7, 2},
      {I18n::Message::ThirdQuartile, I18n::Message::ThirdQuartileSymbol,
       &Store::thirdQuartile, 8, 4},
      {I18n::Message::Median, I18n::Message::MedianSymbol, &Store::median, 9,
       3},
      {I18n::Message::InterquartileRange,
       I18n::Message::InterquartileRangeSymbol, &Store::quartileRange, 10, 7},
      {I18n::Message::SumValues, I18n::Message::SumValuesSymbol, &Store::sum,
       11, 14},
      {I18n::Message::SumSquareValues, I18n::Message::SumSquareValuesSymbol,
       &Store::squaredValueSum, 12, 15},
      {I18n::Message::SampleStandardDeviationS,
       I18n::Message::SampleStandardDeviationSSymbol,
       &Store::sampleStandardDeviation, 13, 12},
      {I18n::Message::SampleVariance, I18n::Message::SampleVarianceSymbol,
       &Store::sampleVariance, 14, 13},
      {I18n::Message::Mean, I18n::Message::MuMeanSymbol, &Store::mean, 15,
       8},  // Not displayed with defaultLayout
      {I18n::Message::StatisticsSampleMean, I18n::Message::MeanSymbol,
       &Store::mean, 16, 11},  // Not displayed with defaultLayout
  };
  constexpr static int k_numberOfCalculations =
      sizeof(k_calculationRows) / sizeof(CalculationRow);

  // TableViewDataSource
  KDCoordinate nonMemoizedColumnWidth(int i) override;
  void resetMemoization(bool force = true) override;

  int findCellIndex(int i) const;

  int fixedNumberOfRows() const;
  Shared::DoublePairStore* store() const override { return m_store; }

  Shared::StoreTitleCell m_seriesTitleCells[k_numberOfSeriesTitleCells];
  Escher::EvenOddMessageTextCell
      m_calculationTitleCells[k_numberOfCalculationTitleCells];
  Escher::EvenOddMessageTextCell
      m_calculationSymbolCells[k_numberOfCalculationTitleCells];
  Escher::EvenOddBufferTextCell
      m_calculationModeTitleCells[k_numberOfCalculationTitleCells];
  Escher::EvenOddBufferTextCell
      m_calculationModeSymbolCells[k_numberOfCalculationTitleCells];
  Shared::SeparatorEvenOddBufferTextCell
      m_calculationCells[k_numberOfCalculationCells];
  Escher::EvenOddCell m_hideableCell[k_numberOfHeaderColumns];
  double m_memoizedCellContent[Store::k_numberOfSeries][k_numberOfCalculations];
  Store* m_store;
};

}  // namespace Statistics

#endif
