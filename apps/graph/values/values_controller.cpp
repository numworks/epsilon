#include "values_controller.h"
#include <assert.h>
#include <escher/clipboard.h>
#include <poincare/circuit_breaker_checkpoint.h>
#include <poincare/decimal.h>
#include <poincare/layout_helper.h>
#include <poincare/matrix_layout.h>
#include <poincare/serialization_helper.h>
#include <poincare/string_layout.h>
#include "../../shared/poincare_helpers.h"
#include "../../shared/utils.h"
#include "../../constant.h"
#include "../app.h"

using namespace Shared;
using namespace Poincare;
using namespace Escher;

namespace Graph {

// Constructors

ValuesController::ValuesController(Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, ButtonRowController * header, FunctionColumnParameterController * functionParameterController) :
  Shared::ValuesController(parentResponder, header),
  m_selectableTableView(this),
  m_prefacedView(0, this, &m_selectableTableView, this, this),
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
  }, this), k_font),
  m_exactValuesButton(this, I18n::Message::ExactResults, Invocation([](void * context, void * sender) {
    ValuesController * valuesController = (ValuesController *) context;
    valuesController->exactValuesButtonAction();
    return true;
  }, this), &m_exactValuesDotView, KDFont::Size::Small),
  m_widthManager(this),
  m_heightManager(this)
{
  for (int i = 0; i < k_maxNumberOfDisplayableFunctions; i++) {
    m_functionTitleCells[i].setFont(KDFont::Size::Small);
  }
  initValueCells();
  m_exactValuesButton.setState(false);
  setupSelectableTableViewAndCells(inputEventHandlerDelegate);
}

// TableViewDataSource

KDSize ValuesController::CellSizeWithLayout(Layout l) {
  EvenOddExpressionCell tempCell;
  tempCell.setFont(KDFont::Size::Small);
  tempCell.setLayout(l);
  return tempCell.minimalSizeForOptimalDisplay() + KDSize(Metric::SmallCellMargin * 2, Metric::SmallCellMargin * 2);
}


