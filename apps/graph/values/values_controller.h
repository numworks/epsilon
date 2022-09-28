#ifndef GRAPH_VALUES_CONTROLLER_H
#define GRAPH_VALUES_CONTROLLER_H

#include "../continuous_function_store.h"
#include "../../shared/buffer_function_title_cell.h"
#include "../../shared/interval_parameter_controller.h"
#include "../../shared/scrollable_two_expressions_cell.h"
#include "../../shared/store_cell.h"
#include "../../shared/values_controller.h"
#include "abscissa_title_cell.h"
#include "derivative_parameter_controller.h"
#include "function_column_parameter_controller.h"
#include "interval_parameter_selector_controller.h"
#include <escher/button_state.h>
#include <escher/toggleable_dot_view.h>

namespace Graph {

class ValuesController : public Shared::ValuesController, public Escher::SelectableTableViewDelegate {
public:
  ValuesController(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Escher::ButtonRowController * header, FunctionColumnParameterController * functionParameterController);

  // View controller
  void willDisplayCellAtLocation(Escher::HighlightCell * cell, int i, int j) override;
  int typeAtLocation(int i, int j) override;
  void viewDidDisappear() override;

  // SelectableTableViewDelegate
  void tableViewDidChangeSelection(Escher::SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection = false) override;

  // ButtonRowDelegate
  int numberOfButtons(Escher::ButtonRowController::Position) const override { return isEmpty() ? 0 : 2; }
  Escher::AbstractButtonCell * buttonAtIndex(int index, Escher::ButtonRowController::Position position) const override {
    return index == 0 ? const_cast<Escher::ButtonState *>(&m_exactValuesButton) : const_cast<Escher::AbstractButtonCell *>(&m_setIntervalButton);
  }

  // AlternateEmptyViewDelegate
  bool isEmpty() const override;
  I18n::Message emptyMessage() override;

  // Parameters controllers getters
  Shared::IntervalParameterController * intervalParameterController() override {
    return &m_intervalParameterController;
  }
  IntervalParameterSelectorController * intervalParameterSelectorController() {
    return &m_intervalParameterSelectorController;
  }

private:
  constexpr static size_t k_maxNumberOfSymbolTypes = Shared::ContinuousFunction::k_numberOfSymbolTypes;
  constexpr static int k_maxNumberOfDisplayableFunctions = 4;
  constexpr static int k_maxNumberOfDisplayableSymbolTypes = 2;
  constexpr static int k_maxNumberOfDisplayableAbscissaCells = k_maxNumberOfDisplayableSymbolTypes * k_maxNumberOfDisplayableRows;
  constexpr static int k_maxNumberOfDisplayableCells = k_maxNumberOfDisplayableFunctions * k_maxNumberOfDisplayableRows;

  static KDSize CellSizeWithLayout(Poincare::Layout l);
  // TableViewDataSource
  KDCoordinate nonMemoizedColumnWidth(int i) override;
  KDCoordinate nonMemoizedRowHeight(int j) override;
  Escher::TableSize1DManager * columnWidthManager() override { return &m_widthManager; }
  Escher::TableSize1DManager * rowHeightManager() override { return &m_heightManager; }

  // Values controller
  void setStartEndMessages(Shared::IntervalParameterController * controller, int column) override;
  int maxNumberOfCells() override { return k_maxNumberOfDisplayableCells; }
  int maxNumberOfFunctions() override { return k_maxNumberOfDisplayableFunctions; }
  Shared::PrefacedTableView * prefacedView() override { return &m_prefacedView; }
  void reloadEditedCell(int column, int row) override;

  // Memoization
  void updateNumberOfColumns() const override;
  void updateSizeMemoizationForRow(int row, KDCoordinate rowPreviousHeight) override;
  void deleteRowFromMemoization(int row, KDCoordinate rowPreviousHeight) override;
  void updateSizeMemoizationForColumnAfterIndexChanged(int column, KDCoordinate columnPreviousWidth, int changedRow) override;

  // Model getters
  Ion::Storage::Record recordAtColumn(int i) override;
  Ion::Storage::Record recordAtColumn(int i, bool * isDerivative);
  ContinuousFunctionStore * functionStore() const override { return static_cast<ContinuousFunctionStore *>(Shared::ValuesController::functionStore()); }
  Shared::Interval * intervalAtColumn(int columnIndex) override;
  Shared::ExpiringPointer<Shared::ContinuousFunction> functionAtIndex(int column, int row, double * abscissa, bool * isDerivative);

  // Number of columns
  int numberOfColumnsForAbscissaColumn(int column) override;
  int numberOfColumnsForRecord(Ion::Storage::Record record) const;
  int numberOfColumnsForSymbolType(int symbolTypeIndex) const;
  int numberOfAbscissaColumnsBeforeColumn(int column);
  int numberOfValuesColumns() override;
  Shared::ContinuousFunction::SymbolType symbolTypeAtColumn(int * i) const;

