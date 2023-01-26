#include "values_controller.h"
#include <assert.h>
#include <cmath>
#include "../../shared/poincare_helpers.h"
#include "../app.h"
#include <apps/i18n.h>
#include <poincare/based_integer.h>
#include <poincare/sequence.h>
#include <poincare/sum.h>

using namespace Poincare;
using namespace Escher;

namespace Sequence {

ValuesController::ValuesController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, ButtonRowController * header) :
  Shared::ValuesController(parentResponder, header),
  m_intervalParameterController(this, inputEventHandlerDelegate),
  m_setIntervalButton(this, I18n::Message::IntervalSet, Invocation::Builder<ValuesController>([](ValuesController * valuesController, void * sender) {
    StackViewController * stack = ((StackViewController *)valuesController->stackController());
    IntervalParameterController * controller = valuesController->intervalParameterController();
    controller->setInterval(valuesController->intervalAtColumn(valuesController->selectedColumn()));
    App::app()->snapshot()->setIntervalModifiedByUser(true);
    /* No need to change Nstart/Nend messages because they are the only messages
     * used and we set them in ValuesController::ValuesController(...) */
    stack->push(controller);
    return true;
  }, this), k_cellFont)
{
  setupSelectableTableViewAndCells(inputEventHandlerDelegate);
  setDefaultStartEndMessages();
  initValueCells();
}

// TableSize1DManager (height)
bool ValuesController::hasAtLeastOneSumColumn() {
  return numberOfColumns() > (1 + functionStore()->numberOfActiveFunctions());
}

KDCoordinate ValuesController::computeSizeAtIndex(int i) {
  return
    (i == 0 && hasAtLeastOneSumColumn()) ?
      k_sumLayoutHeight :
      RegularTableSize1DManager::computeSizeAtIndex(i);
}

KDCoordinate ValuesController::computeCumulatedSizeBeforeIndex(int i, KDCoordinate defaultSize) {
  return
    RegularTableSize1DManager::computeCumulatedSizeBeforeIndex(i, defaultSize) +
    (i > 0 && hasAtLeastOneSumColumn()) * (k_sumLayoutHeight - defaultSize);
}

int ValuesController::computeIndexAfterCumulatedSize(KDCoordinate offset, KDCoordinate defaultSize) {
  return
    RegularTableSize1DManager::computeIndexAfterCumulatedSize(
      offset - (offset >= defaultSize && hasAtLeastOneSumColumn()) * (k_sumLayoutHeight - defaultSize),
      defaultSize);
}

// ColumnHelper

int ValuesController::fillColumnName(int columnIndex, char * buffer) {
  /* The column names U_n, V_n, etc. are implemented as layout for now (see setTitleCellText of this file)
   * Since there is no column parameters for these column, the fillColumnName is not yet implemented. */
  if (typeAtLocation(columnIndex, 0) == k_functionTitleCellType) {
    return functionTitleLayout(columnIndex).serializeParsedExpression(buffer, k_maxSizeOfColumnName, textFieldDelegateApp()->localContext());
  }
  return Shared::ValuesController::fillColumnName(columnIndex, buffer);
}

// EditableCellTableViewController

bool ValuesController::setDataAtLocation(double floatBody, int columnIndex, int rowIndex) {
  assert(checkDataAtLocation(floatBody, columnIndex, rowIndex));
  return Shared::ValuesController::setDataAtLocation(std::round(floatBody), columnIndex, rowIndex);
}

// Shared::ValuesController

Ion::Storage::Record ValuesController::recordAtColumn(int i, bool * isSumColumn) {
  assert(typeAtLocation(i, 0) == k_functionTitleCellType);
  int numberOfActiveSequences = functionStore()->numberOfActiveFunctions();
  assert(numberOfAbscissaColumns() == 1);
  int currentColumnIndex = numberOfAbscissaColumns();
  for (int k = 0; k < numberOfActiveSequences; k++) {
    Ion::Storage::Record record = functionStore()->activeRecordAtIndex(k);
    Shared::ExpiringPointer<Shared::Sequence> seq = functionStore()->modelForRecord(record);
    int numberOfColumnsForCurrentRecord = 1 + seq->displaySum();
    if (currentColumnIndex <= i && i < currentColumnIndex + numberOfColumnsForCurrentRecord) {
      if (isSumColumn) {
        *isSumColumn = i == currentColumnIndex + 1;
      }
      return record;
    }
    currentColumnIndex += numberOfColumnsForCurrentRecord;
  }
  assert(false); // Out of bounds
  return nullptr;
}

