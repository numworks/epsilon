#include "calculation_controller.h"
#include "../apps_container.h"
#include "../exam_mode_configuration.h"
#include "../shared/editable_cell_table_view_controller.h"
#include "../shared/poincare_helpers.h"
#include <poincare/code_point_layout.h>
#include <poincare/vertical_offset_layout.h>
#include <poincare/preferences.h>
#include <algorithm>
#include <assert.h>

using namespace Poincare;
using namespace Shared;
using namespace Escher;

namespace Regression {

CalculationController::CalculationController(Responder * parentResponder, ButtonRowController * header, Store * store) :
  DoublePairTableController(parentResponder, header),
  m_store(store)
{
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    m_columnTitleCells[i].setParentResponder(&m_selectableTableView);
  }
  for (int i = 0; i < k_numberOfDoubleCalculationCells; i++) {
    m_doubleCalculationCells[i].setParentResponder(&m_selectableTableView);
  }
  for (int i = 0; i < k_maxNumberOfDisplayableRows; i++) {
    m_titleCells[i].setAlignment(KDContext::k_alignRight, KDContext::k_alignCenter);
    m_titleCells[i].setMessageFont(KDFont::SmallFont);
    m_titleSymbolCells[i].setAlignment(KDContext::k_alignCenter, KDContext::k_alignCenter);
    m_titleSymbolCells[i].setMessageFont(KDFont::SmallFont);
  }
  for (int i = 0; i < k_numberOfHeaderColumns; i++) {
    m_hideableCell[i].setHide(true);
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
  if (t->selectedColumn() > 1 && t->selectedRow() >= 0 && t->selectedRow() <= k_totalNumberOfDoubleBufferRows) {
    // If we are on a double text cell, we have to choose which subcell to select
    EvenOddDoubleBufferTextCellWithSeparator * myCell = (EvenOddDoubleBufferTextCellWithSeparator *)t->selectedCell();
    // Default selected subcell is the left one
    bool firstSubCellSelected = true;
    if (previousSelectedCellX > 1 && previousSelectedCellY >= 0 && previousSelectedCellY <= k_totalNumberOfDoubleBufferRows) {
      // If we come from another double text cell, we have to update subselection
      EvenOddDoubleBufferTextCellWithSeparator * myPreviousCell = (EvenOddDoubleBufferTextCellWithSeparator *)t->cellAtLocation(previousSelectedCellX, previousSelectedCellY);
      /* If the selection stays in the same column, we copy the subselection
       * from previous cell. Otherwise, the selection has jumped to another
       * column, we thus subselect the other subcell. */
       assert(myPreviousCell);
      firstSubCellSelected = t->selectedColumn() == previousSelectedCellX ? myPreviousCell->firstTextSelected() : !myPreviousCell->firstTextSelected();
    }
    myCell->selectFirstText(firstSubCellSelected);
  }
}

int CalculationController::numberOfRows() const {
  /* Rows for : title + Mean ... Variance + Number of points + Covariance + ∑xy
   * + (Regression) + Coefficients + (R) + (R2) */
  return 1 + k_totalNumberOfDoubleBufferRows + 3 + hasSeriesDisplaying(&DisplayRegression) + maxNumberOfCoefficients() + hasSeriesDisplaying(&DisplayR) + hasSeriesDisplaying(&DisplayR2);
}

int CalculationController::numberOfColumns() const {
  return 2 + m_store->numberOfValidSeries();
}

void CalculationController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  if (i <= 1 && j == 0) {
    return;
  }
  EvenOddCell * myCell = static_cast<EvenOddCell *>(cell);
  myCell->setEven(j%2 == 0);
  myCell->setHighlighted(i == selectedColumn() && j == selectedRow());

