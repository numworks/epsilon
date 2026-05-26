#include "values_controller.h"

#include <apps/constant.h>
#include <apps/global_preferences.h>
#include <apps/shared/poincare_helpers.h>
#include <assert.h>
#include <escher/clipboard.h>
#include <omg/utf8_helper.h>
#include <poincare/cas.h>
#include <poincare/circuit_breaker_checkpoint.h>
#include <poincare/helpers/layout.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>

#include "../app.h"
#include "escher/app.h"

using namespace Shared;
using namespace Poincare;
using namespace Escher;

namespace Graph {

/* PUBLIC */

ValuesController::ValuesController(
    Responder* parentResponder, ButtonRowController* header,
    FunctionParameterController* functionParameterController,
    DerivativeColumnParameterController* derivativeColumnParameterController)
    : Shared::ValuesController(parentResponder, header),
      m_tableSizeManager(this),
      m_functionParameterController(functionParameterController),
      m_intervalParameterController(this),
      m_derivativeColumnParameterController(
          derivativeColumnParameterController),
      m_setIntervalButton(
          this, I18n::Message::IntervalSet,
          Invocation::Builder<ValuesController>(
              [](ValuesController* valuesController, void* sender) {
                StackViewController* stack =
                    ((StackViewController*)valuesController->stackController());
                IntervalParameterSelectorController*
                    intervalSelectorController =
                        valuesController->intervalParameterSelectorController();
                if (intervalSelectorController->numberOfRows() == 1) {
                  IntervalParameterController* intervalController =
                      valuesController->intervalParameterController();
                  intervalController->setInterval(
                      valuesController->intervalAtColumn(0));
                  int i = 1;
                  intervalSelectorController->setStartEndMessages(
                      intervalController,
                      valuesController->symbolTypeAtColumn(&i));
                  stack->push(intervalController);
                  return true;
                }
                stack->push(intervalSelectorController);
                return true;
              },
              this),
          k_cellFont),
      m_exactValuesButton(
          this, I18n::Message::ExactResults,
          Invocation::Builder<ValuesController>(
              [](ValuesController* valuesController, void* sender) {
                valuesController->exactValuesButtonAction();
                return true;
              },
              this),
          &m_exactValuesDotView, k_cellFont),
      m_exactValuesAreActivated(false) {
  m_prefacedTwiceTableView.setPrefaceDelegate(this);
  initValueCells();
  m_exactValuesButton.setState(m_exactValuesAreActivated);
  setupSelectableTableViewAndCells();
}

bool ValuesController::displayButtonExactValues() const {
  // When the store is full, the performances significantly drop.
  return !functionStore()->memoizationOverflows();
}

// ViewController

void ValuesController::viewDidDisappear() {
  activateExactValues(false);
  Shared::ValuesController::viewDidDisappear();
}

// TableViewDataSource

void ValuesController::fillCellForLocation(HighlightCell* cell, int column,
                                           int row) {
  // Handle hidden cells
  int type = typeAtLocation(column, row);
  if (type == k_notEditableValueCellType || type == k_editableValueCellType) {
    EvenOddCell* eoCell = static_cast<EvenOddCell*>(cell);
    eoCell->setVisible(cellHasValue(column, row, true));
    if (!cellHasValue(column, row, false)) {
      static_cast<EvenOddCell*>(cell)->setEven(row % 2 == 0);
      if (type == k_notEditableValueCellType) {
        static_cast<EvenOddExpressionCell*>(eoCell)->setLayout(Layout());
      } else {
        assert(type == k_editableValueCellType);
        static_cast<Escher::AbstractEvenOddEditableTextCell*>(eoCell)
            ->editableTextCell()
            ->textField()
            ->setText("");
      }
      return;
    }
  }
  Shared::ValuesController::fillCellForLocation(cell, column, row);
}

int ValuesController::typeAtLocation(int column, int row) const {
  symbolTypeAtColumn(&column);
  return Shared::ValuesController::typeAtLocation(column, row);
}

KDCoordinate ValuesController::separatorBeforeColumn(int column) const {
  return column > 0 && typeAtLocation(column, 0) == k_abscissaTitleCellType
             ? Escher::Metric::TableSeparatorThickness
             : 0;
}

void ValuesController::rowWasDeleted(int row, int column) {
  // Shift size memoization
  m_tableSizeManager.deleteRowMemoization(row);
  Shared::ValuesController::rowWasDeleted(row, column);
}

// ButtonRowDelegate

Escher::ButtonCell* ValuesController::buttonAtIndex(
    int index, Escher::ButtonRowController::Position position) const {
  return index == 0 && displayButtonExactValues()
             ? const_cast<Escher::ButtonState*>(&m_exactValuesButton)
             : const_cast<Escher::SimpleButtonCell*>(&m_setIntervalButton);
}

// PrefacedTableViewDelegate

int ValuesController::columnToFreeze() {
  assert(numberOfColumns() > 0 && numberOfAbscissaColumns() > 0);
  if (selectedRow() == -1) {
    return -1;
  }
  int column = selectedColumn();
  while (typeAtLocation(column, 0) != k_abscissaTitleCellType) {
    column--;
  }
  assert(column >= 0);
  return column;
}

// ParameterDelegate

void ValuesController::hideDerivative(Ion::Storage::Record record) {
  selectCellAtLocation(selectedColumn() - 1, selectedRow());
  ParameterDelegate::hideDerivative(record);
  stackController()->pop();
}

/* PRIVATE */

KDSize ValuesController::ApproximatedParametricCellSize() {
  KDSize layoutSize = LayoutHelpers::Point2DSizeGivenChildSize(
      KDSize(PrintFloat::glyphLengthForFloatWithPrecision(
                 SharedGlobalPrefs().numberOfSignificantDigits()) *
                 KDFont::GlyphWidth(k_cellFont),
             KDFont::GlyphHeight(k_cellFont)));
  return layoutSize + KDSize(Metric::SmallCellMargin * 2, 0);
}

KDSize ValuesController::CellSizeWithLayout(const Layout& l) {
  EvenOddTableExpressionCell tempCell;
  tempCell.setFont(k_cellFont);
  tempCell.setLayout(l);
  bool addVerticalMargin =
      !static_cast<const Layout&>(l).tree()->child(0)->isPoint2DLayout();
  return tempCell.minimalSizeForOptimalDisplay() +
         KDSize(Metric::SmallCellMargin * 2,
                addVerticalMargin * Metric::SmallCellMargin * 2);
}

// HeavyTableSizeManagerDelegate

KDSize ValuesController::cellSizeAtLocation(int row, int column) {
  if (row >= numberOfRows() || column >= numberOfColumns()) {
    return KDSize(TableSize1DManager::k_undefinedSize,
                  TableSize1DManager::k_undefinedSize);
  }

  KDCoordinate columnWidth = Shared::ValuesController::defaultColumnWidth();
  KDCoordinate rowHeight = Shared::ValuesController::defaultRowHeight();

  // Special case for parametric functions
  int tempCol = column;
  ContinuousFunctionProperties::SymbolType symbol =
      symbolTypeAtColumn(&tempCol);
  if (!m_exactValuesAreActivated && tempCol > 0 &&
      symbol == ContinuousFunctionProperties::SymbolType::T) {
    KDSize parametricSize = ApproximatedParametricCellSize();
    columnWidth = parametricSize.width();
    if (cellHasValue(column, row, false) && row > 0) {
      rowHeight = parametricSize.height();
    }
  }

  KDSize size(columnWidth, rowHeight);
  if (typeAtLocation(column, row) == k_functionTitleCellType) {
    size = CellSizeWithLayout(functionTitleLayout(column));
  } else if (m_exactValuesAreActivated &&
             typeAtLocation(column, row) == k_notEditableValueCellType &&
             cellHasValue(column, row, false)) {
    // Size is constant when displaying approximations
    Layout l = memoizedLayoutForCell(column, row);
    assert(!l.isUninitialized());
    size = CellSizeWithLayout(l);
  }
  columnWidth = std::max(size.width(), columnWidth);
  rowHeight = std::max(size.height(), rowHeight);
  return KDSize(columnWidth + separatorBeforeColumn(column),
                rowHeight + separatorBeforeRow(row));
}

// ColumnHelper

size_t ValuesController::fillColumnName(int column, char* buffer) {
  if (typeAtLocation(column, 0) == k_functionTitleCellType) {
    Ion::Storage::Record record = recordAtColumn(column);
    OMG::ExpiringPointer<ContinuousFunction> function =
        functionStore()->modelForRecord(record);
    constexpr size_t bufferNameSize =
        ContinuousFunction::k_maxNameWithArgumentSize + 1;
    return function->nameWithArgument(buffer, bufferNameSize);
  }
  return Shared::ValuesController::fillColumnName(column, buffer);
}

// EditableCellTableViewController

void ValuesController::reloadEditedCell(int column, int row) {
  if (m_exactValuesAreActivated) {
    // Sizes might have changed
    selectableTableView()->reloadData(true, false);
    return;
  }
  Shared::ValuesController::reloadEditedCell(column, row);
}

void ValuesController::setTitleCellStyle(HighlightCell* cell, int column) {
  if (typeAtLocation(column, 0) != k_functionTitleCellType) {
    return;
  }
  OMG::ExpiringPointer<ContinuousFunction> function =
      functionStore()->modelForRecord(recordAtColumn(column));
  Escher::ThemeComponent color = function->color();
  if (function->isDerivative()) {
    Ion::Storage::Record parentRecord = function->parentRecord();
    if (!function->isActiveInPlot()) {
      /* If the function is not active in plot, its color is not visible. Use
       * the parent's. */
      color = functionStore()->modelForRecord(parentRecord)->color();
    }
  }
  static_cast<FunctionTitleCell*>(cell)->setColor(color);
}

// Shared::ValuesController

Ion::Storage::Record ValuesController::recordAtColumn(int i) {
  assert(typeAtLocation(i, 0) == k_functionTitleCellType);
  ContinuousFunctionProperties::SymbolType symbolType = symbolTypeAtColumn(&i);
  Ion::Storage::Record record =
      functionStore()->activeRecordOfSymbolTypeInTableAtIndex(symbolType,
                                                              i - 1);
  assert(!record.isNull());
  return record;
}

void ValuesController::updateNumberOfColumns() {
  for (size_t symbolTypeIndex = 0; symbolTypeIndex < k_maxNumberOfSymbolTypes;
       symbolTypeIndex++) {
    m_numberOfValuesColumnsForType[symbolTypeIndex] = 0;
  }
  /* Value table has severe performances drops when the number of functions
   * overflows the memoization. */
  int numberOfDisplayedFunctions =
      std::min(Shared::ContinuousFunctionStore::k_maxNumberOfMemoizedModels,
               functionStore()->numberOfActiveFunctionsInTable());
  for (int i = 0; i < numberOfDisplayedFunctions; i++) {
    Ion::Storage::Record record =
        functionStore()->activeRecordInTableAtIndex(i);
    OMG::ExpiringPointer<ContinuousFunction> f =
        functionStore()->modelForRecord(record);
    int symbolTypeIndex = static_cast<int>(f->properties().symbolType());
    m_numberOfValuesColumnsForType[symbolTypeIndex]++;
  }
  m_numberOfColumns = 0;
  for (size_t symbolTypeIndex = 0; symbolTypeIndex < k_maxNumberOfSymbolTypes;
       symbolTypeIndex++) {
    // Count abscissa column if the sub table does exist
    m_numberOfColumns += numberOfColumnsForSymbolType(symbolTypeIndex);
  }
}

Layout* ValuesController::memoizedLayoutAtIndex(int i) {
  assert(i >= 0 && i < k_maxNumberOfDisplayableCells);
  return &m_memoizedLayouts[i];
}

Layout ValuesController::functionTitleLayout(int column) {
  assert(typeAtLocation(column, 0) == k_functionTitleCellType);
  Ion::Storage::Record record = recordAtColumn(column);
  OMG::ExpiringPointer<ContinuousFunction> function =
      functionStore()->modelForRecord(record);
  constexpr size_t bufferNameSize =
      ContinuousFunction::k_maxNameWithArgumentSize + 1;
  char buffer[bufferNameSize];
  if (!function->isNamed() && !function->isDerivative()) {
    return PoincareHelpers::CreateLayout(function->originalEquation(),
                                         App::app()->localContext());
  }
  function->nameWithArgument(buffer, bufferNameSize);
  return Layout::String(buffer);
}

int ValuesController::numberOfAbscissaColumnsBeforeAbsoluteColumn(
    int column) const {
  int abscissaColumns = 0;
  int symbolType = column < 0 ? k_maxNumberOfSymbolTypes
                              : (int)symbolTypeAtColumn(&column) + 1;
  for (int symbolTypeIndex = 0; symbolTypeIndex < symbolType;
       symbolTypeIndex++) {
    abscissaColumns += (m_numberOfValuesColumnsForType[symbolTypeIndex] > 0);
  }
  return abscissaColumns;
}

int ValuesController::numberOfAbscissaColumnsBeforeValuesColumn(
    int column) const {
  int abscissaColumns = 0;
  int valuesColumns = 0;
  size_t symbolTypeIndex = 0;
  while (valuesColumns <= column) {
    assert(symbolTypeIndex < k_maxNumberOfSymbolTypes);
    const int numberOfValuesColumnsForType =
        m_numberOfValuesColumnsForType[symbolTypeIndex++];
    valuesColumns += numberOfValuesColumnsForType;
    abscissaColumns += (numberOfValuesColumnsForType > 0);
  }
  return abscissaColumns;
}

void ValuesController::setStartEndMessages(
    Shared::IntervalParameterController* controller, int column) {
  m_intervalParameterSelectorController.setStartEndMessages(
      controller, symbolTypeAtColumn(&column));
}

void ValuesController::createMemoizedLayout(int column, int row, int index) {
  double abscissa;
  OMG::ExpiringPointer<ContinuousFunction> function =
      functionAtIndex(column, row, &abscissa);
  const SymbolContext& symbolContext = App::app()->localContext();
  UserExpression result;
  {
    // Compute exact result
    SystemExpression abscissaExpression =
        SystemExpression::DecimalBuilderFromDouble(abscissa);
    bool simplificationFailure = false;
    /* Defined symbols should have been replaced at this point. Remaining ones
     * are undefined and will be replaced as such. */
    SystemExpression e =
        function->expressionReduced().cloneAndReplaceSymbolWithExpression(
            Shared::Function::k_unknownName, abscissaExpression,
            &simplificationFailure,
            SymbolicComputation::ReplaceAllSymbolsWithUndefined);
    UserExpression approximation;
    e.cloneAndBeautifyAndApproximate(
        &result, &approximation,
        Poincare::BeautificationParameters{
            .user =
                {
                    .complexFormat = SharedGlobalPrefs().complexFormat(),
                    .angleUnit = SharedGlobalPrefs().angleUnit(),
                    .unitFormat = SharedGlobalPrefs().unitFormat(),
                    .context = symbolContext,
                }},
        ApproximationPolicy{.optimize = true});
    assert(!approximation.isUninitialized());
    /* Approximate in case of simplification failure, as we cannot display a
     * non-beautified expression. */
    if (simplificationFailure || !m_exactValuesAreActivated ||
        CAS::ShouldOnlyDisplayApproximation(function->originalEquation(),
                                            result, approximation,
                                            symbolContext)) {
      // Do not show exact expressions in certain cases, use approximate
      // result
      result = approximation;
    }
  }
  assert(!result.isUninitialized());
  Poincare::Preferences::PrintFloatMode floatDisplayMode =
      SharedGlobalPrefs().displayMode();
  uint8_t significantDigits = SharedGlobalPrefs().numberOfSignificantDigits();
  Layout layout =
      result.createLayout(floatDisplayMode, significantDigits, symbolContext);
  if (result.isPoint() && layout->layoutSize(k_cellFont).width() >
                              ApproximatedParametricCellSize().width() -
                                  2 * Metric::SmallCellMargin) {
    // Fallback on two rows point display if one row does not fit
    Poincare::Layout child0 = result.cloneChildAtIndex(0).createLayout(
        floatDisplayMode, significantDigits, symbolContext);
    Poincare::Layout child1 = result.cloneChildAtIndex(1).createLayout(
        floatDisplayMode, significantDigits, symbolContext);
    layout = Poincare::Layout::Create(KPoint2DL(KA, KB),
                                      {.KA = child0, .KB = child1});
  }
  *memoizedLayoutAtIndex(index) = layout;
}

int ValuesController::numberOfColumnsForAbscissaColumn(int column) {
  return numberOfColumnsForSymbolType((int)symbolTypeAtColumn(&column));
}

void ValuesController::updateSizeMemoizationForColumnAfterIndexChanged(
    int column, KDCoordinate columnPreviousWidth, int row) {
  // Update the size only if column becomes larger
  if (m_exactValuesAreActivated) {
    KDCoordinate minimalWidthForColumn = std::min(
        k_maxColumnWidth,
        CellSizeWithLayout(memoizedLayoutForCell(column, row)).width());
    if (columnPreviousWidth < minimalWidthForColumn) {
      m_tableSizeManager.columnDidChange(column);
    }
  }
}

Shared::Interval* ValuesController::intervalAtColumn(int column) {
  return App::app()->intervalForSymbolType(symbolTypeAtColumn(&column));
}

I18n::Message ValuesController::valuesParameterMessageAtColumn(
    int column) const {
  return ContinuousFunctionProperties::MessageForSymbolType(
      symbolTypeAtColumn(&column));
}

Shared::ExpressionFunctionTitleCell* ValuesController::functionTitleCells(
    int j) {
  assert(j >= 0 && j < k_maxNumberOfDisplayableColumns);
  return &m_functionTitleCells[j];
}

EvenOddTableExpressionCell* ValuesController::valueCells(int j) {
  assert(j >= 0 && j < k_maxNumberOfDisplayableCells);
  return &m_valueCells[j];
}

Escher::AbstractEvenOddEditableTextCell* ValuesController::abscissaCells(
    int j) {
  assert(j >= 0 && j < k_maxNumberOfDisplayableAbscissaCells);
  return &m_abscissaCells[j];
}

Escher::EvenOddTableMessageTextCell* ValuesController::abscissaTitleCells(
    int j) {
  assert(j >= 0 && j < abscissaTitleCellsCount());
  return &m_abscissaTitleCells[j];
}

// Graph::ValuesController

template <class T>
T* ValuesController::parameterController() {
  Ion::Storage::Record record = recordAtColumn(selectedColumn());
  OMG::ExpiringPointer<ContinuousFunction> function =
      functionStore()->modelForRecord(record);
  if (function->isDerivative()) {
    m_derivativeColumnParameterController->setRecord(record);
    m_derivativeColumnParameterController->setParameterDelegate(this);
    return m_derivativeColumnParameterController;
  }
  m_functionParameterController->setRecord(record);
  m_functionParameterController->setParameterDelegate(this);
  return m_functionParameterController;
}

bool ValuesController::exactValuesButtonAction() {
  assert(m_exactValuesButton.state() == m_exactValuesAreActivated);
  /* Approximated values computation is much faster than exact values so it's
   * uninterruptable and if the exact values computation is interrupted, it
   * falls back on approximated values computation. */
  resetLayoutMemoization();

  if (m_exactValuesAreActivated) {
    activateExactValues(false);
    m_selectableTableView.reloadData();
    return true;
  }

  {
    CircuitBreakerCheckpoint checkpoint(
        Ion::CircuitBreaker::CheckpointType::Back);
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

OMG::ExpiringPointer<ContinuousFunction> ValuesController::functionAtIndex(
    int column, int row, double* abscissa) {
  *abscissa = intervalAtColumn(column)->element(
      row - 1);  // Subtract the title row from row to get the element index
  Ion::Storage::Record record = recordAtColumn(column);
  return functionStore()->modelForRecord(record);
}

int ValuesController::numberOfColumnsForSymbolType(int symbolTypeIndex) const {
  return m_numberOfValuesColumnsForType[symbolTypeIndex] +
         (m_numberOfValuesColumnsForType[symbolTypeIndex] >
          0);  // Count abscissa column if there is one
}

ContinuousFunctionProperties::SymbolType ValuesController::symbolTypeAtColumn(
    int* column) const {
  // column becomes the index of the column in the sub table (of symbol type)
  size_t symbolTypeIndex = 0;
  while (*column >= numberOfColumnsForSymbolType(symbolTypeIndex)) {
    *column -= numberOfColumnsForSymbolType(symbolTypeIndex++);
    assert(symbolTypeIndex < k_maxNumberOfSymbolTypes);
  }
  return static_cast<ContinuousFunctionProperties::SymbolType>(symbolTypeIndex);
}

}  // namespace Graph
