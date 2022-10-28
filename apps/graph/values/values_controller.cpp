#include "values_controller.h"
#include <assert.h>
#include <escher/clipboard.h>
#include <poincare/circuit_breaker_checkpoint.h>
#include <poincare/decimal.h>
#include <poincare/layout_helper.h>
#include <poincare/matrix_layout.h>
#include <poincare/serialization_helper.h>
#include <poincare/square_bracket_pair_layout.h>
#include <poincare/string_layout.h>
#include <apps/shared/poincare_helpers.h>
#include <apps/shared/utils.h>
#include <apps/constant.h>
#include "../app.h"

using namespace Shared;
using namespace Poincare;
using namespace Escher;

namespace Graph {

// Constructors

ValuesController::ValuesController(Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, ButtonRowController * header, FunctionColumnParameterController * functionParameterController) :
  Shared::ValuesController(parentResponder, header),
  m_functionParameterController(functionParameterController),
  m_intervalParameterController(this, inputEventHandlerDelegate),
  m_derivativeParameterController(this),
  m_setIntervalButton(this, I18n::Message::IntervalSet, Invocation([](void * context, void * sender) {
    ValuesController * valuesController = (ValuesController *) context;
    StackViewController * stack = ((StackViewController *)valuesController->stackController());
    IntervalParameterSelectorController * intervalSelectorController = valuesController->intervalParameterSelectorController();
    if (intervalSelectorController->numberOfRows() == 1) {
      IntervalParameterController * intervalController = valuesController->intervalParameterController();
      intervalController->setInterval(valuesController->intervalAtColumn(0));
      int i = 1;
      intervalSelectorController->setStartEndMessages(intervalController, valuesController->symbolTypeAtColumn(&i));
      stack->push(intervalController);
      return true;
    }
    stack->push(intervalSelectorController);
    return true;
  }, this), k_cellFont),
  m_exactValuesButton(this, I18n::Message::ExactResults, Invocation([](void * context, void * sender) {
    ValuesController * valuesController = (ValuesController *) context;
    valuesController->exactValuesButtonAction();
    return true;
  }, this), &m_exactValuesDotView, k_cellFont),
  m_widthManager(this),
  m_heightManager(this),
  m_exactValuesAreActivated(false)
{
  initValueCells();
  m_exactValuesButton.setState(m_exactValuesAreActivated);
  setupSelectableTableViewAndCells(inputEventHandlerDelegate);
}

// TableViewDataSource

KDSize ValuesController::ApproximatedParametricCellSize() {
  KDSize layoutSize = SquareBracketPairLayoutNode::SizeGivenChildSize(KDSize(
    PrintFloat::glyphLengthForFloatWithPrecision(::Preferences::VeryLargeNumberOfSignificantDigits) * KDFont::GlyphWidth(k_cellFont),
    2 * KDFont::GlyphHeight(k_cellFont) + GridLayoutNode::k_gridEntryMargin));
  return layoutSize + KDSize(Metric::SmallCellMargin * 2, Metric::SmallCellMargin * 2);
}

KDSize ValuesController::CellSizeWithLayout(Layout l) {
  EvenOddExpressionCell tempCell;
  tempCell.setFont(k_cellFont);
  tempCell.setLayout(l);
  return tempCell.minimalSizeForOptimalDisplay() + KDSize(Metric::SmallCellMargin * 2, Metric::SmallCellMargin * 2);
}

KDCoordinate ValuesController::nonMemoizedColumnWidth(int i) {
  KDCoordinate columnWidth;
  KDCoordinate maxColumnWidth = MaxColumnWidth();
  int tempI = i;
  ContinuousFunctionProperties::SymbolType symbol = symbolTypeAtColumn(&tempI);
  if (tempI > 0 && symbol == ContinuousFunctionProperties::SymbolType::T) {
    // Default width is larger for parametric functions
    columnWidth = ApproximatedParametricCellSize().width();
  } else {
    columnWidth = Shared::ValuesController::defaultColumnWidth();
  }
  if (typeAtLocation(i, 0) == k_functionTitleCellType) {
    columnWidth = std::min(maxColumnWidth, std::max(CellSizeWithLayout(functionTitleLayout(i)).width(), columnWidth));
  }
  if (!m_exactValuesAreActivated) {
    // Width is constant when displaying approximations
    return columnWidth;
  }
  int nRows = numberOfElementsInColumn(i) + 1;
  for (int j = 0; j < nRows; j++) {
    if (typeAtLocation(i, j) == k_notEditableValueCellType) {
      Layout l = memoizedLayoutForCell(i, j);
      assert(!l.isUninitialized());
      columnWidth = std::max(CellSizeWithLayout(l).width(), columnWidth);
      if (columnWidth > maxColumnWidth) {
        return maxColumnWidth;
      }
    }
  }
  return columnWidth;
}

KDCoordinate ValuesController::nonMemoizedRowHeight(int j) {
  KDCoordinate rowHeight = Shared::ValuesController::defaultRowHeight();
  KDCoordinate maxRowHeight = MaxRowHeight();
  int nColumns = numberOfColumns();
  if (j == 0) {
    for (int i = 0; i < nColumns; i++) {
      if (typeAtLocation(i, 0) == k_functionTitleCellType) {
        rowHeight = std::max(CellSizeWithLayout(functionTitleLayout(i)).height(), rowHeight);
      }
      if (rowHeight > maxRowHeight) {
        return maxRowHeight;
      }
    }
    return rowHeight;
  }
  for (int i = 0; i < nColumns; i++) {
    int tempI = i;
    ContinuousFunctionProperties::SymbolType symbol = symbolTypeAtColumn(&tempI);
    if (!m_exactValuesAreActivated) {
     if (symbol != ContinuousFunctionProperties::SymbolType::T || j >= numberOfElementsInColumn(i) + 1) {
      /* Height is constant when exact result is not displayed and
       * either there is no parametric function or it's the last row
       * of the column. */
      continue;
     } else {
      return ApproximatedParametricCellSize().height();
     }
    }
    if (typeAtLocation(i, j) == k_notEditableValueCellType && j < numberOfElementsInColumn(i) + 1) {
      assert(m_exactValuesAreActivated);
      Layout l = memoizedLayoutForCell(i, j);
      assert(!l.isUninitialized());
      rowHeight = std::max(CellSizeWithLayout(l).height(), rowHeight);
      if (rowHeight > maxRowHeight) {
        return maxRowHeight;
      }
    }
  }
  return rowHeight;
}

void ValuesController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  // Handle hidden cells
  int typeAtLoc = typeAtLocation(i,j);
  if (typeAtLoc == k_editableValueCellType) {
    StoreCell * storeCell = static_cast<StoreCell *>(cell);
    storeCell->setSeparatorLeft(i > 0);
  }
  if (typeAtLoc == k_notEditableValueCellType || typeAtLoc == k_editableValueCellType) {
    const int numberOfElementsInCol = numberOfElementsInColumn(i);
    EvenOddCell * eoCell = static_cast<EvenOddCell *>(cell);
    eoCell->setVisible(j <= numberOfElementsInCol + 1);
    if (j >= numberOfElementsInCol + 1) {
      static_cast<EvenOddCell *>(cell)->setEven(j%2 == 0);
      if (typeAtLoc == k_notEditableValueCellType) {
        static_cast<EvenOddExpressionCell *>(eoCell)->setLayout(Layout());
      } else {
        assert(typeAtLoc == k_editableValueCellType);
        static_cast<StoreCell *>(eoCell)->editableTextCell()->textField()->setText("");
      }
      return;
    }
  }
  Shared::ValuesController::willDisplayCellAtLocation(cell, i, j);
}

