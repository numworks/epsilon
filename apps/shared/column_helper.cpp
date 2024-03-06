#include "column_helper.h"

#include <apps/constant.h>
#include <escher/container.h>
#include <poincare/comparison.h>
#include <poincare/print.h>
#include <poincare/serialization_helper.h>

#include "column_parameter_controller.h"
#include "poincare_helpers.h"

using namespace Escher;
using namespace Poincare;

namespace Shared {

/* ClearColumnHelper */

ClearColumnHelper::ClearColumnHelper()
    : m_confirmPopUpController(Invocation::Builder<ClearColumnHelper>(
          [](ClearColumnHelper *param, void *parent) {
            param->clearSelectedColumn();
            App::app()->modalViewController()->dismissModal();
            param->table()->reloadData();
            return true;
          },
          this)) {}

void ClearColumnHelper::presentClearSelectedColumnPopupIfClearable() {
  int column = table()->selectedColumn();
  if (numberOfElementsInColumn(column) > 0 && isColumnClearable(column)) {
    setClearPopUpContent();
    m_confirmPopUpController.presentModally();
  }
}

void ClearColumnHelper::setClearPopUpContent() {
  char columnNameBuffer[ColumnParameterController::k_titleBufferSize];
  fillColumnName(table()->selectedColumn(), columnNameBuffer);
  m_confirmPopUpController.setMessageWithPlaceholders(
      I18n::Message::ClearColumnConfirmation, columnNameBuffer);
}

/* StoreColumnHelper */

StoreColumnHelper::StoreColumnHelper(Escher::Responder *responder,
                                     Context *parentContext,
                                     ClearColumnHelper *clearColumnHelper)
    : m_clearColumnHelper(clearColumnHelper),
      m_templateController(responder, this),
      m_templateStackController(nullptr, &m_templateController,
                                StackViewController::Style::PurpleWhite),
      m_parentContext(parentContext) {}

bool StoreColumnHelper::switchSelectedColumnHideStatus() {
  int series = selectedSeries();
  bool previousStatus = store()->seriesIsActive(series);
  if (previousStatus) {
    // Any previously valid series can be hidden
    store()->setSeriesHidden(series, true);
    return true;
  } else {
    // Series may still be invalid, in that case nothing happens
    store()->setSeriesHidden(series, false);
    return selectedSeriesIsActive();
  }
}

void StoreColumnHelper::sortSelectedColumn() {
  store()->sortColumn(selectedSeries(),
                      store()->relativeColumn(referencedColumn()));
}

void StoreColumnHelper::displayFormulaInput() {
  int index =
      formulaMemoizationIndex(store()->seriesAtColumn(referencedColumn()),
                              store()->relativeColumn(referencedColumn()));
  if (!memoizedFormula(index).isUninitialized()) {
    fillFormulaInputWithTemplate(memoizedFormula(index));
    return;
  }
  App::app()->displayModalViewController(&m_templateStackController, 0.f, 0.f,
                                         Metric::PopUpMarginsNoBottom);
}

void StoreColumnHelper::fillFormulaInputWithTemplate(Layout templateLayout) {
  constexpr size_t k_sizeOfBuffer = Constant::MaxSerializedExpressionSize;
  char templateString[k_sizeOfBuffer];
  size_t filledLength =
      fillColumnNameFromStore(referencedColumn(), templateString);
  if (filledLength < ClearColumnHelper::k_maxSizeOfColumnName - 1) {
    filledLength += SerializationHelper::CodePoint(
        templateString + filledLength, k_sizeOfBuffer - filledLength, '=');
  }
  if (!templateLayout.isUninitialized()) {
    templateLayout.serializeParsedExpression(
        templateString + filledLength, k_sizeOfBuffer - filledLength, nullptr);
  }
  inputViewController()->setTextBody(templateString);
  inputViewController()->edit(
      Ion::Events::OK, this,
      [](void *context, void *sender) {
        StoreColumnHelper *storeColumnHelper =
            static_cast<StoreColumnHelper *>(context);
        InputViewController *thisInputViewController =
            static_cast<InputViewController *>(sender);
        return storeColumnHelper->fillColumnWithFormula(
            thisInputViewController->textBody());
      },
      [](void *context, void *sender) { return true; });
}

bool StoreColumnHelper::fillColumnWithFormula(const char *text) {
  int column = store()->relativeColumn(referencedColumn());
  int series = store()->seriesAtColumn(referencedColumn());
  Layout formulaLayout;
  FillColumnStatus status =
      privateFillColumnWithFormula(text, &series, &column, &formulaLayout);
  if (status == FillColumnStatus::Success ||
      status == FillColumnStatus::NoDataToStore) {
    if (status == FillColumnStatus::Success) {
      /* We want to update the series only after the expression of the formula
       * is destroyed, to avoid carrying multiple huge lists in the pool. That's
       * why the formula is created in another scope
       * (privateFillColumnWithFormula). */
      if (!store()->updateSeries(series)) {
        App::app()->setFirstResponder(table());
        return false;
      }
      reload();
      memoizeFormula(formulaLayout, formulaMemoizationIndex(series, column));
    }
    App::app()->setFirstResponder(table());
    return true;
  }
  if (status == FillColumnStatus::SyntaxError ||
      status == FillColumnStatus::DataNotSuitable) {
    App::app()->displayWarning(status == FillColumnStatus::SyntaxError
                                   ? I18n::Message::SyntaxError
                                   : I18n::Message::DataNotSuitable);
  }
  return false;
}

int StoreColumnHelper::formulaMemoizationIndex(int series, int column) {
  // Index without ECC which can't be modified
  return DoublePairStore::k_numberOfColumnsPerSeries * series + column;
}

StoreColumnHelper::FillColumnStatus
StoreColumnHelper::privateFillColumnWithFormula(const char *text, int *series,
                                                int *column,
                                                Layout *formulaLayout) {
  StoreContext storeContext(store(), m_parentContext);
  Expression formula = Expression::Parse(text, &storeContext);
  if (formula.isUninitialized()) {
    return FillColumnStatus::SyntaxError;
  }
  if (ComparisonNode::IsBinaryEquality(formula)) {
    bool isValidEquality = false;
    Expression leftOfEqual = formula.childAtIndex(0);
#if 0  // TODO_PCJ
    if (leftOfEqual.type() == ExpressionNode::Type::Symbol) {
      Symbol symbolLeftOfEqual = static_cast<Symbol &>(leftOfEqual);
      if (store()->isColumnName(symbolLeftOfEqual.name(),
                                strlen(symbolLeftOfEqual.name()), series,
                                column)) {
        formula = formula.childAtIndex(1);
        isValidEquality = true;
      }
    }
#else
    assert(false);
#endif
    if (!isValidEquality) {
      return FillColumnStatus::DataNotSuitable;
    }
  }

  // Create the layout before simplifying
  *formulaLayout = formula.createLayout(
      Preferences::SharedPreferences()->displayMode(),
      Preferences::SharedPreferences()->numberOfSignificantDigits(),
      &storeContext);

  PoincareHelpers::CloneAndSimplify(
      &formula, &storeContext,
      {.target = ReductionTarget::SystemForApproximation,
       .symbolicComputation =
           SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined});

  if (formula.isUndefined()) {
    return FillColumnStatus::DataNotSuitable;
  }

  if (formula.recursivelyMatches(
          [](const Expression e) { return e.isRandomList(); }) ||
      formula.type() != ExpressionNode::Type::List) {
    // Sometimes the formula is a list but the reduction failed.
    formula = PoincareHelpers::Approximate<double>(formula, &storeContext);
  }

  if (formula.type() == ExpressionNode::Type::List) {
    bool allChildrenAreUndefined = true;
    int formulaNumberOfChildren = formula.numberOfChildren();
    for (int i = 0; i < formulaNumberOfChildren; i++) {
      if (!formula.childAtIndex(i).isUndefined()) {
        allChildrenAreUndefined = false;
        break;
      }
    }
    if (allChildrenAreUndefined) {
      return FillColumnStatus::DataNotSuitable;
    }
    /* Delay the update of the series so that not too much data exists at the
     * same time in the pool. We might be working with huge lists right now, so
     * it's better to get out of the scope and destroy the list before storing
     * the data of the double pair store in the storage. */
    store()->setList(static_cast<Poincare::List &>(formula), *series, *column,
                     true, true);
    return FillColumnStatus::Success;
  }

  // Formula is not a list: set each cell to the same value
  double evaluation =
      PoincareHelpers::ApproximateToScalar<double>(formula, &storeContext);
  if (std::isnan(evaluation)) {
    return FillColumnStatus::DataNotSuitable;
  }
  int numberOfPairs = store()->numberOfPairsOfSeries(*series);
  if (numberOfPairs == 0) {
    return FillColumnStatus::NoDataToStore;
  }

  // If formula contains a random formula, evaluate it for each pairs.
  bool evaluateForEachPairs = formula.recursivelyMatches(
      [](const Expression e) { return e.isRandomNumber(); });
  for (int j = 0; j < numberOfPairs; j++) {
    store()->set(evaluation, *series, *column, j, true, true);
    if (evaluateForEachPairs) {
      evaluation =
          PoincareHelpers::ApproximateToScalar<double>(formula, &storeContext);
    }
  }
  return FillColumnStatus::Success;
}

size_t StoreColumnHelper::clearPopUpText(int column, char *buffer,
                                         size_t bufferSize) {
  DoublePairStore *store = this->store();
  int series = store->seriesAtColumn(column);
  int relativeColumn = store->relativeColumn(column);
  DoublePairStore::ClearType type = store->clearType(relativeColumn);
  constexpr size_t placeHolderSize = DoublePairStore::k_tableNameLength + 1;
  char placeHolder[placeHolderSize];
  I18n::Message message = I18n::Message::Default;
  if (type == DoublePairStore::ClearType::ClearTable) {
    store->tableName(series, placeHolder, placeHolderSize);
    message = I18n::Message::ClearTableConfirmation;
  } else {
    store->fillColumnName(series, relativeColumn, placeHolder);
    assert(type == DoublePairStore::ClearType::ClearColumn ||
           type == DoublePairStore::ClearType::ResetColumn);
    message = type == DoublePairStore::ClearType::ClearColumn
                  ? I18n::Message::ClearColumnConfirmation
                  : I18n::Message::ResetFreqConfirmation;
  }
  return Poincare::Print::CustomPrintf(buffer, bufferSize,
                                       I18n::translate(message), placeHolder);
}

size_t StoreColumnHelper::clearCellText(int column, char *buffer,
                                        size_t bufferSize) {
  DoublePairStore *store = this->store();
  int series = store->seriesAtColumn(column);
  int relativeColumn = store->relativeColumn(column);
  DoublePairStore::ClearType type = store->clearType(relativeColumn);
  if (type == DoublePairStore::ClearType::ClearTable) {
    constexpr size_t placeHolderSize = DoublePairStore::k_tableNameLength + 1;
    char placeHolder[placeHolderSize];
    store->tableName(series, placeHolder, placeHolderSize);
    return Poincare::Print::CustomPrintf(
        buffer, bufferSize, I18n::translate(I18n::Message::ClearTable),
        placeHolder);
  } else {
    assert(type == DoublePairStore::ClearType::ClearColumn ||
           type == DoublePairStore::ClearType::ResetColumn);
    I18n::Message message = type == DoublePairStore::ClearType::ClearColumn
                                ? I18n::Message::ClearColumn
                                : I18n::Message::ResetFrequencies;
    return Poincare::Print::CustomPrintf(buffer, bufferSize,
                                         I18n::translate(message));
  }
}

void StoreColumnHelper::reloadSeriesVisibleCells(int series,
                                                 int relativeColumn) {
  // Reload visible cells of the series and, if not -1, relative column
  int nbOfColumns = table()->totalNumberOfColumns();
  for (int i = 0; i < nbOfColumns; i++) {
    if (store()->seriesAtColumn(i) == series &&
        (relativeColumn == -1 ||
         relativeColumn == store()->relativeColumn(i))) {
      table()->reloadVisibleCellsAtColumn(i);
    }
  }
}

}  // namespace Shared
