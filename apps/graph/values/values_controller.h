#ifndef GRAPH_VALUES_CONTROLLER_H
#define GRAPH_VALUES_CONTROLLER_H

#include <apps/graph/shared/derivative_column_parameter_controller.h>
#include <apps/graph/shared/function_parameter_controller.h>
#include <apps/shared/continuous_function_store.h>
#include <apps/shared/expression_function_title_cell.h>
#include <apps/shared/interval_parameter_controller.h>
#include <apps/shared/values_controller.h>
#include <escher/button_state.h>
#include <escher/even_odd_editable_text_cell.h>
#include <escher/even_odd_message_text_cell.h>
#include <escher/heavy_table_size_manager.h>
#include <escher/scrollable_two_layouts_cell.h>
#include <escher/toggleable_dot_view.h>
#include <omg/round.h>

#include "interval_parameter_selector_controller.h"

namespace Graph {

class ValuesController : public Shared::ValuesController,
                         public Escher::PrefacedTableViewDelegate,
                         public Escher::HeavyTableSizeManagerDelegate,
                         public ParameterDelegate {
 public:
  ValuesController(
      Escher::Responder* parentResponder, Escher::ButtonRowController* header,
      FunctionParameterController* functionParameterController,
      DerivativeColumnParameterController* derivativeColumnParameterController);
  bool displayButtonExactValues() const;

  // View controller
  void viewDidDisappear() override;

  // TableViewDataSource
  void fillCellForLocation(Escher::HighlightCell* cell, int column,
                           int row) override;
  int typeAtLocation(int column, int row) const override;
  KDCoordinate separatorBeforeColumn(int column) override;

  // ButtonRowDelegate
  int numberOfButtons(Escher::ButtonRowController::Position) const override {
    return isEmpty() ? 0 : 1 + displayButtonExactValues();
  }
  Escher::ButtonCell* buttonAtIndex(
      int index, Escher::ButtonRowController::Position position) const override;

  // AlternateEmptyViewDelegate
  bool isEmpty() const override {
    return functionStore()->numberOfActiveFunctionsInTable() == 0;
  }
  I18n::Message emptyMessage() override {
    return functionStore()->numberOfDefinedModels() == 0
               ? I18n::Message::NoFunction
               : I18n::Message::NoActivatedFunction;
  }

  // Shared::ValuesController
  Shared::IntervalParameterController* intervalParameterController() override {
    return &m_intervalParameterController;
  }
  IntervalParameterSelectorController* intervalParameterSelectorController() {
    return &m_intervalParameterSelectorController;
  }

  // PrefacedTableViewDelegate
  KDCoordinate maxRowPrefaceHeight() const override { return 3 * k_cellHeight; }
  int columnToFreeze() override;

  // ParameterDelegate
  bool usePersonalizedTitle() const override { return true; }
  bool showColorCell() const override { return false; }
  I18n::Message hideDerivativeMessage() const override {
    return I18n::Message::HideDerivativeColumn;
  }
  void hideDerivative(Ion::Storage::Record record,
                      int derivationOrder) override;

 private:
  constexpr static size_t k_maxNumberOfSymbolTypes =
      Shared::ContinuousFunctionProperties::k_numberOfSymbolTypes;
  constexpr static int k_maxNumberOfDisplayableSymbolTypes =
      OMG::CeilDivision(k_maxNumberOfDisplayableColumns, 2);
  constexpr static int k_maxNumberOfDisplayableAbscissaCells =
      k_maxNumberOfDisplayableSymbolTypes * k_maxNumberOfDisplayableRows;
  constexpr static KDCoordinate k_maxColumnWidth = 2 * k_cellWidth;
  constexpr static KDCoordinate k_maxRowHeight = 5 * k_cellHeight;
  static KDSize ApproximatedParametricCellSize();
  static KDSize CellSizeWithLayout(Poincare::Layout l);

  // TableViewDataSource
  /* Note: computing the total height and width of the table
   * when exact results are switched on is slow because all layouts
   * need to be computed. The speed optimization could come from either
   * a change of API or a change in the way scrollView/tableView work. */
  KDCoordinate nonMemoizedColumnWidth(int column) override {
    assert(false);
    return -1;
  }
  KDCoordinate nonMemoizedRowHeight(int row) override {
    assert(false);
    return -1;
  }
  Escher::TableSize1DManager* columnWidthManager() override {
    return m_tableSizeManager.columnWidthManager();
  }
  Escher::TableSize1DManager* rowHeightManager() override {
    return m_tableSizeManager.rowHeightManager();
  }
  void rowWasDeleted(int column, int row) override;

  // HeavyTableSizeManagerDelegate
  KDSize cellSizeAtLocation(int row, int col) override;
  KDSize maxCellSize() const override {
    return KDSize(k_maxColumnWidth, k_maxRowHeight);
  }

  // ColumnHelper
  size_t fillColumnName(int column, char* buffer) override;

  // EditableCellTableViewController
  void reloadEditedCell(int column, int row) override;
  void updateSizeMemoizationForRow(int row) override {
    m_tableSizeManager.rowDidChange(row);
  }
  void setTitleCellStyle(Escher::HighlightCell* titleCell, int column) override;

  // Shared::ValuesController
  Shared::ContinuousFunctionStore* functionStore() const override {
    return static_cast<Shared::ContinuousFunctionStore*>(
        Shared::ValuesController::functionStore());
  }
  Ion::Storage::Record recordAtColumn(int i) override;
  void updateNumberOfColumns() override;
  Poincare::Layout* memoizedLayoutAtIndex(int i) override;
  Poincare::Layout functionTitleLayout(int column) override;
  int numberOfAbscissaColumnsBeforeAbsoluteColumn(int column) const override;
  int numberOfAbscissaColumnsBeforeValuesColumn(int column) const override;
  void setStartEndMessages(Shared::IntervalParameterController* controller,
                           int column) override;
  void createMemoizedLayout(int column, int row, int index) override;
  int numberOfColumnsForAbscissaColumn(int column) override;
  void updateSizeMemoizationForColumnAfterIndexChanged(
      int column, KDCoordinate columnPreviousWidth, int changedRow) override;
  Shared::Interval* intervalAtColumn(int column) override;
  I18n::Message valuesParameterMessageAtColumn(int column) const override;
  Shared::ExpressionFunctionTitleCell* functionTitleCells(int j) override;
  Escher::EvenOddExpressionCell* valueCells(int j) override;
  int abscissaCellsCount() const override {
    return k_maxNumberOfDisplayableAbscissaCells;
  }
  Escher::AbstractEvenOddEditableTextCell* abscissaCells(int j) override;
  int abscissaTitleCellsCount() const override {
    return k_maxNumberOfDisplayableSymbolTypes;
  }
  Escher::EvenOddMessageTextCell* abscissaTitleCells(int j) override;
  Escher::SelectableViewController* functionParameterController() override {
    return parameterController<Escher::SelectableViewController>();
  }
  Shared::ColumnParameters* functionParameters() override {
    return parameterController<Shared::ColumnParameters>();
  }

  template <class T>
  T* parameterController();
  bool exactValuesButtonAction();
  void activateExactValues(bool activate);
  Ion::Storage::Record recordAtColumn(int i, int* derivationOrder);
  Shared::ExpiringPointer<Shared::ContinuousFunction> functionAtIndex(
      int column, int row, double* abscissa, int* derivationOrder);
  int numberOfColumnsForRecord(Ion::Storage::Record record) const;
  int numberOfColumnsForSymbolType(int symbolTypeIndex) const;
  Shared::ContinuousFunctionProperties::SymbolType symbolTypeAtColumn(
      int* column) const;
  bool cellHasValue(int column, int row, bool includeEmptyLastRow) const {
    return row <= numberOfElementsInColumn(column) + includeEmptyLastRow;
  }

  // + 1 for title row
  constexpr static int k_maxNumberOfRows =
      Shared::Interval::k_maxNumberOfElements + 1;
  // Only k_maxNumberOfMemoizedModels functions are computed
  constexpr static int k_maxNumberOfColumns =
      Shared::ContinuousFunctionProperties::k_numberOfVariableSymbolTypes +
      Shared::ContinuousFunctionStore::k_maxNumberOfMemoizedModels;
  Escher::HeavyTableSizeManager<k_maxNumberOfRows, k_maxNumberOfColumns>
      m_tableSizeManager;
  mutable int m_numberOfValuesColumnsForType[k_maxNumberOfSymbolTypes];
  Shared::ExpressionFunctionTitleCell
      m_functionTitleCells[k_maxNumberOfDisplayableColumns];
  Escher::EvenOddExpressionCell m_valueCells[k_maxNumberOfDisplayableCells];
  Escher::EvenOddMessageTextCell
      m_abscissaTitleCells[k_maxNumberOfDisplayableSymbolTypes];
  Escher::EvenOddEditableTextCell<
      Poincare::PrintFloat::k_maxNumberOfSignificantDigits>
      m_abscissaCells[k_maxNumberOfDisplayableAbscissaCells];
  FunctionParameterController* m_functionParameterController;
  Shared::IntervalParameterController m_intervalParameterController;
  IntervalParameterSelectorController m_intervalParameterSelectorController;
  DerivativeColumnParameterController* m_derivativeColumnParameterController;
  Escher::SimpleButtonCell m_setIntervalButton;
  Escher::ButtonState m_exactValuesButton;
  Escher::ToggleableDotView m_exactValuesDotView;
  bool m_exactValuesAreActivated;
  mutable Poincare::Layout m_memoizedLayouts[k_maxNumberOfDisplayableCells];
};

}  // namespace Graph

#endif
