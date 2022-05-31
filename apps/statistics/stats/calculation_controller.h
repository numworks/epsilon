#ifndef STATISTICS_CALCULATION_CONTROLLER_H
#define STATISTICS_CALCULATION_CONTROLLER_H

#include <poincare/print_float.h>
#include <poincare/preferences.h>
#include "../store.h"
#include "calculation_selectable_table_view.h"
#include <apps/shared/hideable_even_odd_cell.h>
#include <apps/shared/separator_even_odd_buffer_text_cell.h>
#include <apps/shared/store_title_cell.h>
#include <apps/shared/double_pair_table_controller.h>

namespace Statistics {

class CalculationController : public Shared::DoublePairTableController {

public:
  CalculationController(Escher::Responder * parentResponder, Escher::ButtonRowController * header, Store * store);

  // TableViewDataSource
  int numberOfRows() const override { return k_fixedNumberOfRows + m_store->totalNumberOfModes() + showModeFrequency(); }
  bool showModeFrequency() const { return m_store->totalNumberOfModes() > 0; }
  int numberOfColumns() const override;
  void willDisplayCellAtLocation(Escher::HighlightCell * cell, int i, int j) override;
  KDCoordinate columnWidth(int i) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;

  // ViewController
  TELEMETRY_ID("Calculation");

  // SelectableTableViewDelegate
  void tableViewDidChangeSelection(Escher::SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) override;

private:
  static constexpr int k_fixedNumberOfRows = 16;
  static constexpr int k_numberOfCalculationCells = 3 * k_maxNumberOfDisplayableRows;
  static constexpr int k_numberOfSeriesTitleCells = 3;
  static constexpr int k_numberOfCalculationTitleCells = k_maxNumberOfDisplayableRows;
  static constexpr int k_numberOfHeaderColumns = 2; // Title & Symbol

  static constexpr int k_calculationTitleCellType = 0;
  static constexpr int k_calculationSymbolCellType = 1;
  static constexpr int k_calculationModeTitleCellType = 2;
  static constexpr int k_calculationModeSymbolCellType = 3;
  static constexpr int k_calculationCellType = 4;
  static constexpr int k_seriesTitleCellType = 5;
  static constexpr int k_hideableCellType = 6;
  static constexpr KDCoordinate k_calculationTitleCellWidth = Escher::Metric::SmallFontCellWidth(k_titleNumberOfChars, Escher::Metric::CellVerticalElementMargin);
  static constexpr KDCoordinate CalculationSymbolCellWidth(int maxChars) { return Escher::Metric::SmallFontCellWidth(maxChars, Escher::Metric::CellVerticalElementMargin); }
  /* Margins from EvenOddCell::layoutSubviews (and derived classes
   * implementations) must be accounted for here, along with the separator
   * width from SeparatorEvenOddBufferTextCell. */
  static constexpr KDCoordinate k_calculationCellWidth = Escher::Metric::SmallFontCellWidth(Poincare::PrintFloat::glyphLengthForFloatWithPrecision(Poincare::Preferences::VeryLargeNumberOfSignificantDigits), Escher::EvenOddCell::k_horizontalMargin) + Escher::EvenOddCell::k_separatorWidth;

  typedef struct {
    I18n::Message title;
    I18n::Message symbol;
    Store::CalculPointer calculationMethod;
    uint8_t defaultIndex;
    uint8_t variant1Index;
  } CalculationRow;

  constexpr static CalculationRow k_calculationRows[] = {
    // Message, symbol, function, defaultLayout, variant1Layout
    { I18n::Message::TotalFrequency, I18n::Message::TotalFrequencySymbol, &Store::sumOfOccurrences, 0, 0 },
    { I18n::Message::Minimum, I18n::Message::MinimumSymbol, &Store::minValue, 1, 1 },
    { I18n::Message::Maximum, I18n::Message::MaximumSymbol, &Store::maxValue, 2, 5 },
    { I18n::Message::Range, I18n::Message::RangeSymbol, &Store::range, 3, 6 },
    { I18n::Message::Mean, I18n::Message::MeanSymbol, &Store::mean, 4, 8},
    { I18n::Message::StandardDeviation, I18n::Message::StandardDeviationSigmaSymbol, &Store::standardDeviation, 5, 9 },
    { I18n::Message::Deviation, I18n::Message::DeviationSymbol, &Store::variance, 6, 10 },
    { I18n::Message::FirstQuartile, I18n::Message::FirstQuartileSymbol, &Store::firstQuartile, 7, 2 },
    { I18n::Message::ThirdQuartile, I18n::Message::ThirdQuartileSymbol, &Store::thirdQuartile, 8, 4 },
    { I18n::Message::Median, I18n::Message::MedianSymbol, &Store::median, 9, 3 },
    { I18n::Message::InterquartileRange, I18n::Message::InterquartileRangeSymbol, &Store::quartileRange, 10, 7 },
    { I18n::Message::SumValues, I18n::Message::SumValuesSymbol, &Store::sum, 11, 13 },
    { I18n::Message::SumSquareValues, I18n::Message::SumSquareValuesSymbol, &Store::squaredValueSum, 12, 14 },
    { I18n::Message::SampleStandardDeviationS, I18n::Message::SampleStandardDeviationSSymbol, &Store::sampleStandardDeviation, 13, 11 },
    { I18n::Message::SampleVariance, I18n::Message::SampleVarianceSymbol, &Store::sampleVariance, 14, 12 },
  };
  int findCellIndex(int i) const;

  Shared::DoublePairStore * store() const override { return m_store; }

  Shared::StoreTitleCell m_seriesTitleCells[k_numberOfSeriesTitleCells];
  Escher::EvenOddMessageTextCell m_calculationTitleCells[k_numberOfCalculationTitleCells];
  Escher::EvenOddMessageTextCell m_calculationSymbolCells[k_numberOfCalculationTitleCells];
  Escher::EvenOddBufferTextCell m_calculationModeTitleCells[k_numberOfCalculationTitleCells];
  Escher::EvenOddBufferTextCell m_calculationModeSymbolCells[k_numberOfCalculationTitleCells];
  Shared::SeparatorEvenOddBufferTextCell m_calculationCells[k_numberOfCalculationCells];
  Shared::HideableEvenOddCell m_hideableCell[k_numberOfHeaderColumns];
  Store * m_store;
};

}


#endif