  const int numberRows = numberOfRows();
  const int numberOfRowsBeforeCoefficients = k_regressionCellIndex + hasSeriesDisplaying(&DisplayRegression) - 1;
  // Calculation title and symbols
  if (i <= 1) {
    EvenOddMessageTextCell * myCell = static_cast<EvenOddMessageTextCell *>(cell);
    myCell->setTextColor(i == 1 ? Palette::GrayDark : KDColorBlack);
    if (j <= numberOfRowsBeforeCoefficients) {
      I18n::Message titles[k_regressionCellIndex][k_numberOfHeaderColumns] = {
        { I18n::Message::Mean, I18n::Message::MeanSymbol },
        { I18n::Message::Sum, I18n::Message::SumValuesSymbol },
        { I18n::Message::SquareSum, I18n::Message::SumSquareValuesSymbol },
        { I18n::Message::StandardDeviation, I18n::Message::StandardDeviationSigmaSymbol },
        { I18n::Message::Deviation, I18n::Message::DeviationSymbol },
        { I18n::Message::SampleStandardDeviationS, I18n::Message::SampleStandardDeviationSSymbol},
        { I18n::Message::NumberOfDots, I18n::Message::UpperN },
        { I18n::Message::Covariance, I18n::Message::Cov },
        { I18n::Message::SumOfProducts, I18n::Message::Sxy },
        { I18n::Message::Regression, I18n::Message::Y }
      };
      myCell->setMessage(titles[j-1][i]);
      return;
    }
    // R cannot be displayed without R2
    assert(!hasSeriesDisplaying(&DisplayR) || hasSeriesDisplaying(&DisplayR2));
    if (hasSeriesDisplaying(&DisplayR2) && (j == numberRows - 1 || (hasSeriesDisplaying(&DisplayR) && j == numberRows - 2))) {
      I18n::Message titles[2][k_numberOfHeaderColumns] = {
        { I18n::Message::DeterminationCoeff, I18n::Message::R2 },
        { I18n::Message::CorrelationCoeff, I18n::Message::R }
      };
      myCell->setMessage((j == numberRows - 1) ? titles[0][i] : titles[1][i]);
      if (ExamModeConfiguration::statsDiagnosticsAreForbidden()) {
        myCell->setTextColor(Palette::GrayDark);
      }
      return;
    }
    I18n::Message titles[5][k_numberOfHeaderColumns] = {
      { I18n::Message::CoefficientA, I18n::Message::A },
      { I18n::Message::CoefficientB, I18n::Message::B },
      { I18n::Message::CoefficientC, I18n::Message::C },
      { I18n::Message::CoefficientD, I18n::Message::D },
      { I18n::Message::CoefficientE, I18n::Message::E }
    };
    myCell->setMessage(titles[j - numberOfRowsBeforeCoefficients - 1][i]);
    return;
  }

  size_t seriesNumber = m_store->indexOfKthValidSeries(i - k_numberOfHeaderColumns);
  assert(seriesNumber < DoublePairStore::k_numberOfSeries);

  // Coordinate and series title
  if (j == 0 && i > 1) {
    ColumnTitleCell * myCell = static_cast<ColumnTitleCell *>(cell);
    char buffer[Shared::ClearColumnHelper::k_maxSizeOfColumnName];
    m_store->fillColumnName(seriesNumber, 0, buffer);
    myCell->setFirstText(buffer);
    m_store->fillColumnName(seriesNumber, 1, buffer);
    myCell->setSecondText(buffer);
    assert(seriesNumber < Palette::numberOfDataColors());
    myCell->setColor(Palette::DataColor[seriesNumber]);
    return;
  }

