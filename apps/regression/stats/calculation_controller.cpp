#include "calculation_controller.h"

#include <apps/apps_container_helper.h>
#include <apps/shared/editable_cell_table_view_controller.h>
#include <apps/shared/poincare_helpers.h>
#include <assert.h>
#include <poincare/code_point_layout.h>
#include <poincare/helpers.h>
#include <poincare/preferences.h>
#include <poincare/vertical_offset_layout.h>

#include <algorithm>

using namespace Poincare;
using namespace Shared;
using namespace Escher;

namespace Regression {

CalculationController::CalculationController(Responder *parentResponder,
                                             ButtonRowController *header,
                                             Store *store)
    : DoublePairTableController(parentResponder, header), m_store(store) {
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    m_columnTitleCells[i].setParentResponder(&m_selectableTableView);
  }
  for (int i = 0; i < k_numberOfDoubleCalculationCells; i++) {
    m_doubleCalculationCells[i].setParentResponder(&m_selectableTableView);
  }
  for (int i = 0; i < k_maxNumberOfDisplayableRows; i++) {
    m_titleCells[i].setAlignment(KDContext::k_alignRight,
                                 KDContext::k_alignCenter);
    m_titleCells[i].setMessageFont(KDFont::Size::Small);
    m_titleSymbolCells[i].setAlignment(KDContext::k_alignCenter,
                                       KDContext::k_alignCenter);
    m_titleSymbolCells[i].setMessageFont(KDFont::Size::Small);
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

void CalculationController::tableViewDidChangeSelection(
    SelectableTableView *t, int previousSelectedCellX,
    int previousSelectedCellY, bool withinTemporarySelection) {
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
      tabController()->selectTab();
    } else {
      t->selectCellAtLocation(selectedColumn(), 1);
    }
  }
}

void CalculationController::tableViewDidChangeSelectionAndDidScroll(
    SelectableTableView *t, int previousSelectedCellX,
    int previousSelectedCellY, bool withinTemporarySelection) {
  if (withinTemporarySelection) {
    return;
  }
  if (t->selectedColumn() > 1 && t->selectedRow() >= 0 &&
      t->selectedRow() <= k_numberOfDoubleBufferCalculations) {
    /* If we are on a double text cell, we have to choose which subcell to
     * select It has to be done after the scroll for the selectedCell to be
     * defined */
    EvenOddDoubleBufferTextCellWithSeparator *myCell =
        (EvenOddDoubleBufferTextCellWithSeparator *)t->selectedCell();
    // Default selected subcell is the left one
    bool firstSubCellSelected = true;
    if (previousSelectedCellX > 1 && previousSelectedCellY >= 0 &&
        previousSelectedCellY <= k_numberOfDoubleBufferCalculations) {
      // If we come from another double text cell, we have to update
      // subselection
      EvenOddDoubleBufferTextCellWithSeparator *myPreviousCell =
          (EvenOddDoubleBufferTextCellWithSeparator *)t->cellAtLocation(
              previousSelectedCellX, previousSelectedCellY);
      /* If the selection stays in the same column, we copy the subselection
       * from previous cell. Otherwise, the selection has jumped to another
       * column, we thus subselect the other subcell. */
      assert(myPreviousCell);
      firstSubCellSelected = t->selectedColumn() == previousSelectedCellX
                                 ? myPreviousCell->firstTextSelected()
                                 : !myPreviousCell->firstTextSelected();
    }
    myCell->selectFirstText(firstSubCellSelected);
  }
}

bool CalculationController::canStoreContentOfCellAtLocation(
    Escher::SelectableTableView *t, int col, int row) const {
  if (!Shared::DoublePairTableController::canStoreContentOfCellAtLocation(
          t, col, row)) {
    return false;
  }
  assert(row > 0 && col > 1);
  const int calculationIndex = getCalculationIndex(row);
  if (calculationIndex == k_numberOfBufferCalculations) {
    // Regression formula
    return false;
  }
  if (calculationIndex > k_numberOfBufferCalculations) {
    SeparatorEvenOddBufferTextCell *bufferCell =
        static_cast<SeparatorEvenOddBufferTextCell *>(
            t->cellAtLocation(col, row));
    return strcmp(bufferCell->text(), I18n::translate(I18n::Message::Dash)) &&
           strcmp(bufferCell->text(), I18n::translate(I18n::Message::Disabled));
  }
  return true;
}