  // Function evaluation memoization
  constexpr static int k_valuesCellBufferSize = 2*Poincare::PrintFloat::charSizeForFloatsWithPrecision(Poincare::Preferences::VeryLargeNumberOfSignificantDigits)+3; // The largest buffer holds (-1.234567E-123;-1.234567E-123)
  Poincare::Layout * memoizedLayoutAtIndex(int i) override {
    assert(i >= 0 && i < k_maxNumberOfDisplayableCells);
    return &m_memoizedLayouts[i];
  }
  int numberOfMemoizedColumn() override { return k_maxNumberOfDisplayableFunctions; }
  /* The conversion of column coordinates from the absolute table to the table
   * on only values cell depends on the number of abscissa columns which depends
   * on the number of different plot types in the table. */
  int valuesColumnForAbsoluteColumn(int column) override;
  int absoluteColumnForValuesColumn(int column) override;
  void createMemoizedLayout(int column, int row, int index) override;

  // Parameter controllers
  template <class T> T * parameterController();
  Escher::SelectableViewController * functionParameterController() override { return parameterController<Escher::SelectableViewController>(); }
  Shared::ColumnParameters * functionParameters() override { return parameterController<Shared::ColumnParameters>(); }

  I18n::Message valuesParameterMessageAtColumn(int columnIndex) const override;
  /* The paramater i should be the column index and symbolTypeAtColumn changes
   * it to be the relative column index within the sub table. */

  //Column name and title cells
  int fillColumnName(int columnIndex, char * buffer) override;
  void setTitleCellText(Escher::HighlightCell * titleCell, int columnIndex) override;
  void setTitleCellStyle(Escher::HighlightCell * titleCell, int columnIndex) override;

  // Cells & View
  Shared::BufferFunctionTitleCell * functionTitleCells(int j) override;
  Escher::EvenOddExpressionCell * valueCells(int j) override;
  int abscissaCellsCount() const override { return k_maxNumberOfDisplayableAbscissaCells; }
  Escher::EvenOddEditableTextCell * abscissaCells(int j) override { assert (j >= 0 && j < k_maxNumberOfDisplayableAbscissaCells); return &m_abscissaCells[j]; }
  int abscissaTitleCellsCount() const override { return k_maxNumberOfDisplayableSymbolTypes; }
  Escher::EvenOddMessageTextCell * abscissaTitleCells(int j) override { assert (j >= 0 && j < abscissaTitleCellsCount()); return &m_abscissaTitleCells[j]; }
  Escher::SelectableTableView * selectableTableView() override { return &m_selectableTableView; }

  bool exactValuesButtonAction();

  /* For parametric function, we display the evaluation with the form "(1;2)".
   * This form is not parsable so when we store it into the clipboard, we want
   * to turn it into a parsable matrix "[[1][2]]". To do so, we use a child
   * class of SelectableTableView to override the behaviour of the responder
   *  when encountering a cut/copy events. */
  class ValuesSelectableTableView : public Escher::SelectableTableView {
  public:
    ValuesSelectableTableView(ValuesController * vc) : Escher::SelectableTableView(vc, vc, vc) {}
    bool handleEvent(Ion::Events::Event event) override;
  };

  ValuesSelectableTableView m_selectableTableView;
  Shared::PrefacedTableView m_prefacedView;
  mutable int m_numberOfValuesColumnsForType[k_maxNumberOfSymbolTypes];
  Shared::BufferFunctionTitleCell m_functionTitleCells[k_maxNumberOfDisplayableFunctions];
  Escher::EvenOddExpressionCell m_valueCells[k_maxNumberOfDisplayableCells];
  AbscissaTitleCell m_abscissaTitleCells[k_maxNumberOfDisplayableSymbolTypes];
  Shared::StoreCell m_abscissaCells[k_maxNumberOfDisplayableAbscissaCells];
  FunctionColumnParameterController * m_functionParameterController;
  Shared::IntervalParameterController m_intervalParameterController;
  IntervalParameterSelectorController m_intervalParameterSelectorController;
  DerivativeParameterController m_derivativeParameterController;
  Escher::AbstractButtonCell m_setIntervalButton;
  Escher::ButtonState m_exactValuesButton;
  Escher::ToggleableDotView m_exactValuesDotView;
  Escher::ShortMemoizedColumnWidthManager m_widthManager;
  Escher::LongMemoizedRowHeightManager m_heightManager;
  mutable Poincare::Layout m_memoizedLayouts[k_maxNumberOfDisplayableCells];
};

}

#endif