  // Calculation cell
  const int numberSignificantDigits = Preferences::VeryLargeNumberOfSignificantDigits;
  if (i > 1 && j > 0 && j <= k_totalNumberOfDoubleBufferRows) {
    ArgCalculPointer calculationMethods[k_totalNumberOfDoubleBufferRows] = {
      &Store::meanOfColumn,
      &Store::sumOfColumn,
      &Store::squaredValueSumOfColumn,
      &Store::standardDeviationOfColumn,
      &Store::varianceOfColumn,
      &Store::sampleStandardDeviationOfColumn,
    };
    if (std::isnan(m_memoizedDoubleCalculationCells[seriesNumber][0][j-1]) || std::isnan(m_memoizedDoubleCalculationCells[seriesNumber][1][j-1])) {
      m_memoizedDoubleCalculationCells[seriesNumber][0][j-1] = (m_store->*calculationMethods[j-1])(seriesNumber, 0, false);
      m_memoizedDoubleCalculationCells[seriesNumber][1][j-1] = (m_store->*calculationMethods[j-1])(seriesNumber, 1, false);
    }
    double calculation1 = m_memoizedDoubleCalculationCells[seriesNumber][0][j-1];
    double calculation2 = m_memoizedDoubleCalculationCells[seriesNumber][1][j-1];
    assert((calculation1 == (m_store->*calculationMethods[j-1])(seriesNumber, 0, false)
         || (std::isnan(calculation1) && std::isnan((m_store->*calculationMethods[j-1])(seriesNumber, 0, false))))
        && (calculation2 == (m_store->*calculationMethods[j-1])(seriesNumber, 1, false)
          || (std::isnan(calculation2) && std::isnan((m_store->*calculationMethods[j-1])(seriesNumber, 1, false)))));
    EvenOddDoubleBufferTextCellWithSeparator * myCell = static_cast<EvenOddDoubleBufferTextCellWithSeparator *>(cell);
    constexpr int bufferSize = PrintFloat::charSizeForFloatsWithPrecision(numberSignificantDigits);
    char buffer[bufferSize];
    PoincareHelpers::ConvertFloatToText<double>(calculation1, buffer, bufferSize, numberSignificantDigits);
    myCell->setFirstText(buffer);
    PoincareHelpers::ConvertFloatToText<double>(calculation2, buffer, bufferSize, numberSignificantDigits);
    myCell->setSecondText(buffer);
    return;
  }
  SeparatorEvenOddBufferTextCell * bufferCell = static_cast<SeparatorEvenOddBufferTextCell *>(cell);
  bufferCell->setTextColor(KDColorBlack);
  if (i > 1 && j == k_regressionCellIndex) {
    Model * model = m_store->modelForSeries(seriesNumber);
    I18n::Message message = shouldSeriesDisplay(seriesNumber, &DisplayRegression) ? model->formulaMessage() : I18n::Message::Dash;
    bufferCell->setText(I18n::translate(message));
    return;
  }
  if (i > 1 && j > k_totalNumberOfDoubleBufferRows && j < k_regressionCellIndex) {
    assert(j != k_regressionCellIndex);
    const int calculationIndex = j-k_totalNumberOfDoubleBufferRows-1;
    if (std::isnan(m_memoizedSimpleCalculationCells[seriesNumber][calculationIndex])) {
      if (calculationIndex == 0) {
        m_memoizedSimpleCalculationCells[seriesNumber][calculationIndex] = m_store->doubleCastedNumberOfPairsOfSeries(seriesNumber);
      } else if (calculationIndex == 1) {
        m_memoizedSimpleCalculationCells[seriesNumber][calculationIndex] = m_store->covariance(seriesNumber);
      } else {
        assert(calculationIndex == 2);
        m_memoizedSimpleCalculationCells[seriesNumber][calculationIndex] = m_store->columnProductSum(seriesNumber);
      }
    }
    assert(
        (calculationIndex == 0 && m_memoizedSimpleCalculationCells[seriesNumber][calculationIndex] == m_store->doubleCastedNumberOfPairsOfSeries(seriesNumber))
        || (calculationIndex == 1 && m_memoizedSimpleCalculationCells[seriesNumber][calculationIndex] == m_store->covariance(seriesNumber))
        || (calculationIndex == 2 && m_memoizedSimpleCalculationCells[seriesNumber][calculationIndex] == m_store->columnProductSum(seriesNumber))
        );
    double calculation = m_memoizedSimpleCalculationCells[seriesNumber][calculationIndex];
    constexpr int bufferSize = PrintFloat::charSizeForFloatsWithPrecision(numberSignificantDigits);
    char buffer[bufferSize];
    PoincareHelpers::ConvertFloatToText<double>(calculation, buffer, bufferSize, numberSignificantDigits);
    bufferCell->setText(buffer);
    return;
  }
  if (i > 1 && j > k_totalNumberOfDoubleBufferRows) {
    assert(j > numberOfRowsBeforeCoefficients);
    int maxNumberCoefficients = maxNumberOfCoefficients();

    Poincare::Context * globContext = AppsContainer::sharedAppsContainer()->globalContext();
    if (!m_store->coefficientsAreDefined(seriesNumber, globContext)) {
      // Put dashes if regression is not defined
      bufferCell->setText(I18n::translate(I18n::Message::Dash));
      return;
    }

    if (j > numberOfRowsBeforeCoefficients + maxNumberCoefficients) {
      // Fill r and r2 if needed
      if (shouldSeriesDisplay(seriesNumber, DisplayR2) && (j == numberRows - 1 || (shouldSeriesDisplay(seriesNumber, DisplayR) && j == numberRows - 2))) {
        if (ExamModeConfiguration::statsDiagnosticsAreForbidden()) {
          bufferCell->setTextColor(Palette::GrayDark);
          bufferCell->setText(I18n::translate(I18n::Message::Disabled));
          return;
        }
        double calculation;
        if (j == numberRows - 1) {
          calculation = m_store->determinationCoefficientForSeries(seriesNumber, globContext);
        } else {
          calculation = m_store->correlationCoefficient(seriesNumber);
        }
        constexpr int bufferSize = PrintFloat::charSizeForFloatsWithPrecision(numberSignificantDigits);
        char buffer[bufferSize];
        PoincareHelpers::ConvertFloatToText<double>(calculation, buffer, bufferSize, numberSignificantDigits);
        bufferCell->setText(buffer);
        return;
      } else {
        bufferCell->setText(I18n::translate(I18n::Message::Dash));
        return;
      }
    } else {
      // Fill the current coefficient if needed
      int currentNumberOfCoefs = m_store->modelForSeries(seriesNumber)->numberOfCoefficients();
      if (j > k_regressionCellIndex + currentNumberOfCoefs) {
        bufferCell->setText(I18n::translate(I18n::Message::Dash));
        return;
      } else {
        double * coefficients = m_store->coefficientsForSeries(seriesNumber, globContext);
        constexpr int bufferSize = PrintFloat::charSizeForFloatsWithPrecision(numberSignificantDigits);
        char buffer[bufferSize];
        PoincareHelpers::ConvertFloatToText<double>(coefficients[j - k_regressionCellIndex - 1], buffer, bufferSize, numberSignificantDigits);
        bufferCell->setText(buffer);
        return;
      }
    }
  }
}