int CalculationController::numberOfRows() const {
  /* Rows for : title + Mean ... Variance + Number of points + Covariance + ∑xy
   * + r + (Regression) + Coefficients + (R2) */
  return 1 + k_numberOfDoubleBufferCalculations +
         k_numberOfSingleBufferCalculations + 1 +
         hasSeriesDisplaying(&DisplayRegression) +
         numberOfDisplayedCoefficients() + hasSeriesDisplaying(&DisplayR2);
}

int CalculationController::numberOfColumns() const {
  return 2 + m_store->numberOfActiveSeries();
}

void CalculationController::willDisplayCellAtLocation(HighlightCell *cell,
                                                      int i, int j) {
  if (i <= 1 && j == 0) {
    return;
  }
  EvenOddCell *myCell = static_cast<EvenOddCell *>(cell);
  myCell->setEven(j % 2 == 0);

  // Coordinate and series title
  if (j == 0 && i > 1) {
    ColumnTitleCell *myCell = static_cast<ColumnTitleCell *>(cell);
    size_t seriesNumber =
        m_store->seriesIndexFromActiveSeriesIndex(i - k_numberOfHeaderColumns);
    assert(seriesNumber < DoublePairStore::k_numberOfSeries);
    char buffer[Shared::ClearColumnHelper::k_maxSizeOfColumnName];
    m_store->fillColumnName(seriesNumber, 0, buffer);
    myCell->setFirstText(buffer);
    m_store->fillColumnName(seriesNumber, 1, buffer);
    myCell->setSecondText(buffer);
    assert(seriesNumber < Palette::numberOfDataColors());
    myCell->setColor(Palette::DataColor[seriesNumber]);
    return;
  }

  const int calculationIndex = getCalculationIndex(j);
  // Calculation title and symbols
  if (i <= 1) {
    EvenOddMessageTextCell *myCell =
        static_cast<EvenOddMessageTextCell *>(cell);
    myCell->setTextColor(i == 0 ? KDColorBlack : Palette::GrayDark);
    I18n::Message message = (i == 0) ? MessageForCalculation(calculationIndex)
                                     : SymbolForCalculation(calculationIndex);
    myCell->setMessage(message);
    if (calculationIndex >= k_numberOfBufferCalculations + 1 +
                                k_maxNumberOfDistinctCoefficients &&
        Preferences::sharedPreferences->examMode().forbidStatsDiagnostics()) {
      // R and R2 messages should be grayed out.
      myCell->setTextColor(Palette::GrayDark);
    }
    return;
  }

  size_t seriesNumber =
      m_store->seriesIndexFromActiveSeriesIndex(i - k_numberOfHeaderColumns);
  assert(seriesNumber < DoublePairStore::k_numberOfSeries);

  assert(i > 1 && j > 0);
  // Calculation cell
  const int numberSignificantDigits =
      Preferences::VeryLargeNumberOfSignificantDigits;
  if (calculationIndex < k_numberOfDoubleBufferCalculations) {
    ArgCalculPointer calculationMethods[k_numberOfDoubleBufferCalculations] = {
        &Store::meanOfColumn,
        &Store::sumOfColumn,
        &Store::squaredValueSumOfColumn,
        &Store::standardDeviationOfColumn,
        &Store::varianceOfColumn,
        &Store::sampleStandardDeviationOfColumn,
    };
    double *calculation1 =
        m_memoizedDoubleCalculationCells[seriesNumber][0] + calculationIndex;
    double *calculation2 =
        m_memoizedDoubleCalculationCells[seriesNumber][1] + calculationIndex;
    if (std::isnan(*calculation1) || std::isnan(*calculation2)) {
      *calculation1 = (m_store->*calculationMethods[calculationIndex])(
          seriesNumber, 0, false);
      *calculation2 = (m_store->*calculationMethods[calculationIndex])(
          seriesNumber, 1, false);
    }
    assert(Poincare::Helpers::EqualOrBothNan(
               *calculation1, (m_store->*calculationMethods[calculationIndex])(
                                  seriesNumber, 0, false)) &&
           Poincare::Helpers::EqualOrBothNan(
               *calculation2, (m_store->*calculationMethods[calculationIndex])(
                                  seriesNumber, 1, false)));
    EvenOddDoubleBufferTextCellWithSeparator *myCell =
        static_cast<EvenOddDoubleBufferTextCellWithSeparator *>(cell);
    constexpr int bufferSize =
        PrintFloat::charSizeForFloatsWithPrecision(numberSignificantDigits);
    char buffer[bufferSize];
    PoincareHelpers::ConvertFloatToText<double>(
        *calculation1, buffer, bufferSize, numberSignificantDigits);
    myCell->setFirstText(buffer);
    PoincareHelpers::ConvertFloatToText<double>(
        *calculation2, buffer, bufferSize, numberSignificantDigits);
    myCell->setSecondText(buffer);
    return;
  }
  SeparatorEvenOddBufferTextCell *bufferCell =
      static_cast<SeparatorEvenOddBufferTextCell *>(cell);
  bufferCell->setTextColor(KDColorBlack);
  if (calculationIndex < k_numberOfBufferCalculations) {
    const int singleCalculationIndex =
        calculationIndex - k_numberOfDoubleBufferCalculations;
    double *calculation =
        m_memoizedSimpleCalculationCells[seriesNumber] + singleCalculationIndex;
    if (std::isnan(*calculation)) {
      if (singleCalculationIndex == 0) {
        *calculation = m_store->doubleCastedNumberOfPairsOfSeries(seriesNumber);
      } else if (singleCalculationIndex == 1) {
        *calculation = m_store->covariance(seriesNumber);
      } else {
        assert(singleCalculationIndex == 2);
        *calculation = m_store->columnProductSum(seriesNumber);
      }
    }
    assert((singleCalculationIndex == 0 &&
            Poincare::Helpers::EqualOrBothNan(
                *calculation,
                m_store->doubleCastedNumberOfPairsOfSeries(seriesNumber))) ||
           (singleCalculationIndex == 1 &&
            Poincare::Helpers::EqualOrBothNan(
                *calculation, m_store->covariance(seriesNumber))) ||
           (singleCalculationIndex == 2 &&
            Poincare::Helpers::EqualOrBothNan(
                *calculation, m_store->columnProductSum(seriesNumber))));
    constexpr int bufferSize =
        PrintFloat::charSizeForFloatsWithPrecision(numberSignificantDigits);
    char buffer[bufferSize];
    PoincareHelpers::ConvertFloatToText<double>(
        *calculation, buffer, bufferSize, numberSignificantDigits);
    bufferCell->setText(buffer);
    return;
  }
  if (calculationIndex == k_numberOfBufferCalculations) {
    Model *model = m_store->modelForSeries(seriesNumber);
    I18n::Message message =
        shouldSeriesDisplay(seriesNumber, &DisplayRegression)
            ? model->formulaMessage()
            : I18n::Message::Dash;
    bufferCell->setText(I18n::translate(message));
    return;
  }
  Poincare::Context *globContext =
      AppsContainerHelper::sharedAppsContainerGlobalContext();
  if (calculationIndex <
      k_numberOfBufferCalculations + 1 + k_maxNumberOfDistinctCoefficients) {
    if (!m_store->coefficientsAreDefined(seriesNumber, globContext)) {
      // Put dashes if regression is not defined
      bufferCell->setText(I18n::translate(I18n::Message::Dash));
      return;
    }
    int coefficientIndex =
        calculationIndex - (k_numberOfBufferCalculations + 1) - 1;
    int numberOfCoefficients =
        m_store->modelForSeries(seriesNumber)->numberOfCoefficients();
    if (shouldDisplayMCoefficient() &&
        m_store->seriesRegressionType(seriesNumber) ==
            Model::Type::LinearAxpb &&
        coefficientIndex <= 0) {
      // In that case only, M is coefficientIndex 0 and A is coefficientIndex -1
      coefficientIndex = -coefficientIndex - 1;
    }
    if (coefficientIndex < 0 || coefficientIndex >= numberOfCoefficients) {
      bufferCell->setText(I18n::translate(I18n::Message::Dash));
      return;
    }
    double *coefficients =
        m_store->coefficientsForSeries(seriesNumber, globContext);
    constexpr int bufferSize =
        PrintFloat::charSizeForFloatsWithPrecision(numberSignificantDigits);
    char buffer[bufferSize];
    PoincareHelpers::ConvertFloatToText<double>(coefficients[coefficientIndex],
                                                buffer, bufferSize,
                                                numberSignificantDigits);
    bufferCell->setText(buffer);
    return;
  }
  // Calculation is either R or R2
  assert(calculationIndex >=
         k_numberOfBufferCalculations + 1 + k_maxNumberOfDistinctCoefficients);
  if (Preferences::sharedPreferences->examMode().forbidStatsDiagnostics()) {
    bufferCell->setTextColor(Palette::GrayDark);
    bufferCell->setText(I18n::translate(I18n::Message::Disabled));
    return;
  }
  bool calculationIsR = (calculationIndex == k_maxNumberOfRows - 2);
  if (!calculationIsR && !shouldSeriesDisplay(seriesNumber, DisplayR2)) {
    bufferCell->setText(I18n::translate(I18n::Message::Dash));
    return;
  }
  double calculation = calculationIsR
                           ? m_store->correlationCoefficient(seriesNumber)
                           : m_store->determinationCoefficientForSeries(
                                 seriesNumber, globContext);
  constexpr int bufferSize =
      PrintFloat::charSizeForFloatsWithPrecision(numberSignificantDigits);
  char buffer[bufferSize];
  PoincareHelpers::ConvertFloatToText<double>(calculation, buffer, bufferSize,
                                              numberSignificantDigits);
  bufferCell->setText(buffer);
}

