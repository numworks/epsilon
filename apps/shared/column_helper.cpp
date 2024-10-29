#include "column_helper.h"

#include <apps/constant.h>
#include <escher/container.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/print.h>

#include "column_parameter_controller.h"
#include "poincare_helpers.h"

using namespace Escher;
using namespace Poincare;

namespace Shared {

/* ClearColumnHelper */

ClearColumnHelper::ClearColumnHelper()
    : m_confirmPopUpController(Invocation::Builder<ClearColumnHelper>(
          [](ClearColumnHelper* param, void* parent) {
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

StoreColumnHelper::StoreColumnHelper(Escher::Responder* responder,
                                     Context* parentContext,
                                     ClearColumnHelper* clearColumnHelper)
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
    fillFormulaInputWithFormula(memoizedFormula(index));
    return;
  }
  App::app()->displayModalViewController(&m_templateStackController, 0.f, 0.f,
                                         Metric::PopUpMarginsNoBottom);
}

void StoreColumnHelper::fillFormulaInputWithTemplate(
    const Layout& templateLayout) {
  constexpr size_t k_bufferSize = ClearColumnHelper::k_maxSizeOfColumnName;
  char formulaText[k_bufferSize];
  size_t length = fillColumnNameFromStore(referencedColumn(), formulaText);
  if (length < ClearColumnHelper::k_maxSizeOfColumnName - 1) {
    length += SerializationHelper::CodePoint(formulaText + length,
                                             k_bufferSize - length, '=');
  }
  Layout formulaLayout = Layout::Parse(formulaText);
  if (!templateLayout.isUninitialized()) {
    formulaLayout = Layout::Concatenate(formulaLayout, templateLayout);
  }
  fillFormulaInputWithFormula(formulaLayout);
}

void StoreColumnHelper::fillFormulaInputWithFormula(
    const Layout& formulaLayout) {
  inputViewController()->setLayout(formulaLayout);
  inputViewController()->edit(
      Ion::Events::OK, this,
      [](void* context, void* sender) {
        StoreColumnHelper* storeColumnHelper =
            static_cast<StoreColumnHelper*>(context);
        InputViewController* thisInputViewController =
            static_cast<InputViewController*>(sender);
        return storeColumnHelper->fillColumnWithFormula(
            thisInputViewController->layout());
      },
      [](void* context, void* sender) { return true; });
}

bool StoreColumnHelper::fillColumnWithFormula(
    const Poincare::Layout& formulaLayout) {
  int column = store()->relativeColumn(referencedColumn());
  int series = store()->seriesAtColumn(referencedColumn());
  FillColumnStatus status =
      privateFillColumnWithFormula(formulaLayout, &series, &column);
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
StoreColumnHelper::privateFillColumnWithFormula(const Layout& formulaLayout,
                                                int* series, int* column) {
  StoreContext storeContext(store(), m_parentContext);
  UserExpression formula =
      UserExpression::Parse(formulaLayout.tree(), &storeContext);
  if (formula.isUninitialized()) {
    return FillColumnStatus::SyntaxError;
  }
  if (formula.isEquality()) {
    bool isValidEquality = false;
    const UserExpression leftOfEqual = formula.cloneChildAtIndex(0);
    if (leftOfEqual.isUserSymbol()) {
      const Symbol symbolLeftOfEqual = static_cast<const Symbol&>(leftOfEqual);
      if (store()->isColumnName(symbolLeftOfEqual.name(),
                                strlen(symbolLeftOfEqual.name()), series,
                                column)) {
        formula = formula.cloneChildAtIndex(1);
        isValidEquality = true;
      }
    }
    if (!isValidEquality) {
      return FillColumnStatus::DataNotSuitable;
    }
  }

  PoincareHelpers::CloneAndSimplify(
      &formula, &storeContext,
      {.target = ReductionTarget::SystemForApproximation,
       .symbolicComputation = SymbolicComputation::ReplaceAllSymbols});

  if (formula.isUndefined()) {
    return FillColumnStatus::DataNotSuitable;
  }

  if (formula.recursivelyMatches(
          [](const NewExpression e) { return e.isRandomList(); }) ||
      !formula.isList()) {
    // Sometimes the formula is a list but the reduction failed.
    formula = PoincareHelpers::Approximate<double>(formula, &storeContext);
  }

  if (formula.isList()) {
    bool allChildrenAreUndefined = true;
    int formulaNumberOfChildren =
        static_cast<List&>(formula).numberOfChildren();
    for (int i = 0; i < formulaNumberOfChildren; i++) {
      if (!formula.cloneChildAtIndex(i).isUndefined()) {
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
    store()->setList(static_cast<Poincare::List&>(formula), *series, *column,
                     true, true);
    return FillColumnStatus::Success;
  }

  // Formula is not a list: set each cell to the same value
  double evaluation = formula.approximateUserExpressionToScalar<double>();
  if (std::isnan(evaluation)) {
    return FillColumnStatus::DataNotSuitable;
  }
  int numberOfPairs = store()->numberOfPairsOfSeries(*series);
  if (numberOfPairs == 0) {
    return FillColumnStatus::NoDataToStore;
  }

  // If formula contains a random formula, evaluate it for each pairs.
  bool evaluateForEachPairs = formula.recursivelyMatches(
      [](const NewExpression e) { return e.isRandomNumber(); });
  for (int j = 0; j < numberOfPairs; j++) {
    store()->set(evaluation, *series, *column, j, true, true);
    if (evaluateForEachPairs) {
      evaluation = formula.approximateUserExpressionToScalar<double>();
    }
  }
  return FillColumnStatus::Success;
}

size_t StoreColumnHelper::clearPopUpText(int column, char* buffer,
                                         size_t bufferSize) {
  DoublePairStore* store = this->store();
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

size_t StoreColumnHelper::clearCellText(int column, char* buffer,
                                        size_t bufferSize) {
  DoublePairStore* store = this->store();
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