KDCoordinate CalculationController::columnWidth(int i) {
  if (i == 0) {
    return k_titleCalculationCellWidth;
  }
  if (i == 1) {
    return k_symbolColumnWidth;
  }
  Model::Type currentType = m_store->seriesRegressionType(m_store->indexOfKthValidSeries(i - k_numberOfHeaderColumns));
  if (currentType == Model::Type::Quartic) {
    return k_quarticCalculationCellWidth;
  }
  if (currentType == Model::Type::Cubic) {
    return k_cubicCalculationCellWidth;
  }
  return k_minCalculationCellWidth;
}

HighlightCell * CalculationController::reusableCell(int index, int type) {
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
  if (type == k_standardCalculationTitleCellType || type == k_symbolCalculationTitleCellType) {
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
  if (j > 0 && j <= k_totalNumberOfDoubleBufferRows) {
    return k_doubleBufferCalculationCellType;
  }
  return k_standardCalculationCellType;
}

bool CalculationController::shouldSeriesDisplay(int series, DisplayCondition condition) const {
  return condition(m_store->seriesRegressionType(series));
}

bool CalculationController::hasSeriesDisplaying(DisplayCondition condition) const {
  int numberOfDefinedSeries = m_store->numberOfValidSeries();
  for (int i = 0; i < numberOfDefinedSeries; i++) {
    if (shouldSeriesDisplay(m_store->indexOfKthValidSeries(i), condition)) {
      return true;
    }
  }
  return false;
}

int CalculationController::maxNumberOfCoefficients() const {
  int maxNumberCoefficients = 0;
  int numberOfDefinedSeries = m_store->numberOfValidSeries();
  for (int i = 0; i < numberOfDefinedSeries; i++) {
    int currentNumberOfCoefs = m_store->modelForSeries(m_store->indexOfKthValidSeries(i))->numberOfCoefficients();
    maxNumberCoefficients = std::max(maxNumberCoefficients, currentNumberOfCoefs);
  }
  return maxNumberCoefficients;
}

void CalculationController::resetMemoization() {
  for (int s = 0; s < Store::k_numberOfSeries; s++) {
    for (int i = 0; i < 2; i++) {
      for (int j = 0; j < k_totalNumberOfDoubleBufferRows; j++) {
        m_memoizedDoubleCalculationCells[s][i][j] = NAN;
      }
    }
    for (int i = 0; i < k_regressionCellIndex-k_totalNumberOfDoubleBufferRows-1; i++) {
      m_memoizedSimpleCalculationCells[s][i] = NAN;
    }
  }
}

}