Layout ValuesController::functionTitleLayout(int columnIndex) {
  assert(typeAtLocation(columnIndex, 0) == k_functionTitleCellType);
  constexpr size_t bufferNameSize = ContinuousFunction::k_maxNameWithArgumentSize + 1;
  char buffer[bufferNameSize];
  bool isDerivative = false;
  Ion::Storage::Record record = recordAtColumn(columnIndex, &isDerivative);
  Shared::ExpiringPointer<ContinuousFunction> function = functionStore()->modelForRecord(record);
  if (isDerivative) {
    function->derivativeNameWithArgument(buffer, bufferNameSize);
    return StringLayout::Builder(buffer);
  }
  if (function->isNamed()) {
    function->nameWithArgument(buffer, bufferNameSize);
    return StringLayout::Builder(buffer);
  }
  return PoincareHelpers::CreateLayout(function->originalEquation(), textFieldDelegateApp()->localContext());
}


int ValuesController::fillColumnName(int columnIndex, char * buffer) {
  if (typeAtLocation(columnIndex, 0) == k_functionTitleCellType) {
    constexpr size_t bufferNameSize = ContinuousFunction::k_maxNameWithArgumentSize + 1;
    int size = functionTitleLayout(columnIndex).serializeParsedExpression(buffer, bufferNameSize, textFieldDelegateApp()->localContext());
    // Serialization may have introduced system parentheses.
    SerializationHelper::ReplaceSystemParenthesesByUserParentheses(buffer, bufferNameSize - 1);
    return size;
  }
  return Shared::ValuesController::fillColumnName(columnIndex, buffer);
}

