#include "values_controller.h"

#include <apps/i18n.h>
#include <apps/shared/poincare_helpers.h>
#include <assert.h>
#include <poincare/old/sequence.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/sum.h>

#include <cmath>

#include "../app.h"

using namespace Poincare;
using namespace Escher;

namespace Sequence {

ValuesController::ValuesController(Responder *parentResponder,
                                   ButtonRowController *header)
    : Shared::ValuesController(parentResponder, header),
      m_intervalParameterController(this),
      m_sequenceColumnParameterController(this),
      m_sumColumnParameterController(this),
      m_setIntervalButton(
          this, I18n::Message::IntervalSet,
          Invocation::Builder<ValuesController>(
              [](ValuesController *valuesController, void *sender) {
                StackViewController *stack =
                    ((StackViewController *)
                         valuesController->stackController());
                IntervalParameterController *controller =
                    valuesController->intervalParameterController();
                controller->setInterval(valuesController->intervalAtColumn(
                    valuesController->selectedColumn()));
                App::app()->snapshot()->setIntervalModifiedByUser(true);
                /* No need to change Nstart/Nend messages because they are the
                 * only messages used and we set them in
                 * ValuesController::ValuesController(...) */
                stack->push(controller);
                return true;
              },
              this),
          k_cellFont),
      m_hasAtLeastOneSumColumn(false) {
  setupSelectableTableViewAndCells();
  setDefaultStartEndMessages();
  initValueCells();
}

KDCoordinate ValuesController::computeSizeAtIndex(int i) {
  return (i == 0 && m_hasAtLeastOneSumColumn)
             ? k_sumLayoutHeight
             : RegularTableSize1DManager::computeSizeAtIndex(i);
}

KDCoordinate ValuesController::computeCumulatedSizeBeforeIndex(
    int i, KDCoordinate defaultSize) {
  return RegularTableSize1DManager::computeCumulatedSizeBeforeIndex(
             i, defaultSize) +
         (i > 0 && m_hasAtLeastOneSumColumn) *
             (k_sumLayoutHeight - defaultSize);
}

int ValuesController::computeIndexAfterCumulatedSize(KDCoordinate offset,
                                                     KDCoordinate defaultSize) {
  return RegularTableSize1DManager::computeIndexAfterCumulatedSize(
      offset - (offset >= defaultSize && m_hasAtLeastOneSumColumn) *
                   (k_sumLayoutHeight - defaultSize),
      defaultSize);
}

// ColumnHelper

size_t ValuesController::fillColumnName(int column, char *buffer) {
  if (typeAtLocation(column, 0) != k_functionTitleCellType) {
    return Shared::ValuesController::fillColumnName(column, buffer);
  }
  bool isSumColumn = false;
  Ion::Storage::Record record = recordAtColumn(column, &isSumColumn);
  Shared::ExpiringPointer<Shared::Sequence> seq =
      functionStore()->modelForRecord(record);
  if (!isSumColumn) {
    return seq->nameWithArgument(buffer, k_maxSizeOfColumnName);
  }
  size_t sigmaLength = SerializationHelper::CodePoint(
      buffer, k_maxSizeOfColumnName, UCodePointNArySummation);
  return sigmaLength +
         seq->name(buffer + sigmaLength, k_maxSizeOfColumnName - sigmaLength);
}

// EditableCellTableViewController

bool ValuesController::setDataAtLocation(double floatBody, int column,
                                         int row) {
  assert(checkDataAtLocation(floatBody, column, row));
  return Shared::ValuesController::setDataAtLocation(std::round(floatBody),
                                                     column, row);
}

// Shared::ValuesController

Ion::Storage::Record ValuesController::recordAtColumn(int i,
                                                      bool *isSumColumn) {
  assert(typeAtLocation(i, 0) == k_functionTitleCellType);
  int numberOfActiveSequences = functionStore()->numberOfActiveFunctions();
  assert(numberOfAbscissaColumns() == 1);
  int currentColumn = numberOfAbscissaColumns();
  for (int k = 0; k < numberOfActiveSequences; k++) {
    Ion::Storage::Record record = functionStore()->activeRecordAtIndex(k);
    Shared::ExpiringPointer<Shared::Sequence> seq =
        functionStore()->modelForRecord(record);
    int numberOfColumnsForCurrentRecord = 1 + seq->displaySum();
    if (currentColumn <= i &&
        i < currentColumn + numberOfColumnsForCurrentRecord) {
      if (isSumColumn) {
        *isSumColumn = i == currentColumn + 1;
      }
      return record;
    }
    currentColumn += numberOfColumnsForCurrentRecord;
  }
  assert(false);  // Out of bounds
  return nullptr;
}

void ValuesController::updateNumberOfColumns() {
  m_numberOfColumns = numberOfAbscissaColumns();
  bool previousHasAtLeastOneSumColumn = m_hasAtLeastOneSumColumn;
  m_hasAtLeastOneSumColumn = false;
  int numberOfActiveSequences = functionStore()->numberOfActiveFunctions();
  for (int k = 0; k < numberOfActiveSequences; k++) {
    Shared::ExpiringPointer<Shared::Sequence> seq =
        functionStore()->modelForRecord(
            functionStore()->activeRecordAtIndex(k));
    bool displaySum = seq->displaySum();
    m_numberOfColumns += 1 + displaySum;
    m_hasAtLeastOneSumColumn |= displaySum;
  }
  if (previousHasAtLeastOneSumColumn != m_hasAtLeastOneSumColumn) {
    m_prefacedTwiceTableView.resetSizeAndOffsetMemoization();
  }
}

Layout *ValuesController::memoizedLayoutAtIndex(int i) {
  assert(i >= 0 && i < k_maxNumberOfDisplayableCells);
  return &m_memoizedLayouts[i];
}

Layout ValuesController::functionTitleLayout(int column) {
  Preferences *preferences = Preferences::SharedPreferences();
  bool isSumColumn = false;
  Shared::Sequence *sequence =
      functionStore()->modelForRecord(recordAtColumn(column, &isSumColumn));
  if (!isSumColumn) {
    return sequence->nameLayout();
  }
  constexpr const char *k_variable = "k";
  constexpr const char *n_variable = "n";
  UserExpression sumExpression =
      Sum::Builder(Poincare::Sequence::Builder(
                       sequence->fullName(), 1,
                       Symbol::Builder(k_variable, strlen(k_variable))),
                   Symbol::Builder(k_variable, strlen(k_variable)),
                   NewExpression::Builder(sequence->initialRank()),
                   Symbol::Builder(n_variable, strlen(n_variable)));
  return sumExpression.createLayout(preferences->displayMode(),
                                    preferences->numberOfSignificantDigits(),
                                    nullptr);
}

void ValuesController::createMemoizedLayout(int column, int row, int index) {
  Preferences *preferences = Preferences::SharedPreferences();
  double abscissa = intervalAtColumn(column)->element(
      row - 1);  // Subtract the title row from row to get the element index
  bool isSumColumn = false;
  Context *context = App::app()->localContext();
  Shared::ExpiringPointer<Shared::Sequence> sequence =
      functionStore()->modelForRecord(recordAtColumn(column, &isSumColumn));
  UserExpression result;
  if (isSumColumn) {
    result =
        sequence->sumBetweenBounds(sequence->initialRank(), abscissa, context);
  } else {
    Coordinate2D<double> xy =
        sequence->evaluateXYAtParameter(abscissa, context);
    result = NewExpression::Builder<double>(xy.y());
  }
  *memoizedLayoutAtIndex(index) =
      result.createLayout(preferences->displayMode(),
                          preferences->numberOfSignificantDigits(), context);
}

Shared::Interval *ValuesController::intervalAtColumn(int column) {
  return App::app()->interval();
}

Shared::ExpressionFunctionTitleCell *ValuesController::functionTitleCells(
    int j) {
  assert(j >= 0 && j < k_maxNumberOfDisplayableColumns);
  return &m_sequenceTitleCells[j];
}

Escher::EvenOddExpressionCell *ValuesController::valueCells(int j) {
  assert(j >= 0 && j < k_maxNumberOfDisplayableCells);
  return &m_valueCells[j];
}

Escher::AbstractEvenOddEditableTextCell *ValuesController::abscissaCells(
    int j) {
  assert(j >= 0 && j < k_maxNumberOfDisplayableRows);
  return &m_abscissaCells[j];
}

Escher::EvenOddMessageTextCell *ValuesController::abscissaTitleCells(int j) {
  assert(j >= 0 && j < abscissaTitleCellsCount());
  return &m_abscissaTitleCell;
}

void ValuesController::setDefaultStartEndMessages() {
  m_intervalParameterController.setStartEndMessages(I18n::Message::NStart,
                                                    I18n::Message::NEnd);
}

Shared::ColumnParameterController *
ValuesController::sequenceColumnParameterController() {
  int col = selectedColumn();
  assert(col > 0);
  bool isSumColumn = false;
  Ion::Storage::Record currentRecord = recordAtColumn(col, &isSumColumn);
  if (isSumColumn) {
    m_sumColumnParameterController.setRecord(currentRecord);
    return &m_sumColumnParameterController;
  }
  m_sequenceColumnParameterController.setRecord(currentRecord);
  return &m_sequenceColumnParameterController;
}

}  // namespace Sequence
