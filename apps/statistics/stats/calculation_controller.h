#ifndef STATISTICS_CALCULATION_CONTROLLER_H
#define STATISTICS_CALCULATION_CONTROLLER_H

#include <apps/shared/buffer_function_title_cell.h>
#include <apps/shared/double_pair_table_controller.h>
#include <escher/even_odd_buffer_text_cell.h>
#include <poincare/old/preferences.h>
#include <poincare/print_float.h>

#include <array>

#include "../store.h"
#include "calculation_selectable_table_view.h"

namespace Statistics {

class CalculationController : public Shared::DoublePairTableController {
 public:
  CalculationController(Escher::Responder* parentResponder,
                        Escher::ButtonRowController* header, Store* store);

  // ViewController
  TELEMETRY_ID("Calculation");

  // TableViewDataSource
  int numberOfRows() const override {
    return fixedNumberOfRows() + m_store->totalNumberOfModes() +
           showModeFrequency();
  }
  void fillCellForLocation(Escher::HighlightCell* cell, int column,
                           int row) override;
  Escher::HighlightCell* reusableCell(int index, int type) override;
  int reusableCellCount(int type) const override;
  int typeAtLocation(int column, int row) const override;

  bool showModeFrequency() const { return m_store->totalNumberOfModes() > 0; }

 private:
  // Number of cells
  constexpr static int k_fixedMaxNumberOfRows = 17;
  // Cell types
  constexpr static int k_calculationModeTitleCellType = 5;
  constexpr static int k_calculationModeSymbolCellType = 6;
  // Cell sizes
  /* Margins from EvenOddCell::layoutSubviews (and derived classes
   * implementations) must be accounted for here. */
  constexpr static KDCoordinate k_calculationCellWidth =
      Escher::Metric::SmallFontCellWidth(
          Poincare::PrintFloat::glyphLengthForFloatWithPrecision(
              Escher::AbstractEvenOddBufferTextCell::k_defaultPrecision),
          Escher::EvenOddCell::k_horizontalMargin);

  struct CalculationRow {
    I18n::Message title;
    I18n::Message symbol;
    Store::CalculPointer calculationMethod;
    uint8_t defaultIndex;
    uint8_t variant1Index;
  };

  constexpr static CalculationRow k_calculationRows[] = {
      // Message, symbol, function, defaultLayout, variant1Layout
      {I18n::Message::TotalFrequency, I18n::Message::N,
       &Store::sumOfOccurrences, 0, 0},
      {I18n::Message::Minimum, I18n::Message::MinimumSymbol, &Store::minValue,
       1, 1},
      {I18n::Message::Maximum, I18n::Message::MaximumSymbol, &Store::maxValue,
       2, 5},
      {I18n::Message::Range, I18n::Message::RangeSymbol, &Store::range, 3, 6},
      {I18n::Message::Mean, I18n::Message::MeanSymbol, &Store::mean, 4,
       16},  // Not displayed under variant1Layout
      {I18n::Message::StandardDeviation, I18n::Message::Sigma,
       &Store::standardDeviation, 5, 9},
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
      {I18n::Message::SampleStandardDeviationS, I18n::Message::S,
       &Store::sampleStandardDeviation, 13, 12},
      {I18n::Message::SampleVariance, I18n::Message::SampleVarianceSymbol,
       &Store::sampleVariance, 14, 13},
      {I18n::Message::Mean, I18n::Message::MuMeanSymbol, &Store::mean, 15,
       8},  // Not displayed with defaultLayout
      {I18n::Message::StatisticsSampleMean, I18n::Message::MeanSymbol,
       &Store::mean, 16, 11},  // Not displayed with defaultLayout
  };
  constexpr static int k_numberOfCalculations = std::size(k_calculationRows);

  // TableViewDataSource
  KDCoordinate nonMemoizedColumnWidth(int column) override;
  void resetSizeMemoization() override;

  // DoublePairTableController
  Shared::DoublePairStore* store() const override { return m_store; }

  int findCellIndex(int i) const;
  int fixedNumberOfRows() const;

  Shared::BufferFunctionTitleCell
      m_seriesTitleCells[k_numberOfSeriesTitleCells];
  Escher::SmallFontEvenOddBufferTextCell
      m_calculationModeTitleCells[k_maxNumberOfDisplayableRows];
  Escher::SmallFontEvenOddBufferTextCell
      m_calculationModeSymbolCells[k_maxNumberOfDisplayableRows];
  Escher::FloatEvenOddBufferTextCell<>
      m_calculationCells[k_numberOfCalculationCells];
  double m_memoizedCellContent[Store::k_numberOfSeries][k_numberOfCalculations];
  Store* m_store;
};

}  // namespace Statistics

#endif