void ValuesController::setTitleCellText(HighlightCell * cell, int columnIndex) {
  if (typeAtLocation(columnIndex,0) == k_functionTitleCellType) {
    ExpressionFunctionTitleCell * titleCell = static_cast<ExpressionFunctionTitleCell *>(cell);
    titleCell->setLayout(functionTitleLayout(columnIndex));
    return;
  }
  Shared::ValuesController::setTitleCellText(cell, columnIndex);
}

void ValuesController::setTitleCellStyle(HighlightCell * cell, int columnIndex) {
  if (typeAtLocation(columnIndex, 0)  == k_abscissaTitleCellType) {
    AbscissaTitleCell * myCell = static_cast<AbscissaTitleCell *>(cell);
    myCell->setSeparatorLeft(columnIndex > 0);
    return;
  }
  Shared::ValuesController::setTitleCellStyle(cell, columnIndex);
}

int ValuesController::typeAtLocation(int i, int j) {
  symbolTypeAtColumn(&i);
  return Shared::ValuesController::typeAtLocation(i, j);
}

void ValuesController::viewDidDisappear() {
  activateExactValues(false);
  Shared::ValuesController::viewDidDisappear();
}

// AlternateEmptyViewDelegate

bool ValuesController::isEmpty() const {
  if (functionStore()->numberOfActiveFunctionsInTable() == 0) {
    return true;
  }
  return false;
}

I18n::Message ValuesController::emptyMessage() {
  if (functionStore()->numberOfDefinedModels() == 0) {
    return I18n::Message::NoFunction;
  }
  return I18n::Message::NoActivatedFunction;
}

// Values controller

void ValuesController::setStartEndMessages(Shared::IntervalParameterController * controller, int column) {
  int c = column+1;
  m_intervalParameterSelectorController.setStartEndMessages(controller, symbolTypeAtColumn(&c));
}

void ValuesController::reloadEditedCell(int column, int row) {
  if (m_exactValuesAreActivated) {
    // Sizes might have changed
    selectableTableView()->reloadData();
    return;
  }
  Shared::ValuesController::reloadEditedCell(column, row);
}

// Memoization

void ValuesController::updateNumberOfColumns() const {
  for (size_t symbolTypeIndex = 0; symbolTypeIndex < k_maxNumberOfSymbolTypes; symbolTypeIndex++) {
    m_numberOfValuesColumnsForType[symbolTypeIndex] = 0;
  }
  int numberOfActiveFunctionsInTable = functionStore()->numberOfActiveFunctionsInTable();
  for (int i = 0; i < numberOfActiveFunctionsInTable; i++) {
    Ion::Storage::Record record = functionStore()->activeRecordInTableAtIndex(i);
    ExpiringPointer<ContinuousFunction> f = functionStore()->modelForRecord(record);
    int symbolTypeIndex = static_cast<int>(f->properties().symbolType());
    m_numberOfValuesColumnsForType[symbolTypeIndex] += numberOfColumnsForRecord(record);
  }
  m_numberOfColumns = 0;
  for (size_t symbolTypeIndex = 0; symbolTypeIndex < k_maxNumberOfSymbolTypes; symbolTypeIndex++) {
    // Count abscissa column if the sub table does exist
    m_numberOfColumns += numberOfColumnsForSymbolType(symbolTypeIndex);
  }
}

void ValuesController::updateSizeMemoizationForRow(int row, KDCoordinate rowPreviousHeight) {
  m_heightManager.updateMemoizationForIndex(row, rowPreviousHeight);
}

