#include "calculation_controller.h"

#include <apps/apps_container_helper.h>
#include <apps/shared/editable_cell_table_view_controller.h>
#include <apps/shared/poincare_helpers.h>
#include <assert.h>
#include <poincare/layout.h>
#include <poincare/old/helpers.h>

#include <algorithm>

using namespace Poincare;
using namespace Shared;
using namespace Escher;

namespace Regression {

CalculationController::CalculationController(Responder *parentResponder,
                                             ButtonRowController *header,
                                             Store *store)
    : DoublePairTableController(parentResponder, header), m_store(store) {
  for (int i = 0; i < k_numberOfSeriesTitleCells; i++) {
    m_seriesTitleCells[i].setParentResponder(&m_selectableTableView);
  }
  for (int i = 0; i < k_numberOfDoubleCalculationCells; i++) {
    m_doubleCalculationCells[i].setParentResponder(&m_selectableTableView);
  }
  m_selectableTableView.resetSizeAndOffsetMemoization();
}

void CalculationController::tableViewDidChangeSelectionAndDidScroll(
    SelectableTableView *t, int previousSelectedCol, int previousSelectedRow,
    KDPoint previousOffset, bool withinTemporarySelection) {
  assert(t == &m_selectableTableView);
  if (!withinTemporarySelection && t->selectedColumn() > 1 &&
      t->selectedRow() >= 0 &&
      t->selectedRow() <= k_numberOfDoubleBufferCalculations) {
    /* If we are on a double text cell, we have to choose which subcell to
     * select It has to be done after the scroll for the selectedCell to be
     * defined */
    EvenOddDoubleBufferTextCell *myCell =
        static_cast<EvenOddDoubleBufferTextCell *>(t->selectedCell());
    // Default selected subcell is the left one
    bool firstSubCellSelected = true;
    if (previousSelectedCol > 1 && previousSelectedRow >= 0 &&
        previousSelectedRow <= k_numberOfDoubleBufferCalculations) {
      /* If we come from another double text cell, we have to update
       * subselection */
      EvenOddDoubleBufferTextCell *myPreviousCell =
          static_cast<EvenOddDoubleBufferTextCell *>(
              t->cellAtLocation(previousSelectedCol, previousSelectedRow));
      /* If the selection stays in the same column, we copy the subselection
       * from previous cell. Otherwise, the selection has jumped to another
       * column, we thus subselect the other subcell. */
      assert(myPreviousCell);
      firstSubCellSelected = t->selectedColumn() == previousSelectedCol
                                 ? myPreviousCell->firstTextSelected()
                                 : !myPreviousCell->firstTextSelected();
    }
    myCell->selectFirstText(firstSubCellSelected);
  }
}

bool CalculationController::canStoreCellAtLocation(int column, int row) {
  if (!DoublePairTableController::canStoreCellAtLocation(column, row)) {
    return false;
  }
  assert(row > 0 && column > 1);
  const Calculation c = calculationForRow(row);
  if (c == Calculation::Regression) {
    return false;
  }
  if (c == Calculation::CorrelationCoeff || c > Calculation::Regression) {
    AbstractEvenOddBufferTextCell *bufferCell =
        static_cast<AbstractEvenOddBufferTextCell *>(
            m_selectableTableView.cellAtLocation(column, row));
    return strcmp(bufferCell->text(), I18n::translate(I18n::Message::Dash)) &&
           strcmp(bufferCell->text(), I18n::translate(I18n::Message::Disabled));
  }
  return true;
}

int CalculationController::numberOfRows() const {
  /* Rows for : title + Mean ... Sum of Products + (CorrelationCoeff) +
   * (Regression) + (Coefficients) + (Residual stddev) + (R2) */
  return 1 + (static_cast<int>(Calculation::SumOfProducts) + 1) +
         m_store->AnyActiveSeriesSatisfies(Store::DisplayR) +
         m_store->AnyActiveSeriesSatisfies(Store::HasCoefficients) +
         numberOfDisplayedCoefficients() +
         m_store->AnyActiveSeriesSatisfies(
             Store::DisplayResidualStandardDeviation) +
         m_store->AnyActiveSeriesSatisfies(Store::DisplayR2) +
         m_store->AnyActiveSeriesSatisfies(Store::DisplayRSquared);
}

void DashBufferCell(AbstractEvenOddBufferTextCell *bufferCell) {
  bufferCell->setText(I18n::translate(I18n::Message::Dash));
}

void DisableBufferCell(AbstractEvenOddBufferTextCell *bufferCell) {
  bufferCell->setTextColor(Palette::GrayDark);
  bufferCell->setText(I18n::translate(I18n::Message::Disabled));
}

void CalculationController::fillCellForLocation(HighlightCell *cell, int column,
                                                int row) {
  int type = typeAtLocation(column, row);
  if (type == k_hideableCellType) {
    return;
  }
  EvenOddCell *myCell = static_cast<EvenOddCell *>(cell);
  myCell->setEven(row % 2 == 0);

  // Coordinate and series title
  if (type == k_seriesTitleCellType) {
    ColumnTitleCell *myCell = static_cast<ColumnTitleCell *>(cell);
    size_t series = m_store->seriesIndexFromActiveSeriesIndex(
        column - k_numberOfHeaderColumns);
    assert(series < DoublePairStore::k_numberOfSeries);
    char buffer[ClearColumnHelper::k_maxSizeOfColumnName];
    m_store->fillColumnName(series, 0, buffer);
    myCell->setFirstText(buffer);
    m_store->fillColumnName(series, 1, buffer);
    myCell->setSecondText(buffer);
    assert(series < Palette::numberOfDataColors());
    myCell->setColor(Palette::DataColor[series]);
    return;
  }

  const Calculation c = calculationForRow(row);
  bool forbidStatsDiagnostics =
      Preferences::SharedPreferences()->examMode().forbidStatsDiagnostics();
  // Calculation title and symbols
  if (type == k_calculationTitleCellType ||
      type == k_calculationSymbolCellType) {
    EvenOddMessageTextCell *myCell =
        static_cast<EvenOddMessageTextCell *>(cell);
    myCell->setTextColor(
        type == k_calculationTitleCellType ? KDColorBlack : Palette::GrayDark);
    I18n::Message message = type == k_calculationTitleCellType
                                ? MessageForCalculation(c)
                                : SymbolForCalculation(c);
    myCell->setMessage(message);
    if ((c == Calculation::CorrelationCoeff ||
         c == Calculation::DeterminationCoeff || c == Calculation::RSquared) &&
        forbidStatsDiagnostics) {
      // R and R2 messages should be grayed out.
      myCell->setTextColor(Palette::GrayDark);
    }
    return;
  }

  size_t series = m_store->seriesIndexFromActiveSeriesIndex(
      column - k_numberOfHeaderColumns);
  assert(series < DoublePairStore::k_numberOfSeries);
  Model::Type regressionType = m_store->seriesRegressionType(series);

  // Regression cell
  if (c == Calculation::Regression) {
    Model *model = m_store->modelForSeries(series);
    static_cast<AbstractEvenOddBufferTextCell *>(cell)->setText(
        Store::HasCoefficients(regressionType)
            ? model->formula()
            : I18n::translate(I18n::Message::Dash));
    static_cast<AbstractEvenOddBufferTextCell *>(cell)->setTextColor(
        KDColorBlack);
    return;
  }

  assert(type == k_doubleBufferCalculationCellType ||
         type == k_calculationCellType);
  constexpr int bufferSize = PrintFloat::charSizeForFloatsWithPrecision(
      AbstractEvenOddBufferTextCell::k_defaultPrecision);
  char buffer[bufferSize];

  // Double calculation cells
  if (c <= Calculation::SampleStandardDeviationS) {
    int calculationIndex = static_cast<int>(c);
    using DoubleCalculation =
        double (Store::*)(int, int, Store::CalculationOptions) const;
    constexpr DoubleCalculation
        calculationMethods[k_numberOfDoubleBufferCalculations] = {
            &Store::meanOfColumn,
            &Store::sumOfColumn,
            &Store::squaredValueSumOfColumn,
            &Store::standardDeviationOfColumn,
            &Store::varianceOfColumn,
            &Store::sampleStandardDeviationOfColumn,
        };
    double *calculation1 =
        m_memoizedDoubleCalculationCells[series][0] + calculationIndex;
    double *calculation2 =
        m_memoizedDoubleCalculationCells[series][1] + calculationIndex;
    if (std::isnan(*calculation1) || std::isnan(*calculation2)) {
      *calculation1 = (m_store->*calculationMethods[calculationIndex])(
          series, 0, Store::CalculationOptions());
      *calculation2 = (m_store->*calculationMethods[calculationIndex])(
          series, 1, Store::CalculationOptions());
    }
    assert(Helpers::EqualOrBothNan(
               *calculation1, (m_store->*calculationMethods[calculationIndex])(
                                  series, 0, Store::CalculationOptions())) &&
           Helpers::EqualOrBothNan(
               *calculation2, (m_store->*calculationMethods[calculationIndex])(
                                  series, 1, Store::CalculationOptions())));
    EvenOddDoubleBufferTextCell *myCell =
        static_cast<EvenOddDoubleBufferTextCell *>(cell);
    PoincareHelpers::ConvertFloatToText<double>(
        *calculation1, buffer, bufferSize,
        AbstractEvenOddBufferTextCell::k_defaultPrecision);
    myCell->setFirstText(buffer);
    PoincareHelpers::ConvertFloatToText<double>(
        *calculation2, buffer, bufferSize,
        AbstractEvenOddBufferTextCell::k_defaultPrecision);
    myCell->setSecondText(buffer);
    return;
  }

  // Single calculation cells
  Context *globContext =
      AppsContainerHelper::sharedAppsContainerGlobalContext();
  AbstractEvenOddBufferTextCell *bufferCell =
      static_cast<AbstractEvenOddBufferTextCell *>(cell);
  bufferCell->setTextColor(KDColorBlack);
  double result = NAN;

  if (c >= Calculation::NumberOfDots && c <= Calculation::SumOfProducts) {
    int calculationIndex =
        static_cast<int>(c) - static_cast<int>(Calculation::NumberOfDots);
    using SingleCalculation = double (Store::*)(int) const;
    constexpr SingleCalculation
        calculationMethods[k_numberOfMemoizedSingleBufferCalculations] = {
            &Store::doubleCastedNumberOfPairsOfSeries, &Store::covariance,
            &Store::columnProductSum};
    double *calculation =
        m_memoizedSimpleCalculationCells[series] + calculationIndex;
    if (std::isnan(*calculation)) {
      *calculation = (m_store->*calculationMethods[calculationIndex])(series);
    }
    assert(
        (c == Calculation::NumberOfDots &&
         Helpers::EqualOrBothNan(
             *calculation,
             m_store->doubleCastedNumberOfPairsOfSeries(series))) ||
        (c == Calculation::Covariance &&
         Helpers::EqualOrBothNan(*calculation, m_store->covariance(series))) ||
        (c == Calculation::SumOfProducts &&
         Helpers::EqualOrBothNan(*calculation,
                                 m_store->columnProductSum(series))));
    result = *calculation;
  } else if (c >= Calculation::CoefficientM && c <= Calculation::CoefficientE) {
    if (!m_store->coefficientsAreDefined(series, globContext)) {
      // Put dashes if regression is not defined
      return DashBufferCell(bufferCell);
    }
    int coefficientIndex =
        static_cast<int>(c) - static_cast<int>(Calculation::CoefficientA);
    int numberOfCoefficients =
        m_store->modelForSeries(series)->numberOfCoefficients();
    if (coefficientIndex <= 0 && Store::HasCoefficientM(regressionType)) {
      assert(!Store::HasCoefficientA(regressionType));
      // In that case only, M is coefficientIndex 0 and A is coefficientIndex -1
      coefficientIndex = -coefficientIndex - 1;
    }
    if (coefficientIndex < 0 || coefficientIndex >= numberOfCoefficients) {
      return DashBufferCell(bufferCell);
    }
    result =
        m_store->coefficientsForSeries(series, globContext)[coefficientIndex];
  } else if (c == Calculation::CorrelationCoeff) {
    // This could be memoized but don't seem to slow the table down for now.
    if (!Store::DisplayR(regressionType)) {
      return DashBufferCell(bufferCell);
    }
    if (forbidStatsDiagnostics) {
      return DisableBufferCell(bufferCell);
    }
    result = m_store->correlationCoefficient(series);
  } else if (c == Calculation::ResidualStandardDeviation) {
    if (!Store::DisplayResidualStandardDeviation(regressionType)) {
      return DashBufferCell(bufferCell);
    }
    result = m_store->residualStandardDeviation(series, globContext);
  } else {
    assert(c == Calculation::DeterminationCoeff || c == Calculation::RSquared);
    if ((c == Calculation::DeterminationCoeff &&
         Store::DisplayR2(regressionType)) ||
        (c == Calculation::RSquared &&
         Store::DisplayRSquared(regressionType))) {
      if (forbidStatsDiagnostics) {
        return DisableBufferCell(bufferCell);
      }
      result = m_store->determinationCoefficientForSeries(series, globContext);
    } else {
      return DashBufferCell(bufferCell);
    }
  }
  PoincareHelpers::ConvertFloatToText<double>(
      result, buffer, bufferSize,
      AbstractEvenOddBufferTextCell::k_defaultPrecision);
  bufferCell->setText(buffer);
}

KDCoordinate CalculationController::nonMemoizedColumnWidth(int column) {
  if (column == 0) {
    return k_calculationTitleCellWidth;
  }
  if (column == 1) {
    return k_symbolColumnWidth;
  }
  Model::Type currentType =
      m_store->seriesRegressionType(m_store->seriesIndexFromActiveSeriesIndex(
          column - k_numberOfHeaderColumns));
  if (currentType == Model::Type::Quartic) {
    return k_quarticCalculationCellWidth;
  }
  if (currentType == Model::Type::Cubic) {
    return k_cubicCalculationCellWidth;
  }
  return k_minCalculationCellWidth;
}

HighlightCell *CalculationController::reusableCell(int index, int type) {
  assert(0 <= index && index < reusableCellCount(type));
  switch (type) {
    case k_seriesTitleCellType:
      return &m_seriesTitleCells[index];
    case k_calculationCellType:
      return &m_calculationCells[index];
    case k_doubleBufferCalculationCellType:
      return &m_doubleCalculationCells[index];
    default:
      return DoublePairTableController::reusableCell(index, type);
  }
}

int CalculationController::reusableCellCount(int type) const {
  if (type == k_doubleBufferCalculationCellType) {
    return k_numberOfDoubleCalculationCells;
  }
  return DoublePairTableController::reusableCellCount(type);
}

int CalculationController::typeAtLocation(int column, int row) const {
  if (column > 1 && row > 0 && row <= k_numberOfDoubleBufferCalculations) {
    return k_doubleBufferCalculationCellType;
  }
  return DoublePairTableController::typeAtLocation(column, row);
}

I18n::Message CalculationController::MessageForCalculation(Calculation c) {
  constexpr I18n::Message
      messages[static_cast<int>(Calculation::NumberOfRows)] = {
          I18n::Message::Mean,
          I18n::Message::Sum,
          I18n::Message::SquareSum,
          I18n::Message::StandardDeviation,
          I18n::Message::Deviation,
          I18n::Message::SampleStandardDeviationS,
          I18n::Message::NumberOfDots,
          I18n::Message::Covariance,
          I18n::Message::SumOfProducts,
          I18n::Message::CorrelationCoeff,
          I18n::Message::Regression,
          I18n::Message::CoefficientM,
          I18n::Message::CoefficientA,
          I18n::Message::CoefficientB,
          I18n::Message::CoefficientC,
          I18n::Message::CoefficientD,
          I18n::Message::CoefficientE,
          I18n::Message::ResidualStandardDeviationMessageInTable,
          I18n::Message::DeterminationCoeff,
          I18n::Message::DeterminationCoeff,
      };
  int index = static_cast<int>(c);
  assert(index >= 0 && index < static_cast<int>(Calculation::NumberOfRows));
  return messages[index];
}

I18n::Message CalculationController::SymbolForCalculation(Calculation c) {
  constexpr I18n::Message
      messages[static_cast<int>(Calculation::NumberOfRows)] = {
          I18n::Message::MeanSymbol,
          I18n::Message::SumValuesSymbol,
          I18n::Message::SumSquareValuesSymbol,
          I18n::Message::Sigma,
          I18n::Message::DeviationSymbol,
          I18n::Message::S,
          I18n::Message::UpperN,
          I18n::Message::Cov,
          I18n::Message::Sxy,
          I18n::Message::R,
          I18n::Message::Y,
          I18n::Message::M,
          I18n::Message::A,
          I18n::Message::B,
          I18n::Message::C,
          I18n::Message::D,
          I18n::Message::E,
          I18n::Message::ResidualStandardDeviationSymbol,
          I18n::Message::R2,
          I18n::Message::RSquared,
      };
  int index = static_cast<int>(c);
  assert(index >= 0 && index < static_cast<int>(Calculation::NumberOfRows));
  return messages[index];
}

CalculationController::Calculation CalculationController::calculationForRow(
    int row) const {
  assert(row > 0 && row < numberOfRows());
  // Ignore top row
  row--;
  if (row < static_cast<int>(Calculation::CorrelationCoeff)) {
    return static_cast<Calculation>(row);
  }
  row += !m_store->AnyActiveSeriesSatisfies(Store::DisplayR);
  if (row == static_cast<int>(Calculation::CorrelationCoeff)) {
    return Calculation::CorrelationCoeff;
  }
  row += !m_store->AnyActiveSeriesSatisfies(Store::HasCoefficients);
  if (row == static_cast<int>(Calculation::Regression)) {
    return Calculation::Regression;
  }
  row += !m_store->AnyActiveSeriesSatisfies(Store::HasCoefficientM);
  if (row == static_cast<int>(Calculation::CoefficientM)) {
    return Calculation::CoefficientM;
  }
  row += !m_store->AnyActiveSeriesSatisfies(Store::HasCoefficientA);
  if (row == static_cast<int>(Calculation::CoefficientA)) {
    return Calculation::CoefficientA;
  }
  int displayedBCDECoeffs = numberOfDisplayedBCDECoefficients();
  if (row <=
      static_cast<int>(Calculation::CoefficientA) + displayedBCDECoeffs) {
    return static_cast<Calculation>(row);
  }
  row += static_cast<int>(Calculation::CoefficientE) -
         static_cast<int>(Calculation::CoefficientB) + 1 - displayedBCDECoeffs;
  row += !m_store->AnyActiveSeriesSatisfies(
      Store::DisplayResidualStandardDeviation);
  if (row == static_cast<int>(Calculation::ResidualStandardDeviation)) {
    return Calculation::ResidualStandardDeviation;
  }
  row += !m_store->AnyActiveSeriesSatisfies(Store::DisplayR2);
  if (row == static_cast<int>(Calculation::DeterminationCoeff)) {
    return Calculation::DeterminationCoeff;
  }
  assert(row == static_cast<int>(Calculation::NumberOfRows) - 1 &&
         m_store->AnyActiveSeriesSatisfies(Store::DisplayRSquared));
  return static_cast<Calculation>(row);
}

int CalculationController::numberOfDisplayedCoefficients() const {
  return m_store->AnyActiveSeriesSatisfies(Store::HasCoefficientM) +
         m_store->AnyActiveSeriesSatisfies(Store::HasCoefficientA) +
         numberOfDisplayedBCDECoefficients();
}

int CalculationController::numberOfDisplayedBCDECoefficients() const {
  int maxNumberCoefficients = 0;
  int numberOfDefinedSeries = m_store->numberOfActiveSeries();
  /* "mx+b" is the only model having a "m": coefficient. It is only available in
   * Variant1 of RegressionApp. */
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

void CalculationController::resetSizeMemoization() {
  DoublePairTableController::resetSizeMemoization();
  for (int s = 0; s < k_numberOfSeriesTitleCells; s++) {
    for (int i = 0; i < 2; i++) {
      for (int j = 0; j < k_numberOfDoubleBufferCalculations; j++) {
        m_memoizedDoubleCalculationCells[s][i][j] = NAN;
      }
    }
    for (int i = 0; i < k_numberOfMemoizedSingleBufferCalculations; i++) {
      m_memoizedSimpleCalculationCells[s][i] = NAN;
    }
  }
}

}  // namespace Regression
