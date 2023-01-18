#ifndef GRAPH_VALUES_CONTROLLER_H
#define GRAPH_VALUES_CONTROLLER_H

#include <apps/shared/continuous_function_store.h>
#include <apps/graph/shared/function_parameter_controller.h>
#include <apps/shared/expression_function_title_cell.h>
#include <apps/shared/interval_parameter_controller.h>
#include <apps/shared/scrollable_two_expressions_cell.h>
#include <apps/shared/store_cell.h>
#include <apps/shared/values_controller.h>
#include "abscissa_title_cell.h"
#include "derivative_parameter_controller.h"
#include "interval_parameter_selector_controller.h"
#include <escher/button_state.h>
#include <escher/toggleable_dot_view.h>

namespace Graph {

class ValuesController : public Shared::ValuesController, public Shared::PrefacedTableViewDelegate {
public:
  ValuesController(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Escher::ButtonRowController * header, FunctionParameterController * functionParameterController);
  bool displayButtonExactValues() const;

  // View controller
  void viewDidDisappear() override;

  // TableViewDataSource
  void willDisplayCellAtLocation(Escher::HighlightCell * cell, int i, int j) override;
  int typeAtLocation(int i, int j) override;

  // ButtonRowDelegate
  int numberOfButtons(Escher::ButtonRowController::Position) const override { return isEmpty() ? 0 : 1 + displayButtonExactValues(); }
  Escher::AbstractButtonCell * buttonAtIndex(int index, Escher::ButtonRowController::Position position) const override;

  // AlternateEmptyViewDelegate
  bool isEmpty() const override { return functionStore()->numberOfActiveFunctionsInTable() == 0; }
  I18n::Message emptyMessage() override { return functionStore()->numberOfDefinedModels() == 0 ? I18n::Message::NoFunction : I18n::Message::NoActivatedFunction; }

  // Shared::ValuesController
  Shared::IntervalParameterController * intervalParameterController() override { return &m_intervalParameterController; }
  IntervalParameterSelectorController * intervalParameterSelectorController() { return &m_intervalParameterSelectorController; }

  // PrefacedTableViewDelegate
  KDCoordinate maxRowPrefaceHeight() const override { return 3 * k_cellHeight; }
  int columnToFreeze() override;
  int firstFeezableColumn() const override { return 0; }

private:
  constexpr static size_t k_maxNumberOfSymbolTypes = Shared::ContinuousFunctionProperties::k_numberOfSymbolTypes;
  constexpr static int k_maxNumberOfDisplayableFunctions = 4;
  constexpr static int k_maxNumberOfDisplayableSymbolTypes = 2;
  constexpr static int k_maxNumberOfDisplayableAbscissaCells = k_maxNumberOfDisplayableSymbolTypes * k_maxNumberOfDisplayableRows;
  constexpr static int k_maxNumberOfDisplayableCells = k_maxNumberOfDisplayableFunctions * k_maxNumberOfDisplayableRows;
  constexpr static int k_valuesCellBufferSize = 2 * Poincare::PrintFloat::charSizeForFloatsWithPrecision(Poincare::Preferences::VeryLargeNumberOfSignificantDigits) + 3; // The largest buffer holds (-1.234567E-123;-1.234567E-123)
  constexpr static KDCoordinate k_maxColumnWidth = 2 * k_cellWidth;
  constexpr static KDCoordinate k_maxRowHeight = 5 * k_cellHeight;
  static KDSize ApproximatedParametricCellSize();
  static KDSize CellSizeWithLayout(Poincare::Layout l);

  // TableViewDataSource
  /* Note: computing the total height and width of the table
   * when exact results are switched on is slow because all layouts
   * need to be computed. The speed optimization could come from either
   * a change of API or a change in the way scrollView/tableView work. */
  KDCoordinate nonMemoizedColumnWidth(int i) override;
  KDCoordinate nonMemoizedRowHeight(int j) override;
  Escher::TableSize1DManager * columnWidthManager() override { return &m_widthManager; }
  Escher::TableSize1DManager * rowHeightManager() override { return &m_heightManager; }

  // ColumnHelper
  int fillColumnName(int columnIndex, char * buffer) override;