void ValuesController::updateNumberOfColumns() const {
  m_numberOfColumns = numberOfAbscissaColumns();
  int numberOfActiveSequences = functionStore()->numberOfActiveFunctions();
  for (int k = 0; k < numberOfActiveSequences; k++) {
    Shared::ExpiringPointer<Shared::Sequence> seq = functionStore()->modelForRecord(functionStore()->activeRecordAtIndex(k));
    m_numberOfColumns += 1 + seq->displaySum();
  }
}

Layout * ValuesController::memoizedLayoutAtIndex(int i) {
  assert(i >= 0 && i < k_maxNumberOfDisplayableCells);
  return &m_memoizedLayouts[i];
}

Layout ValuesController::functionTitleLayout(int columnIndex, bool forceShortVersion) {
  bool isSumColumn = false;
  Shared::Sequence * sequence = functionStore()->modelForRecord(recordAtColumn(columnIndex, &isSumColumn));
  if (!isSumColumn) {
    return sequence->nameLayout();
  }
  constexpr const char * k_variable = "k";
  constexpr const char * n_variable = "n";
  Expression sumExpression =
    Sum::Builder(
      Poincare::Sequence::Builder(sequence->fullName(), strlen(sequence->fullName()), Symbol::Builder(k_variable, strlen(k_variable))),
      Symbol::Builder(k_variable, strlen(k_variable)),
      BasedInteger::Builder(sequence->initialRank()),
      Symbol::Builder(n_variable, strlen(n_variable)));
  return sumExpression.createLayout(Preferences::sharedPreferences->displayMode(), Preferences::sharedPreferences->numberOfSignificantDigits(), nullptr);
}

void ValuesController::createMemoizedLayout(int column, int row, int index) {
  double abscissa = intervalAtColumn(column)->element(row-1); // Subtract the title row from row to get the element index
  bool isSumColumn = false;
  Context * context = textFieldDelegateApp()->localContext();
  Shared::ExpiringPointer<Shared::Sequence> sequence = functionStore()->modelForRecord(recordAtColumn(column, &isSumColumn));
  Expression result;
  if (isSumColumn) {
    result = sequence->sumBetweenBounds(sequence->initialRank(), abscissa, context);
  } else {
    Coordinate2D<double> xy = sequence->evaluateXYAtParameter(abscissa, context);
    result = Float<double>::Builder(xy.x2());
  }
  *memoizedLayoutAtIndex(index) = result.createLayout(Preferences::PrintFloatMode::Decimal, Preferences::VeryLargeNumberOfSignificantDigits, context);
}

Shared::Interval * ValuesController::intervalAtColumn(int columnIndex) {
  return App::app()->interval();
}

Shared::ExpressionFunctionTitleCell * ValuesController::functionTitleCells(int j) {
  assert(j >= 0 && j < k_maxNumberOfDisplayableColumns);
  return &m_sequenceTitleCells[j];
}

Escher::EvenOddExpressionCell * ValuesController::valueCells(int j) {
  assert(j >= 0 && j < k_maxNumberOfDisplayableCells);
  return &m_valueCells[j];
}

Escher::EvenOddEditableTextCell * ValuesController::abscissaCells(int j) {
  assert (j >= 0 && j < k_maxNumberOfDisplayableRows);
  return &m_abscissaCells[j];
}

Escher::EvenOddMessageTextCell * ValuesController::abscissaTitleCells(int j) {
  assert (j >= 0 && j < abscissaTitleCellsCount());
  return &m_abscissaTitleCell;
}

void ValuesController::setDefaultStartEndMessages() {
  m_intervalParameterController.setStartEndMessages(I18n::Message::NStart, I18n::Message::NEnd);
}

}