void ValuesController::updateSizeMemoizationForColumnAfterIndexChanged(int column, KDCoordinate columnPreviousWidth, int row) {
  // Update the size only if column becomes larger
  if (m_exactValuesAreActivated) {
    KDCoordinate minimalWidthForColumn = std::min(MaxColumnWidth(), CellSizeWithLayout(memoizedLayoutForCell(column, row)).width());
    if (columnPreviousWidth < minimalWidthForColumn) {
      m_widthManager.updateMemoizationForIndex(column, columnPreviousWidth, minimalWidthForColumn);
    }
  }
}

// Model getters

Ion::Storage::Record ValuesController::recordAtColumn(int i) {
  bool isDerivative = false;
  return recordAtColumn(i, &isDerivative);
}

Ion::Storage::Record ValuesController::recordAtColumn(int i, bool * isDerivative) {
  assert(typeAtLocation(i, 0) == k_functionTitleCellType);
  ContinuousFunctionProperties::SymbolType symbolType = symbolTypeAtColumn(&i);
  int index = 1;
  int numberOfActiveFunctionsInTableOfSymbolType = functionStore()->numberOfActiveFunctionsInTableOfSymbolType(symbolType);
  for (int k = 0; k < numberOfActiveFunctionsInTableOfSymbolType; k++) {
    Ion::Storage::Record record = functionStore()->activeRecordOfSymbolTypeInTableAtIndex(symbolType, k);
    const int numberOfColumnsForCurrentRecord = numberOfColumnsForRecord(record);
    if (index <= i && i < index + numberOfColumnsForCurrentRecord) {
      ExpiringPointer<ContinuousFunction> f = functionStore()->modelForRecord(record);
      *isDerivative = i != index && f->canDisplayDerivative();
      return record;
    }
    index += numberOfColumnsForCurrentRecord;
  }
  assert(false);
  return nullptr;
}

Shared::Interval * ValuesController::intervalAtColumn(int columnIndex) {
  return App::app()->intervalForSymbolType(symbolTypeAtColumn(&columnIndex));
}

Shared::ExpiringPointer<ContinuousFunction> ValuesController::functionAtIndex(int column, int row, double * abscissa, bool * isDerivative) {
  *abscissa = intervalAtColumn(column)->element(row-1); // Subtract the title row from row to get the element index
  Ion::Storage::Record record = recordAtColumn(column, isDerivative);
  return functionStore()->modelForRecord(record);
}

// Number of columns

int ValuesController::numberOfColumnsForAbscissaColumn(int column) {
  return numberOfColumnsForSymbolType((int)symbolTypeAtColumn(&column));
}

int ValuesController::numberOfColumnsForRecord(Ion::Storage::Record record) const {
  ExpiringPointer<ContinuousFunction> f = functionStore()->modelForRecord(record);
  return 1 + (f->properties().isCartesian() && f->displayDerivative());
}

int ValuesController::numberOfColumnsForSymbolType(int symbolTypeIndex) const {
  return m_numberOfValuesColumnsForType[symbolTypeIndex] + (m_numberOfValuesColumnsForType[symbolTypeIndex] > 0); // Count abscissa column if there is one
}

int ValuesController::numberOfAbscissaColumnsBeforeColumn(int column) const {
  int result = 0;
  int symbolType = column < 0 ?  k_maxNumberOfSymbolTypes : (int)symbolTypeAtColumn(&column) + 1;
  for (int symbolTypeIndex = 0; symbolTypeIndex < symbolType; symbolTypeIndex++) {
    result += (m_numberOfValuesColumnsForType[symbolTypeIndex] > 0);
  }
  return result;
}

int ValuesController::numberOfValuesColumns() const {
  return numberOfColumns() - numberOfAbscissaColumnsBeforeColumn(-1);
}

ContinuousFunctionProperties::SymbolType ValuesController::symbolTypeAtColumn(int * i) const {
  size_t symbolTypeIndex = 0;
  while (*i >= numberOfColumnsForSymbolType(symbolTypeIndex)) {
    *i -= numberOfColumnsForSymbolType(symbolTypeIndex++);
    assert(symbolTypeIndex < k_maxNumberOfSymbolTypes);
  }
  return static_cast<ContinuousFunctionProperties::SymbolType>(symbolTypeIndex);
}

// Function evaluation memoization

int ValuesController::valuesColumnForAbsoluteColumn(int column) {
  return column - numberOfAbscissaColumnsBeforeColumn(column);
}