KDCoordinate CalculationController::nonMemoizedColumnWidth(int i) {
  if (i == 0) {
    return k_titleCalculationCellWidth;
  }
  if (i == 1) {
    return k_symbolColumnWidth;
  }
  Model::Type currentType = m_store->seriesRegressionType(
      m_store->seriesIndexFromActiveSeriesIndex(i - k_numberOfHeaderColumns));
  if (currentType == Model::Type::Quartic) {
    return k_quarticCalculationCellWidth;
  }
  if (currentType == Model::Type::Cubic) {
    return k_cubicCalculationCellWidth;
  }
  return k_minCalculationCellWidth;
}

HighlightCell *CalculationController::reusableCell(int index, int type) {
  if (type == k_standardCalculationTitleCellType) {
    assert(index >= 0 && index < k_maxNumberOfDisplayableRows);
    return &m_titleCells[index];
  }
  if (type == k_symbolCalculationTitleCellType) {
    assert(index >= 0 && index < k_maxNumberOfDisplayableRows);
    return &m_titleSymbolCells[index];
  }
  if (type == k_columnTitleCellType) {
    assert(index >= 0 && index < Store::k_numberOfSeries);
    return &m_columnTitleCells[index];
  }
  if (type == k_doubleBufferCalculationCellType) {
    assert(index >= 0 && index < k_numberOfDoubleCalculationCells);
    return &m_doubleCalculationCells[index];
  }
  if (type == k_hideableCellType) {
    assert(index >= 0 && index < k_numberOfHeaderColumns);
    return &m_hideableCell[index];
  }
  assert(index >= 0 && index < k_numberOfCalculationCells);
  return &m_calculationCells[index];
}