KDCoordinate ValuesController::nonMemoizedColumnWidth(int i) {
  KDCoordinate columnWidth = Shared::ValuesController::defaultColumnWidth();
  KDCoordinate maxColumnWidth = MaxColumnWidth();
  if (!m_exactValuesButton.state()) {
    // Width is constant when displaying approximations
    return columnWidth;
  }
  int nRows = numberOfRows();
  for (int j = 0; j < nRows; j++) {
    if (typeAtLocation(i, j) == k_notEditableValueCellType && j < numberOfElementsInColumn(i) + 1) {
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
  if (j == 0) {
    // Title cell has constant height
    return rowHeight;
  }
  int nColumns = numberOfColumns();
  for (int i = 0; i < nColumns; i++) {
    int tempI = i;
    ContinuousFunction::SymbolType symbol = symbolTypeAtColumn(&tempI);
    if (!m_exactValuesButton.state() && symbol != ContinuousFunction::SymbolType::T) {
      /* Height is constant when there is no parametric function and exact
       * result is not displayed. */
      continue;
    }
    if (typeAtLocation(i, j) == k_notEditableValueCellType && j < numberOfElementsInColumn(i) + 1) {
      Layout l;
      bool shouldBreak = false;
      if (!m_exactValuesButton.state() && symbol == ContinuousFunction::SymbolType::T) {
        /* If the exact result is not displayed, every parametric function cell
         * has the same height, which is a 2-rows vector. No need to compute
         * the real layout to compute height, just use a placeholder vector */
        MatrixLayout m = MatrixLayout::Builder();
        m.addChildAtIndex(StringLayout::Builder(""), 0, 0, nullptr);
        m.addChildAtIndex(StringLayout::Builder(""), 1, 1, nullptr);
        m.setDimensions(2, 1);
        l = m;
        shouldBreak = true;
      } else {
        assert(m_exactValuesButton.state());
        l = memoizedLayoutForCell(i, j);
      }
      assert(!l.isUninitialized());
      rowHeight = std::max(CellSizeWithLayout(l).height(), rowHeight);
      if (rowHeight > maxRowHeight) {
        return maxRowHeight;
      }
      if (shouldBreak) {
        // When displaying approx, parametric functions have the max height.
        break;
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

int ValuesController::fillColumnName(int columnIndex, char * buffer) {
  if (typeAtLocation(columnIndex, 0) == k_functionTitleCellType) {
    const size_t bufferNameSize = ContinuousFunction::k_maxNameWithArgumentSize + 1;
    bool isDerivative = false;
    Ion::Storage::Record record = recordAtColumn(columnIndex, &isDerivative);
    Shared::ExpiringPointer<ContinuousFunction> function = functionStore()->modelForRecord(record);
    if (isDerivative) {
      return function->derivativeNameWithArgument(buffer, bufferNameSize);
    } else {
      if (function->isNamed()) {
        return function->nameWithArgument(buffer, bufferNameSize);
      } else {
        int size = PoincareHelpers::Serialize(function->originalEquation(), buffer, bufferNameSize, Preferences::VeryShortNumberOfSignificantDigits);
        // Serialization may have introduced system parentheses.
        SerializationHelper::ReplaceSystemParenthesesByUserParentheses(buffer, bufferNameSize - 1);
        return size;
      }
    }
  }
  return Shared::ValuesController::fillColumnName(columnIndex, buffer);
}

void ValuesController::setTitleCellText(HighlightCell * cell, int columnIndex) {
  if (typeAtLocation(columnIndex,0) == k_functionTitleCellType) {
    BufferFunctionTitleCell * myTitleCell = static_cast<BufferFunctionTitleCell *>(cell);
    fillColumnName(columnIndex, const_cast<char *>(myTitleCell->text()));
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
  m_exactValuesButton.setState(false);
  Shared::ValuesController::viewDidDisappear();
}

// SelectableTableViewDelegate

void ValuesController::tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) {
  if (withinTemporarySelection) {
    return;
  }
  const int i = selectedColumn();
  int j = selectedRow();
  const int numberOfElementsInCol = numberOfElementsInColumn(i);
  if (j > 1 + numberOfElementsInCol) {
    selectCellAtLocation(i, 1 + numberOfElementsInCol);
    j = 1 + numberOfElementsInCol;
  }
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
  if (m_exactValuesButton.state()) {
    // Sizes might have change
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
    int symbolTypeIndex = static_cast<int>(f->symbolType());
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

void ValuesController::deleteRowFromMemoization(int row, KDCoordinate rowPreviousHeight) {
  m_heightManager.deleteIndexFromMemoization(row, rowPreviousHeight);
}

void ValuesController::updateSizeMemoizationForColumnAfterIndexChanged(int column, KDCoordinate columnPreviousWidth, int row) {
  if (m_exactValuesButton.state()) {
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
  ContinuousFunction::SymbolType symbolType = symbolTypeAtColumn(&i);
  int index = 1;
  int numberOfActiveFunctionsOfSymbolType = functionStore()->numberOfActiveFunctionsOfSymbolType(symbolType);
  for (int k = 0; k < numberOfActiveFunctionsOfSymbolType; k++) {
    Ion::Storage::Record record = functionStore()->activeRecordOfSymbolTypeAtIndex(symbolType, k);
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
  return 1 +
    (f->isAlongXOrY() && f->displayDerivative());
}

int ValuesController::numberOfColumnsForSymbolType(int symbolTypeIndex) const {
  return m_numberOfValuesColumnsForType[symbolTypeIndex] + (m_numberOfValuesColumnsForType[symbolTypeIndex] > 0); // Count abscissa column if there is one
}

int ValuesController::numberOfAbscissaColumnsBeforeColumn(int column) {
  int result = 0;
  int symbolType = column < 0 ?  k_maxNumberOfSymbolTypes : (int)symbolTypeAtColumn(&column) + 1;
  for (int symbolTypeIndex = 0; symbolTypeIndex < symbolType; symbolTypeIndex++) {
    result += (m_numberOfValuesColumnsForType[symbolTypeIndex] > 0);
  }
  return result;
}

int ValuesController::numberOfValuesColumns() {
  return m_numberOfColumns - numberOfAbscissaColumnsBeforeColumn(-1);
}

ContinuousFunction::SymbolType ValuesController::symbolTypeAtColumn(int * i) const {
  size_t symbolTypeIndex = 0;
  while (*i >= numberOfColumnsForSymbolType(symbolTypeIndex)) {
    *i -= numberOfColumnsForSymbolType(symbolTypeIndex++);
    assert(symbolTypeIndex < k_maxNumberOfSymbolTypes);
  }
  return static_cast<ContinuousFunction::SymbolType>(symbolTypeIndex);
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
  if (!isDerivative && m_exactValuesButton.state()) {
    // Compute exact result
    Expression exactResult = function->expressionReduced(context);
    Poincare::VariableContext abscissaContext = Poincare::VariableContext(Shared::Function::k_unknownName, context);
    Poincare::Expression abscissaExpression = Poincare::Decimal::Builder<double>(abscissa);
    abscissaContext.setExpressionForSymbolAbstract(abscissaExpression, Symbol::Builder(Shared::Function::k_unknownName, strlen(Shared::Function::k_unknownName)));
    PoincareHelpers::CloneAndSimplify(&exactResult, &abscissaContext, Poincare::ExpressionNode::ReductionTarget::User);
    if (!Utils::ShouldOnlyDisplayApproximation(function->originalEquation(), exactResult, context)) {
      // Do not show exact expressions in certain cases
      *memoizedLayoutAtIndex(index) = exactResult.createLayout(Poincare::Preferences::PrintFloatMode::Decimal, Poincare::Preferences::VeryLargeNumberOfSignificantDigits, context);
      return;
    }
  }
  // Compute approximate result
  double evaluationX = NAN;
  double evaluationY = NAN;
  bool isParametric = function->symbolType() == ContinuousFunction::SymbolType::T;
  if (isDerivative) {
    assert(function->canDisplayDerivative());
    evaluationY = function->approximateDerivative(abscissa, context);
  } else {
    Poincare::Coordinate2D<double> eval = function->evaluate2DAtParameter(abscissa, context);
    evaluationY = eval.x2();
    if (isParametric) {
      evaluationX = eval.x1();
    }
  }
  Expression approximation;
  if (isParametric) {
    approximation = Matrix::Builder();
    static_cast<Matrix&>(approximation).addChildAtIndexInPlace(Float<double>::Builder(evaluationX), 0, 0);
    static_cast<Matrix&>(approximation).addChildAtIndexInPlace(Float<double>::Builder(evaluationY), 1, 1);
    static_cast<Matrix&>(approximation).setDimensions(2, 1);
  } else {
    approximation = Float<double>::Builder(evaluationY);
  }
  *memoizedLayoutAtIndex(index) = approximation.createLayout(Preferences::PrintFloatMode::Decimal, Preferences::VeryLargeNumberOfSignificantDigits, context);

}

// Parameter controllers

template <class T>
T * ValuesController::parameterController() {
  bool isDerivative = false;
  Ion::Storage::Record record = recordAtColumn(selectedColumn(), &isDerivative);
  if (!functionStore()->modelForRecord(record)->isAlongXOrY()) {
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
  return ContinuousFunction::MessageForSymbolType(symbolTypeAtColumn(&columnIndex));
}

// Cells & View
Shared::BufferFunctionTitleCell * ValuesController::functionTitleCells(int j) {
  assert(j >= 0 && j < k_maxNumberOfDisplayableFunctions);
  return &m_functionTitleCells[j];
}

EvenOddExpressionCell * ValuesController::valueCells(int j) {
  assert(j >= 0 && j < k_maxNumberOfDisplayableCells);
  return &m_valueCells[j];
}

 bool ValuesController::exactValuesButtonAction() {
  bool buttonWasOn = m_exactValuesButton.state();
  resetLayoutMemoization();
  {
    UserCircuitBreakerCheckpoint checkpoint;
    if (CircuitBreakerRun(checkpoint)) {
      m_exactValuesButton.setState(!buttonWasOn);
      m_selectableTableView.reloadData();
      return true;
    } else {
      m_exactValuesButton.setState(false);
      resetLayoutMemoization();
      m_selectableTableView.reloadData();
      return buttonWasOn;
    }
  }
}

/* ValuesController::ValuesSelectableTableView */

int writeMatrixBrackets(char * buffer, const int bufferSize, int type) {
  /* Write the double brackets required in matrix notation.
   * - type == 1: "[["
   * - type == 0: "]["
   * - type == -1: "]]"
   */
  int currentChar = 0;
  assert(currentChar < bufferSize-1);
  buffer[currentChar++] = type < 0 ? '[' : ']';
  assert(currentChar < bufferSize-1);
  buffer[currentChar++] = type <= 0 ? '[' : ']';
  return currentChar;
}

bool ValuesController::ValuesSelectableTableView::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Copy || event == Ion::Events::Cut) {
    HighlightCell * cell = selectedCell();
    if (cell) {
      const char * text = cell->text();
      if (text && text[0] == '(') {
        constexpr int bufferSize = 2*PrintFloat::k_maxFloatCharSize + 6; // "[[a][b]]" gives 6 characters in addition to the 2 floats
        char buffer[bufferSize];
        int currentChar = 0;
        currentChar += writeMatrixBrackets(buffer + currentChar, bufferSize - currentChar, -1);
        assert(currentChar < bufferSize-1);
        size_t semiColonPosition = UTF8Helper::CopyUntilCodePoint(buffer+currentChar, TextField::maxBufferSize() - currentChar, text+1, ';');
        currentChar += semiColonPosition;
        currentChar += writeMatrixBrackets(buffer + currentChar, bufferSize - currentChar, 0);
        assert(currentChar < bufferSize-1);
        currentChar += UTF8Helper::CopyUntilCodePoint(buffer+currentChar, TextField::maxBufferSize() - currentChar, text+1+semiColonPosition+1, ')');
        currentChar += writeMatrixBrackets(buffer + currentChar, bufferSize - currentChar, 1);
        assert(currentChar < bufferSize-1);
        buffer[currentChar] = 0;
        Clipboard::sharedClipboard()->store(buffer);
        return true;
      }
    }
  }
  return SelectableTableView::handleEvent(event);
}

}