int ValuesController::absoluteColumnForValuesColumn(int column) {
  int abscissaColumns = 0;
  int valuesColumns = 0;
  size_t symbolTypeIndex = 0;
  do {
    assert(symbolTypeIndex < k_maxNumberOfSymbolTypes);
    const int numberOfValuesColumnsForType = m_numberOfValuesColumnsForType[symbolTypeIndex++];
    valuesColumns += numberOfValuesColumnsForType;
    abscissaColumns += (numberOfValuesColumnsForType > 0);
  } while (valuesColumns <= column);
  return column + abscissaColumns;
}

void ValuesController::createMemoizedLayout(int column, int row, int index) {
  double abscissa;
  bool isDerivative = false;
  Shared::ExpiringPointer<ContinuousFunction> function = functionAtIndex(column, row, &abscissa, &isDerivative);
  Poincare::Context * context = textFieldDelegateApp()->localContext();
  // Compute exact result
  Expression exactResult = function->expressionReduced(context);
  Poincare::VariableContext abscissaContext = Poincare::VariableContext(Shared::Function::k_unknownName, context);
  Poincare::Expression abscissaExpression = Poincare::Decimal::Builder<double>(abscissa);
  abscissaContext.setExpressionForSymbolAbstract(abscissaExpression, Symbol::Builder(Shared::Function::k_unknownName, strlen(Shared::Function::k_unknownName)));
  PoincareHelpers::CloneAndSimplify(&exactResult, &abscissaContext, Poincare::ExpressionNode::ReductionTarget::User);
  if (m_exactValuesAreActivated && !isDerivative && !Utils::ShouldOnlyDisplayApproximation(function->originalEquation(), exactResult, context)) {
    // Do not show exact expressions in certain cases
    *memoizedLayoutAtIndex(index) = exactResult.createLayout(Poincare::Preferences::PrintFloatMode::Decimal, Poincare::Preferences::VeryLargeNumberOfSignificantDigits, context);
    return;
  }
  // Compute approximate result
  Expression approximation = PoincareHelpers::Approximate<double>(exactResult, context);
  *memoizedLayoutAtIndex(index) = approximation.createLayout(Preferences::PrintFloatMode::Decimal, Preferences::VeryLargeNumberOfSignificantDigits, context);

}

// Parameter controllers

template <class T>
T * ValuesController::parameterController() {
  bool isDerivative = false;
  Ion::Storage::Record record = recordAtColumn(selectedColumn(), &isDerivative);
  if (!functionStore()->modelForRecord(record)->properties().isCartesian()) {
    return nullptr;
  }
  if (isDerivative) {
    m_derivativeParameterController.setRecord(record);
    return &m_derivativeParameterController;
  }
  m_functionParameterController->setRecord(record);
  return m_functionParameterController;
}

I18n::Message ValuesController::valuesParameterMessageAtColumn(int columnIndex) const {
  return ContinuousFunctionProperties::MessageForSymbolType(symbolTypeAtColumn(&columnIndex));
}

// Cells & View
Shared::ExpressionFunctionTitleCell * ValuesController::functionTitleCells(int j) {
  assert(j >= 0 && j < k_maxNumberOfDisplayableFunctions);
  return &m_functionTitleCells[j];
}

EvenOddExpressionCell * ValuesController::valueCells(int j) {
  assert(j >= 0 && j < k_maxNumberOfDisplayableCells);
  return &m_valueCells[j];
}

 bool ValuesController::exactValuesButtonAction() {
  assert(m_exactValuesButton.state() == m_exactValuesAreActivated);
  /* Approximated values computation is much faster than exact values so it's
   * uninterruptable and if the exact values computation is interrupted, it
   * falls back on approximated values computation. */
  resetLayoutMemoization();

  if (m_exactValuesAreActivated) {
    activateExactValues(false);
    resetLayoutMemoization();
    m_selectableTableView.reloadData();
    return true;
  }

  {
    UserCircuitBreakerCheckpoint checkpoint;
    if (CircuitBreakerRun(checkpoint)) {
      activateExactValues(true);
      m_selectableTableView.reloadData();
      return true;
    } else {
      activateExactValues(false);
      resetLayoutMemoization();
      m_selectableTableView.reloadData();
      return false;
    }
  }
}

void ValuesController::activateExactValues(bool activate) {
  m_exactValuesAreActivated = activate;
  m_exactValuesButton.setState(m_exactValuesAreActivated);
}

}