int CalculationController::reusableCellCount(int type) {
  if (type == k_standardCalculationTitleCellType ||
      type == k_symbolCalculationTitleCellType) {
    return k_maxNumberOfDisplayableRows;
  }
  if (type == k_columnTitleCellType) {
    return Store::k_numberOfSeries;
  }
  if (type == k_doubleBufferCalculationCellType) {
    return k_numberOfDoubleCalculationCells;
  }
  if (type == k_hideableCellType) {
    return 2;
  }
  assert(type == k_standardCalculationCellType);
  return k_numberOfCalculationCells;
}

int CalculationController::typeAtLocation(int i, int j) {
  if (i <= 1 && j == 0) {
    return k_hideableCellType;
  }
  if (i == 0) {
    return k_standardCalculationTitleCellType;
  }
  if (i == 1) {
    return k_symbolCalculationTitleCellType;
  }
  if (j == 0) {
    return k_columnTitleCellType;
  }
  if (j > 0 && j <= k_numberOfDoubleBufferCalculations) {
    return k_doubleBufferCalculationCellType;
  }
  return k_standardCalculationCellType;
}

I18n::Message CalculationController::MessageForCalculation(
    int calculationIndex) {
  I18n::Message messages[k_maxNumberOfRows] = {
      I18n::Message::Mean,
      I18n::Message::Sum,
      I18n::Message::SquareSum,
      I18n::Message::StandardDeviation,
      I18n::Message::Deviation,
      I18n::Message::SampleStandardDeviationS,
      I18n::Message::NumberOfDots,
      I18n::Message::Covariance,
      I18n::Message::SumOfProducts,
      I18n::Message::Regression,
      I18n::Message::CoefficientM,
      I18n::Message::CoefficientA,
      I18n::Message::CoefficientB,
      I18n::Message::CoefficientC,
      I18n::Message::CoefficientD,
      I18n::Message::CoefficientE,
      I18n::Message::CorrelationCoeff,
      I18n::Message::DeterminationCoeff};
  return messages[calculationIndex];
}

