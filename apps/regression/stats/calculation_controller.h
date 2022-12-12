#ifndef REGRESSION_CALCULATION_CONTROLLER_H
#define REGRESSION_CALCULATION_CONTROLLER_H

#include <escher/even_odd_expression_cell.h>
#include <escher/even_odd_message_text_cell.h>
#include <poincare/preferences.h>
#include "../store.h"
#include "column_title_cell.h"
#include "even_odd_double_buffer_text_cell_with_separator.h"
#include <apps/shared/separator_even_odd_buffer_text_cell.h>
#include <apps/shared/store_cell.h>
#include <apps/shared/double_pair_table_controller.h>

namespace Regression {

class CalculationController : public Shared::DoublePairTableController {

public:
  CalculationController(Escher::Responder * parentResponder, Escher::ButtonRowController * header, Store * store);

  // View Controller
  TELEMETRY_ID("Calculation");

  void viewWillAppear() override;

  // SelectableTableViewDelegate
  void tableViewDidChangeSelection(Escher::SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) override;
  void tableViewDidChangeSelectionAndDidScroll(Escher::SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) override;

  // TableViewDataSource
  int numberOfRows() const override;
  int numberOfColumns() const override;
  void willDisplayCellAtLocation(Escher::HighlightCell * cell, int i, int j) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;

private:
  // Mean, SumValues, SumSquareValues, StandardDeviationSigma, Deviation, SampleStandardDeviationS
  constexpr static int k_numberOfDoubleBufferCalculations = 6;
  // NumberOfDots, Covariance, SumOfProducts
  constexpr static int k_numberOfSingleBufferCalculations = 3;
  constexpr static int k_numberOfBufferCalculations = k_numberOfDoubleBufferCalculations + k_numberOfSingleBufferCalculations;
  // M, A, B, C, D ,E
  constexpr static int k_maxNumberOfDistinctCoefficients = 6;
  // BufferCalculations + Regression + coefficients + R + R2
  constexpr static int k_maxNumberOfRows = k_numberOfBufferCalculations + 1 + k_maxNumberOfDistinctCoefficients + 2;
  // Displayable cells
  constexpr static int k_numberOfDoubleCalculationCells = Store::k_numberOfSeries * k_numberOfDoubleBufferCalculations;
  constexpr static int k_numberOfCalculationCells = Store::k_numberOfSeries * k_maxNumberOfDisplayableRows;
  // Cell types
  constexpr static int k_standardCalculationTitleCellType = 0;
  constexpr static int k_columnTitleCellType = 1;
  constexpr static int k_doubleBufferCalculationCellType = 2;
  constexpr static int k_standardCalculationCellType = 3;
  constexpr static int k_hideableCellType = 4;
  constexpr static int k_symbolCalculationTitleCellType = 5;
  // Title & Symbol
  constexpr static int k_numberOfHeaderColumns = 2;
  // Cell sizes
  constexpr static KDCoordinate k_titleCalculationCellWidth = Escher::Metric::SmallFontCellWidth(k_titleNumberOfChars, Escher::Metric::CellVerticalElementMargin);
  /* Separator and margins from EvenOddCell::layoutSubviews (and derived classes
   * implementations) must be accounted for here.
   * Calculation width should at least be able to hold two numbers with
   * VeryLargeNumberOfSignificantDigits and contains two even odd cells. */
  constexpr static KDCoordinate k_minCalculationCellWidth = 2 * Escher::Metric::SmallFontCellWidth(Poincare::PrintFloat::glyphLengthForFloatWithPrecision(Poincare::Preferences::VeryLargeNumberOfSignificantDigits), Escher::EvenOddCell::k_horizontalMargin) + Escher::EvenOddCell::k_separatorWidth;
  // To hold _y=a·x^3+b·x^2+c·x+d_
  constexpr static KDCoordinate k_cubicCalculationCellWidth = std::max<KDCoordinate>(Escher::Metric::SmallFontCellWidth(21, Escher::EvenOddCell::k_horizontalMargin) + Escher::EvenOddCell::k_separatorWidth, k_minCalculationCellWidth);
  // To hold _y=a·x^4+b·x^3+c·x^2+d·x+e_
  constexpr static KDCoordinate k_quarticCalculationCellWidth = std::max<KDCoordinate>(Escher::Metric::SmallFontCellWidth(27, Escher::EvenOddCell::k_horizontalMargin) + Escher::EvenOddCell::k_separatorWidth, k_minCalculationCellWidth);
  // Symbol column
  constexpr static int k_symbolColumnMaxNumberOfChars = 3;
  constexpr static KDCoordinate k_symbolColumnWidth = Escher::Metric::SmallFontCellWidth(k_symbolColumnMaxNumberOfChars, Escher::Metric::CellVerticalElementMargin);

  // TableViewDataSource
  KDCoordinate nonMemoizedColumnWidth(int i) override;

  Shared::DoublePairStore * store() const override { return m_store; }
  typedef bool (*DisplayCondition)(Model::Type type);
  static bool DisplayR2(Model::Type type) { return type != Model::Type::None && type != Model::Type::Median; }
  static bool DisplayRegression(Model::Type type) { return type != Model::Type::None; }
  static I18n::Message MessageForCalculation(int calculationRow);
  static I18n::Message SymbolForCalculation(int calculationRow);

  // Return a calculation index for a displayed row index
  int getCalculationIndex(int j) const;
  bool hasSeriesDisplaying(DisplayCondition condition) const;
  bool shouldSeriesDisplay(int series, DisplayCondition condition) const;
  int numberOfDisplayedCoefficients() const;
  bool shouldDisplayMCoefficient() const;
  bool shouldDisplayACoefficient() const;
  int numberOfDisplayedBCDECoefficients() const;
  void resetMemoization(bool force = true) override;

  Escher::EvenOddMessageTextCell m_titleCells[k_maxNumberOfDisplayableRows];
  Escher::EvenOddMessageTextCell m_titleSymbolCells[k_maxNumberOfDisplayableRows];
  ColumnTitleCell m_columnTitleCells[Store::k_numberOfSeries];
  EvenOddDoubleBufferTextCellWithSeparator m_doubleCalculationCells[k_numberOfDoubleCalculationCells];
  Shared::SeparatorEvenOddBufferTextCell m_calculationCells[k_numberOfCalculationCells];
  Escher::EvenOddCell m_hideableCell[k_numberOfHeaderColumns];
  Store * m_store;
  double m_memoizedDoubleCalculationCells[Store::k_numberOfSeries][2][k_numberOfDoubleBufferCalculations];
  double m_memoizedSimpleCalculationCells[Store::k_numberOfSeries][k_numberOfBufferCalculations-k_numberOfDoubleBufferCalculations];
};

}

#endif
