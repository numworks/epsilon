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
  m_selectableTableView.setDelegate(this);
  m_selectableTableView.setVerticalCellOverlap(0);
  m_selectableTableView.setBackgroundColor(Palette::WallScreenDark);
  m_selectableTableView.setMargins(k_margin, k_scrollBarMargin, k_scrollBarMargin, k_margin);
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    m_columnTitleCells[i].setParentResponder(&m_selectableTableView);
  }
  for (int i = 0; i < k_numberOfDoubleCalculationCells; i++) {
    m_doubleCalculationCells[i].setParentResponder(&m_selectableTableView);
  }
  for (int i = 0; i < k_maxNumberOfDisplayableRows; i++) {
    m_titleCells[i].setMessageFont(KDFont::SmallFont);
  }
  m_hideableCell.setHide(true);
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
  if (t->selectedRow() == 0 && t->selectedColumn() == 0) {
    if (previousSelectedCellX == 0 && previousSelectedCellY == 1) {
      selectableTableView()->deselectTable();
      Container::activeApp()->setFirstResponder(tabController());
    } else {
      t->selectCellAtLocation(0, 1);
    }
  }
  if (t->selectedColumn() > 0 && t->selectedRow() >= 0 && t->selectedRow() <= k_totalNumberOfDoubleBufferRows) {
    // If we are on a double text cell, we have to choose which subcell to select
    EvenOddDoubleBufferTextCellWithSeparator * myCell = (EvenOddDoubleBufferTextCellWithSeparator *)t->selectedCell();
    // Default selected subcell is the left one
    bool firstSubCellSelected = true;
    if (previousSelectedCellX > 0 && previousSelectedCellY >= 0 && previousSelectedCellY <= k_totalNumberOfDoubleBufferRows) {
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
  return 1 + m_store->numberOfValidSeries();
}

void CalculationController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  if (i == 0 && j == 0) {
    return;
  }
  EvenOddCell * myCell = static_cast<EvenOddCell *>(cell);
  myCell->setEven(j%2 == 0);
  myCell->setHighlighted(i == selectedColumn() && j == selectedRow());

  const int numberRows = numberOfRows();
  const int numberOfRowsBeforeCoefficients = k_regressionCellIndex + hasSeriesDisplaying(&DisplayRegression) - 1;
  // Calculation title
  if (i == 0) {
    EvenOddMessageTextCell * myCell = static_cast<EvenOddMessageTextCell *>(cell);
    myCell->setTextColor(KDColorBlack);
    myCell->setAlignment(KDContext::k_alignRight, KDContext::k_alignCenter);
    if (j <= numberOfRowsBeforeCoefficients) {
      I18n::Message titles[k_regressionCellIndex] = {I18n::Message::Mean, I18n::Message::Sum, I18n::Message::SquareSum, I18n::Message::StandardDeviation, I18n::Message::Deviation, I18n::Message::NumberOfDots, I18n::Message::Covariance, I18n::Message::Sxy, I18n::Message::Regression};
      myCell->setMessage(titles[j-1]);
      return;
    }
    // R cannot be displayed without R2
    assert(!hasSeriesDisplaying(&DisplayR) || hasSeriesDisplaying(&DisplayR2));
    if (hasSeriesDisplaying(&DisplayR2) && (j == numberRows - 1 || (hasSeriesDisplaying(&DisplayR) && j == numberRows - 2))) {
      myCell->setMessage((j == numberRows - 1) ? I18n::Message::R2 : I18n::Message::R);
      if (ExamModeConfiguration::statsDiagnosticsAreForbidden()) {
        myCell->setTextColor(Palette::GrayDark);
      }
      return;
    }
    I18n::Message titles[5] = {I18n::Message::A, I18n::Message::B, I18n::Message::C, I18n::Message::D, I18n::Message::E};
    myCell->setMessage(titles[j - numberOfRowsBeforeCoefficients - 1]);
    return;
  }

  size_t seriesNumber = m_store->indexOfKthValidSeries(i - 1);
  assert(seriesNumber < DoublePairStore::k_numberOfSeries);

  // Coordinate and series title
  if (j == 0 && i > 0) {
    ColumnTitleCell * myCell = static_cast<ColumnTitleCell *>(cell);
    char buffer[Shared::EditableCellTableViewController::k_maxSizeOfColumnName];
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
  if (i > 0 && j > 0 && j <= k_totalNumberOfDoubleBufferRows) {
    ArgCalculPointer calculationMethods[k_totalNumberOfDoubleBufferRows] = {&Store::meanOfColumn, &Store::sumOfColumn, &Store::squaredValueSumOfColumn, &Store::standardDeviationOfColumn, &Store::varianceOfColumn};
    double calculation1 = (m_store->*calculationMethods[j-1])(seriesNumber, 0, false);
    double calculation2 = (m_store->*calculationMethods[j-1])(seriesNumber, 1, false);
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
  if (i > 0 && j == k_regressionCellIndex) {
    Model * model = m_store->modelForSeries(seriesNumber);
    I18n::Message message = shouldSeriesDisplay(seriesNumber, &DisplayRegression) ? model->formulaMessage() : I18n::Message::Dash;
    bufferCell->setText(I18n::translate(message));
    return;
  }
  if (i > 0 && j > k_totalNumberOfDoubleBufferRows && j < k_regressionCellIndex) {
    assert(j != k_regressionCellIndex);
    double calculation = 0;
    const int calculationIndex = j-k_totalNumberOfDoubleBufferRows-1;
    if (calculationIndex == 0) {
      calculation = m_store->doubleCastedNumberOfPairsOfSeries(seriesNumber);
    } else if (calculationIndex == 1) {
      calculation = m_store->covariance(seriesNumber);
    } else {
      assert(calculationIndex == 2);
      calculation = m_store->columnProductSum(seriesNumber);
    }
    constexpr int bufferSize = PrintFloat::charSizeForFloatsWithPrecision(numberSignificantDigits);
    char buffer[bufferSize];
    PoincareHelpers::ConvertFloatToText<double>(calculation, buffer, bufferSize, numberSignificantDigits);
    bufferCell->setText(buffer);
    return;
  }
  if (i > 0 && j > k_totalNumberOfDoubleBufferRows) {
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
  Model::Type currentType = m_store->seriesRegressionType(m_store->indexOfKthValidSeries(i-1));
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
  if (type == k_columnTitleCellType) {
    assert(index >= 0 && index < Store::k_numberOfSeries);
    return &m_columnTitleCells[index];
  }
  if (type == k_doubleBufferCalculationCellType) {
    assert(index >= 0 && index < k_numberOfDoubleCalculationCells);
    return &m_doubleCalculationCells[index];
  }
  if (type == k_hideableCellType) {
    return &m_hideableCell;
  }
  assert(index >= 0 && index < k_numberOfCalculationCells);
  return &m_calculationCells[index];
}

int CalculationController::reusableCellCount(int type) {
  if (type == k_standardCalculationTitleCellType) {
    return k_maxNumberOfDisplayableRows;
  }
  if (type == k_columnTitleCellType) {
    return Store::k_numberOfSeries;
  }
  if (type == k_doubleBufferCalculationCellType) {
    return k_numberOfDoubleCalculationCells;
  }
  if (type == k_hideableCellType) {
    return 1;
  }
  assert(type == k_standardCalculationCellType);
  return k_numberOfCalculationCells;
}

int CalculationController::typeAtLocation(int i, int j) {
  if (i == 0 && j == 0) {
    return k_hideableCellType;
  }
  if (i == 0) {
    return k_standardCalculationTitleCellType;
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

}

