#include "column_helper.h"
#include "column_parameter_controller.h"
#include "poincare_helpers.h"
#include <escher/container.h>
#include <poincare/print.h>

using namespace Escher;
using namespace Poincare;

namespace Shared {

/* ClearColumnHelper */

ClearColumnHelper::ClearColumnHelper() :
  m_confirmPopUpController(Invocation([](void * context, void * parent){
    ClearColumnHelper * param = static_cast<ClearColumnHelper *>(context);
    param->clearSelectedColumn();
    Container::activeApp()->dismissModalViewController();
    param->table()->reloadData();
    return true;
  }, this))
{}

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
  m_confirmPopUpController.setMessageWithPlaceholder(I18n::Message::ClearColumnConfirmation, columnNameBuffer);
}

int ClearColumnHelper::fillColumnNameWithMessage(char * buffer, I18n::Message message) {
  return Poincare::Print::customPrintf(buffer, k_maxSizeOfColumnName, I18n::translate(message));
}

/* StoreColumnHelper */

bool displayNotSuitableWarning() {
  Container::activeApp()->displayWarning(I18n::Message::DataNotSuitable);
  return false;
}

StoreColumnHelper::StoreColumnHelper(Escher::Responder * responder, DoublePairStore * store, Context * parentContext, ClearColumnHelper * clearColumnHelper) :
  m_clearColumnHelper(clearColumnHelper),
  m_templateController(responder, this),
  m_templateStackController(nullptr, &m_templateController, StackViewController::Style::PurpleWhite),
  m_storeContext(store, parentContext)
{
}

bool StoreColumnHelper::switchSelectedColumnHideStatus() {
  int series = selectedSeries();
  bool previousStatus = store()->seriesIsValid(series);
  if (previousStatus) {
    // Any previously valid series can be hidden
    store()->hideSeries(series);
    return true;
  } else {
    // Series may still be invalid, in that case nothing happens
    store()->updateSeriesValidity(series);
    return selectedSeriesIsValid();
  }
}

void StoreColumnHelper::sortSelectedColumn() {
  store()->sortColumn(selectedSeries(), store()->relativeColumnIndex(referencedColumn()));
}

void StoreColumnHelper::displayFormulaInput() {
  Container::activeApp()->displayModalViewController(&m_templateStackController, 0.f, 0.f, Metric::PopUpTopMargin, Metric::PopUpRightMargin, 0, Metric::PopUpLeftMargin);
}

void StoreColumnHelper::fillFormulaInputWithTemplate(Layout templateLayout) {
  constexpr int k_sizeOfBuffer = DoublePairStore::k_columnNamesLength + 1 + FormulaTemplateMenuController::k_maxSizeOfTemplateText;
  char templateString[k_sizeOfBuffer];
  int filledLength = fillColumnNameFromStore(referencedColumn(), templateString);
  if (filledLength < ClearColumnHelper::k_maxSizeOfColumnName - 1) {
    templateString[filledLength] = '=';
    templateString[filledLength + 1] = 0;
  }
  if (!templateLayout.isUninitialized()) {
    templateLayout.serializeParsedExpression(templateString + filledLength + 1, k_sizeOfBuffer - filledLength - 1, nullptr);
  }
  inputViewController()->setTextBody(templateString);
  inputViewController()->edit(
      Ion::Events::OK,
      this,
      [](void * context, void * sender) {
        StoreColumnHelper * storeColumnHelper = static_cast<StoreColumnHelper *>(context);
        InputViewController * thisInputViewController = static_cast<InputViewController *>(sender);
        return storeColumnHelper->createExpressionForFillingColumnWithFormula(thisInputViewController->textBody());
      }, [](void * context, void * sender) {
        return true;
      });
}

bool StoreColumnHelper::createExpressionForFillingColumnWithFormula(const char * text) {
  Expression expression = Expression::Parse(text, &m_storeContext);
  if (expression.isUninitialized()) {
    Container::activeApp()->displayWarning(I18n::Message::SyntaxError);
    return false;
  }
  if (fillColumnWithFormula(expression)) {
    Container::activeApp()->setFirstResponder(table());
    return true;
  }
  return false;
}

bool StoreColumnHelper::fillColumnWithFormula(Expression formula) {
  int columnToFill = store()->relativeColumnIndex(referencedColumn());
  int seriesToFill = store()->seriesAtColumn(referencedColumn());
  if (formula.type() == ExpressionNode::Type::Equal) {
    bool isValidEquality = false;
    Expression leftOfEqual = formula.childAtIndex(0);
    if (leftOfEqual.type() == ExpressionNode::Type::Symbol) {
      Symbol symbolLeftOfEqual = static_cast<Symbol &>(leftOfEqual);
      if (store()->isColumnName(symbolLeftOfEqual.name(), strlen(symbolLeftOfEqual.name()), &seriesToFill, &columnToFill)) {
        formula = formula.childAtIndex(1);
        isValidEquality = true;
      }
    }
    if (!isValidEquality) {
      return displayNotSuitableWarning();
    }
  }
  PoincareHelpers::CloneAndSimplify(&formula, &m_storeContext, ExpressionNode::ReductionTarget::SystemForApproximation, ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined);
  if (formula.isUndefined()) {
      return displayNotSuitableWarning();
  }
  if (formula.type() == ExpressionNode::Type::List) {
    bool allChildrenAreUndefined = true;
    for (int i = 0; i < formula.numberOfChildren(); i++) {
      if (!formula.childAtIndex(i).isUndefined()) {
        allChildrenAreUndefined = false;
        break;
      }
    }
    if (allChildrenAreUndefined) {
      return displayNotSuitableWarning();
    }
    store()->setList(static_cast<List &>(formula), seriesToFill, columnToFill);
    table()->reloadData();
    return true;
  }
  // If formula contains a random formula, evaluate it for each pairs.
  bool evaluateForEachPairs = formula.hasExpression([](const Expression e, const void * context) { return e.isRandom(); }, nullptr);
  double evaluation = PoincareHelpers::ApproximateToScalar<double>(formula, &m_storeContext);
  if (std::isnan(evaluation)) {
    return displayNotSuitableWarning();
  }
  for (int j = 0; j < store()->numberOfPairsOfSeries(seriesToFill); j++) {
    store()->set(evaluation, seriesToFill, columnToFill, j);
    if (evaluateForEachPairs) {
      evaluation = PoincareHelpers::ApproximateToScalar<double>(formula, &m_storeContext);
    }
  }
  reloadSeriesVisibleCells(selectedSeries());
  return true;
}

void StoreColumnHelper::reloadSeriesVisibleCells(int series, int relativeColumn) {
  // Reload visible cells of the series and, if not -1, relative column
  int nbOfColumns = DoublePairStore::k_numberOfColumnsPerSeries * DoublePairStore::k_numberOfSeries;
  for (int i = 0; i < nbOfColumns; i++) {
    if (store()->seriesAtColumn(i) == series && (relativeColumn == -1 || relativeColumn == store()->relativeColumnIndex(i))) {
      table()->reloadVisibleCellsAtColumn(i);
    }
  }
}

}