I18n::Message CalculationController::SymbolForCalculation(
    int calculationIndex) {
  I18n::Message messages[k_maxNumberOfRows] = {
      I18n::Message::MeanSymbol,
      I18n::Message::SumValuesSymbol,
      I18n::Message::SumSquareValuesSymbol,
      I18n::Message::StandardDeviationSigmaSymbol,
      I18n::Message::DeviationSymbol,
      I18n::Message::SampleStandardDeviationSSymbol,
      I18n::Message::UpperN,
      I18n::Message::Cov,
      I18n::Message::Sxy,
      I18n::Message::Y,
      I18n::Message::M,
      I18n::Message::A,
      I18n::Message::B,
      I18n::Message::C,
      I18n::Message::D,
      I18n::Message::E,
      I18n::Message::R,
      I18n::Message::R2};
  return messages[calculationIndex];
}

int CalculationController::getCalculationIndex(int j) const {
  assert(j > 0 && j < numberOfRows());
  // Ignore top row
  int calculationIndex = j - 1;
  if (calculationIndex < k_numberOfBufferCalculations) {
    return calculationIndex;
  }
  if (calculationIndex == k_numberOfBufferCalculations) {
    // Correlation coefficient
    return k_maxNumberOfRows - 2;
  }
  calculationIndex -= 1;
  bool displayRegression = hasSeriesDisplaying(&DisplayRegression);
  if (calculationIndex < k_numberOfBufferCalculations + displayRegression) {
    return calculationIndex;
  }
  calculationIndex += !displayRegression;
  bool displayCoeffM = shouldDisplayMCoefficient();
  if (calculationIndex < k_numberOfBufferCalculations + 1 + displayCoeffM) {
    return calculationIndex;
  }
  calculationIndex += !displayCoeffM;
  bool displayCoeffA = shouldDisplayACoefficient();
  if (calculationIndex < k_numberOfBufferCalculations + 2 + displayCoeffA) {
    return calculationIndex;
  }
  calculationIndex += !displayCoeffA;
  int displayedBCDECoeffs = numberOfDisplayedBCDECoefficients();
  if (calculationIndex <
      k_numberOfBufferCalculations + 3 + displayedBCDECoeffs) {
    return calculationIndex;
  }
  calculationIndex +=
      k_maxNumberOfDistinctCoefficients - 2 - displayedBCDECoeffs;
  assert(calculationIndex == k_maxNumberOfRows - 2 &&
         hasSeriesDisplaying(&DisplayR2));
  return calculationIndex + 1;
}

