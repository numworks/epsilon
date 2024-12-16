#ifndef REGRESSION_CALCULATION_CONTROLLER_H
#define REGRESSION_CALCULATION_CONTROLLER_H

#include <apps/global_preferences.h>
#include <apps/shared/double_pair_table_controller.h>
#include <escher/even_odd_buffer_text_cell.h>
#include <escher/even_odd_editable_text_cell.h>
#include <escher/even_odd_expression_cell.h>
#include <poincare/preferences.h>

#include "../store.h"
#include "column_title_cell.h"
#include "even_odd_double_buffer_text_cell.h"

namespace Regression {

class CalculationController : public Shared::DoublePairTableController {
 public:
  CalculationController(Escher::Responder* parentResponder,
                        Escher::ButtonRowController* header, Store* store);

  // TableViewDataSource
  int numberOfRows() const override;
  void fillCellForLocation(Escher::HighlightCell* cell, int column,
                           int row) override;
  Escher::HighlightCell* reusableCell(int index, int type) override;
  int reusableCellCount(int type) const override;
  int typeAtLocation(int column, int row) const override;
  bool canStoreCellAtLocation(int column, int row) override;

  // SelectableTableViewDelegate
  void tableViewDidChangeSelectionAndDidScroll(
      Escher::SelectableTableView* t, int previousSelectedCol,
      int previousSelectedRow, KDPoint previousOffset,
      bool withinTemporarySelection) override;

 private:
  enum class Calculation : uint8_t {
    Mean = 0,
    Sum = 1,
    SquareSum = 2,
    StandardDeviation = 3,
    Deviation = 4,
    SampleStandardDeviationS = 5,
    NumberOfDots = 6,
    Covariance = 7,
    SumOfProducts = 8,
    CorrelationCoeff = 9,
    Regression = 10,
    CoefficientM = 11,
    CoefficientA = 12,
    CoefficientB = 13,
    CoefficientC = 14,
    CoefficientD = 15,
    CoefficientE = 16,
    ResidualStandardDeviation = 17,
    DeterminationCoeff = 18,
    RSquared = 19,
    NumberOfRows = 20,  // Always last
  };
  /* Mean, SumValues, SumSquareValues, StandardDeviationSigma, Deviation,
   * SampleStandardDeviationS */
  constexpr static int k_numberOfDoubleBufferCalculations =
      static_cast<int>(Calculation::SampleStandardDeviationS) + 1;
  // NumberOfDots, Covariance, SumOfProducts
  constexpr static int k_numberOfMemoizedSingleBufferCalculations = 3;
  // Displayable cells
  constexpr static int k_numberOfDoubleCalculationCells =
      k_numberOfSeriesTitleCells * k_numberOfDoubleBufferCalculations;
  // Cell types
  constexpr static int k_doubleBufferCalculationCellType = 5;
  // Cell sizes
  /* Margins from EvenOddCell::layoutSubviews (and derived classes
   * implementations) must be accounted for here.
   * Calculation width should at least be able to hold two numbers with
   * VeryLargeNumberOfSignificantDigits and contains two even odd cells. */
  constexpr static KDCoordinate k_minCalculationCellWidth =
      2 * Escher::Metric::SmallFontCellWidth(
              Poincare::PrintFloat::glyphLengthForFloatWithPrecision(
                  Poincare::Preferences::VeryLargeNumberOfSignificantDigits),
              Escher::EvenOddCell::k_horizontalMargin);
  // To hold _y=a·x^3+b·x^2+c·x+d_
  constexpr static KDCoordinate k_cubicCalculationCellWidth =
      std::max<KDCoordinate>(Escher::Metric::SmallFontCellWidth(
                                 21, Escher::EvenOddCell::k_horizontalMargin),
                             k_minCalculationCellWidth);
  // To hold _y=a·x^4+b·x^3+c·x^2+d·x+e_
  constexpr static KDCoordinate k_quarticCalculationCellWidth =
      std::max<KDCoordinate>(Escher::Metric::SmallFontCellWidth(
                                 27, Escher::EvenOddCell::k_horizontalMargin),
                             k_minCalculationCellWidth);
  // Symbol column
  constexpr static int k_symbolColumnMaxNumberOfChars = 3;
  constexpr static KDCoordinate k_symbolColumnWidth =
      Escher::Metric::SmallFontCellWidth(
          k_symbolColumnMaxNumberOfChars,
          Escher::Metric::CellVerticalElementMargin);

  // TableViewDataSource
  KDCoordinate nonMemoizedColumnWidth(int column) override;
  void resetSizeMemoization() override;

  // DoublePairTableController
  Shared::DoublePairStore* store() const override { return m_store; }

  static I18n::Message MessageForCalculation(Calculation c);
  static I18n::Message SymbolForCalculation(Calculation c);
  Calculation calculationForRow(int row) const;
  int numberOfDisplayedCoefficients() const;
  int numberOfDisplayedBCDECoefficients() const;

  ColumnTitleCell m_seriesTitleCells[k_numberOfSeriesTitleCells];
  EvenOddDoubleBufferTextCell
      m_doubleCalculationCells[k_numberOfDoubleCalculationCells];
  Escher::SmallFontEvenOddBufferTextCell
      m_calculationCells[k_numberOfCalculationCells];
  Store* m_store;
  double m_memoizedDoubleCalculationCells[Store::k_numberOfSeries][2]
                                         [k_numberOfDoubleBufferCalculations];
  double m_memoizedSimpleCalculationCells
      [Store::k_numberOfSeries][k_numberOfMemoizedSingleBufferCalculations];
};

}  // namespace Regression

#endif