  // EditableCellTableViewController
  void reloadEditedCell(int column, int row) override;
  void updateSizeMemoizationForRow(int row, KDCoordinate rowPreviousHeight) override { m_heightManager.updateMemoizationForIndex(row, rowPreviousHeight); }
  void setTitleCellStyle(Escher::HighlightCell * titleCell, int columnIndex) override;

  // Shared::ValuesController
  Shared::ContinuousFunctionStore * functionStore() const override { return static_cast<Shared::ContinuousFunctionStore *>(Shared::ValuesController::functionStore()); }
  Ion::Storage::Record recordAtColumn(int i) override;
  void updateNumberOfColumns() const override;
  Poincare::Layout * memoizedLayoutAtIndex(int i) override;
  Poincare::Layout functionTitleLayout(int columnIndex, bool forceShortVersion = false) override;
  int numberOfAbscissaColumnsBeforeAbsoluteColumn(int column) const override;
  int numberOfAbscissaColumnsBeforeValuesColumn(int column) const override;
  void setStartEndMessages(Shared::IntervalParameterController * controller, int column) override;
  void createMemoizedLayout(int column, int row, int index) override;
  int numberOfColumnsForAbscissaColumn(int column) override;
  void updateSizeMemoizationForColumnAfterIndexChanged(int column, KDCoordinate columnPreviousWidth, int changedRow) override;
  Shared::Interval * intervalAtColumn(int columnIndex) override;
  I18n::Message valuesParameterMessageAtColumn(int columnIndex) const override;
  int maxNumberOfCells() override { return k_maxNumberOfDisplayableCells; }
  int maxNumberOfDisplayableFunctions() override { return k_maxNumberOfDisplayableFunctions; }
  Shared::ExpressionFunctionTitleCell * functionTitleCells(int j) override;
  Escher::EvenOddExpressionCell * valueCells(int j) override;
  int abscissaCellsCount() const override { return k_maxNumberOfDisplayableAbscissaCells; }
  Escher::EvenOddEditableTextCell * abscissaCells(int j) override;
  int abscissaTitleCellsCount() const override { return k_maxNumberOfDisplayableSymbolTypes; }
  Escher::EvenOddMessageTextCell * abscissaTitleCells(int j) override;
  Escher::SelectableViewController * functionParameterController() override { return parameterController<Escher::SelectableViewController>(); }
  Shared::ColumnParameters * functionParameters() override { return parameterController<Shared::ColumnParameters>(); }

  template <class T> T * parameterController();
  bool exactValuesButtonAction();
  void activateExactValues(bool activate);
  Ion::Storage::Record recordAtColumn(int i, bool * isDerivative);
  Shared::ExpiringPointer<Shared::ContinuousFunction> functionAtIndex(int column, int row, double * abscissa, bool * isDerivative);
  int numberOfColumnsForRecord(Ion::Storage::Record record) const;
  int numberOfColumnsForSymbolType(int symbolTypeIndex) const;
  Shared::ContinuousFunctionProperties::SymbolType symbolTypeAtColumn(int * column) const;

  mutable int m_numberOfValuesColumnsForType[k_maxNumberOfSymbolTypes];
  Shared::ExpressionFunctionTitleCell m_functionTitleCells[k_maxNumberOfDisplayableFunctions];
  Escher::EvenOddExpressionCell m_valueCells[k_maxNumberOfDisplayableCells];
  AbscissaTitleCell m_abscissaTitleCells[k_maxNumberOfDisplayableSymbolTypes];
  Shared::StoreCell m_abscissaCells[k_maxNumberOfDisplayableAbscissaCells];
  FunctionParameterController * m_functionParameterController;
  Shared::IntervalParameterController m_intervalParameterController;
  IntervalParameterSelectorController m_intervalParameterSelectorController;
  DerivativeParameterController m_derivativeParameterController;
  Escher::AbstractButtonCell m_setIntervalButton;
  Escher::ButtonState m_exactValuesButton;
  Escher::ToggleableDotView m_exactValuesDotView;
  Escher::ShortMemoizedColumnWidthManager m_widthManager;
  Escher::LongMemoizedRowHeightManager m_heightManager;
  bool m_exactValuesAreActivated;
  mutable Poincare::Layout m_memoizedLayouts[k_maxNumberOfDisplayableCells];
};

}

#endif