bool CalculationController::shouldSeriesDisplay(
    int series, DisplayCondition condition) const {
  return condition(m_store->seriesRegressionType(series));
}

bool CalculationController::hasSeriesDisplaying(
    DisplayCondition condition) const {
  int numberOfDefinedSeries = m_store->numberOfActiveSeries();
  for (int i = 0; i < numberOfDefinedSeries; i++) {
    if (shouldSeriesDisplay(m_store->seriesIndexFromActiveSeriesIndex(i),
                            condition)) {
      return true;
    }
  }
  return false;
}

int CalculationController::numberOfDisplayedCoefficients() const {
  return shouldDisplayMCoefficient() + shouldDisplayACoefficient() +
         numberOfDisplayedBCDECoefficients();
}

int CalculationController::numberOfDisplayedBCDECoefficients() const {
  int maxNumberCoefficients = 0;
  int numberOfDefinedSeries = m_store->numberOfActiveSeries();
  /* "mx+b" is the only model having a "m": coefficient. It is only available in
   * Variant1 of RegressionModelOrder. */
  for (int i = 0; i < numberOfDefinedSeries; i++) {
    int series = m_store->seriesIndexFromActiveSeriesIndex(i);
    int numberOfCoefficients =
        m_store->modelForSeries(series)->numberOfCoefficients();
    // Ignore the first coefficient A or M
    maxNumberCoefficients =
        std::max(maxNumberCoefficients, numberOfCoefficients - 1);
  }
  return maxNumberCoefficients;
}

bool CalculationController::shouldDisplayMCoefficient() const {
  if (GlobalPreferences::sharedGlobalPreferences->regressionModelOrder() !=
      CountryPreferences::RegressionModelOrder::Variant1) {
    // LinearAxpb is displayed as mx+b in Variant1 only
    return false;
  }
  int numberOfDefinedSeries = m_store->numberOfActiveSeries();
  for (int i = 0; i < numberOfDefinedSeries; i++) {
    int series = m_store->seriesIndexFromActiveSeriesIndex(i);
    if (m_store->seriesRegressionType(series) == Model::Type::LinearAxpb) {
      // This series needs a M coefficient.
      return true;
    }
  }
  return false;
}

bool CalculationController::shouldDisplayACoefficient() const {
  bool canDisplayM =
      (GlobalPreferences::sharedGlobalPreferences->regressionModelOrder() ==
       CountryPreferences::RegressionModelOrder::Variant1);
  int numberOfDefinedSeries = m_store->numberOfActiveSeries();
  for (int i = 0; i < numberOfDefinedSeries; i++) {
    int series = m_store->seriesIndexFromActiveSeriesIndex(i);
    if (!(canDisplayM &&
          m_store->seriesRegressionType(series) == Model::Type::LinearAxpb) &&
        m_store->modelForSeries(series)->numberOfCoefficients() > 0) {
      // This series needs a A coefficient.
      return true;
    }
  }
  return false;
}

void CalculationController::resetMemoization(bool force) {
  DoublePairTableController::resetMemoization(force);
  for (int s = 0; s < Store::k_numberOfSeries; s++) {
    for (int i = 0; i < 2; i++) {
      for (int j = 0; j < k_numberOfDoubleBufferCalculations; j++) {
        m_memoizedDoubleCalculationCells[s][i][j] = NAN;
      }
    }
    for (int i = 0;
         i < k_numberOfBufferCalculations - k_numberOfDoubleBufferCalculations;
         i++) {
      m_memoizedSimpleCalculationCells[s][i] = NAN;
    }
  }
}

}  // namespace Regression
