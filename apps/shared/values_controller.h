#ifndef SHARED_VALUES_CONTROLLER_H
#define SHARED_VALUES_CONTROLLER_H

#include "editable_cell_table_view_controller.h"
#include "editable_cell_selectable_table_view.h"
#include "expression_function_title_cell.h"
#include "function_store.h"
#include "interval.h"
#include "interval_parameter_controller.h"
#include "prefaced_twice_table_view.h"
#include "values_parameter_controller.h"
#include <apps/i18n.h>
#include <escher/alternate_empty_view_delegate.h>
#include <escher/button_row_controller.h>
#include <escher/even_odd_editable_text_cell.h>
#include <escher/even_odd_expression_cell.h>
#include <escher/even_odd_message_text_cell.h>

namespace Shared {

class ValuesController : public EditableCellTableViewController, public Escher::ButtonRowDelegate,  public Escher::AlternateEmptyViewDefaultDelegate {
public:
  ValuesController(Escher::Responder * parentResponder, Escher::ButtonRowController * header);

  // View controller
  Escher::View * view() override { return &m_prefacedTwiceTableView; }
  const char * title() override { return I18n::translate(I18n::Message::ValuesTab); }
  void viewWillAppear() override;
  void viewDidDisappear() override;
  TELEMETRY_ID("Values");

  // Responder
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void willExitResponderChain(Escher::Responder * nextFirstResponder) override;

  // TableViewDataSource
  int numberOfColumns() const override;
  void willDisplayCellAtLocation(Escher::HighlightCell * cell, int i, int j) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;

  // ButtonRowDelegate
  int numberOfButtons(Escher::ButtonRowController::Position) const override { return isEmpty() ? 0 : 1; }

  // AlternateEmptyViewDelegate
  Escher::Responder * defaultController() override { return tabController(); }

  // EditableCellTableViewController
  int numberOfRowsAtColumn(int i) const override;

  virtual IntervalParameterController * intervalParameterController() = 0;
  void initializeInterval();

protected:
  constexpr static int k_abscissaTitleCellType = 0;
  constexpr static int k_functionTitleCellType = 1;
  constexpr static int k_editableValueCellType = 2;
  constexpr static int k_notEditableValueCellType = 3; // Must be last for Graph::ValuesController
  constexpr static int k_maxNumberOfDisplayableRows = 10;
  constexpr static int k_numberOfTitleRows = 1;

  void initValueCells();

  // EditableCellTableViewController
  bool setDataAtLocation(double floatBody, int columnIndex, int rowIndex) override;
  void didChangeCell(int column, int row) override;
  int numberOfElementsInColumn(int columnIndex) const override;

  // Constructor helper
  void setupSelectableTableViewAndCells(Escher::InputEventHandlerDelegate * inputEventHandlerDelegate);

  // Parent controller getters
  Escher::StackViewController * stackController() const override;
  Responder * tabController() const override;

  // Model getters
  virtual FunctionStore * functionStore() const;
  virtual Ion::Storage::Record recordAtColumn(int i);

  // Number of columns memoization
  virtual void updateNumberOfColumns() const { m_numberOfColumns = numberOfAbscissaColumns() + functionStore()->numberOfActiveFunctions(); }
  mutable int m_numberOfColumns;
  mutable bool m_numberOfColumnsNeedUpdate;

  /* Function evaluation memoization
   * We memoize value cell layouts to increase scrolling speed. However
   * abscissa cells are not memoized (their computation does not require any
   * expression evaluation and is therefore not significantly long).
   * In the following, we refer to 3 different tables:
   * - the absolute table - the complete displayed table
   * - the table of values cells only (the absolute table from which we pruned
   *   the titles and the abscissa columns)
   * - the memoized table (which is a subset of the table of values cells)
   */
  void resetLayoutMemoization();
  virtual Poincare::Layout * memoizedLayoutAtIndex(int i) = 0;
  // Coordinates of memoizedLayoutForCell refer to the absolute table
  Poincare::Layout memoizedLayoutForCell(int i, int j);

  Escher::SelectableViewController * columnParameterController() override;
  Shared::ColumnParameters * columnParameters() override;

  virtual Poincare::Layout functionTitleLayout(int columnIndex) = 0;
  int fillColumnName(int columnIndex, char * buffer) override;
  void setTitleCellText(Escher::HighlightCell * titleCell, int columnIndex) override;
  void setTitleCellStyle(Escher::HighlightCell * titleCell, int columnIndex) override;
  void reloadEditedCell(int column, int row) override;

  virtual int numberOfAbscissaColumnsBeforeAbsoluteColumn(int column) const { return 1; }
  virtual int numberOfAbscissaColumnsBeforeValuesColumn(int column) const { return 1; }
  int numberOfAbscissaColumns() const { return numberOfAbscissaColumnsBeforeAbsoluteColumn(-1); }
  int numberOfValuesColumns() const { return numberOfColumns() - numberOfAbscissaColumns(); }

  Escher::SelectableTableView * selectableTableView() override { return &m_selectableTableView; }

  PrefacedTwiceTableView m_prefacedTwiceTableView;
  EditableCellSelectableTableView m_selectableTableView;

private:
  // Specialization depending on the abscissa names (x, n, t...)
  virtual void setStartEndMessages(Shared::IntervalParameterController * controller, int column) = 0;

  // EditableCellTableViewController
  bool cellAtLocationIsEditable(int columnIndex, int rowIndex) override;
  double dataAtLocation(int columnIndex, int rowIndex) override;
  int maxNumberOfElements() const override { return Interval::k_maxNumberOfElements; };

  /* Function evaluation memoization
   * The following 4 methods convert coordinate from the absolute table to the
   * table of values cell only and vice-versa. */
  int valuesColumnForAbsoluteColumn(int column);
  int valuesRowForAbsoluteRow(int row);
  int absoluteColumnForValuesColumn(int column);
  int absoluteRowForValuesRow(int row);
  /* Coordinates of createMemoizedLayout refer to the absolute table but the index
   * refers to the memoized table */
  virtual void createMemoizedLayout(int i, int j, int index) = 0;
  /* m_firstMemoizedColumn and m_firstMemoizedRow are coordinates of the table
   * of values cells.*/
  virtual int numberOfColumnsForAbscissaColumn(int column) { assert(column == 0); return numberOfColumns(); }
  mutable int m_firstMemoizedColumn;
  mutable int m_firstMemoizedRow;

  virtual void updateSizeMemoizationForColumnAfterIndexChanged(int column, KDCoordinate columnPreviousWidth, int changedRow) {}

  virtual Interval * intervalAtColumn(int columnIndex) = 0;
  virtual I18n::Message valuesParameterMessageAtColumn(int columnIndex) const = 0;
  virtual int maxNumberOfCells() = 0;
  virtual int maxNumberOfDisplayableFunctions() = 0;
  virtual ExpressionFunctionTitleCell * functionTitleCells(int j) = 0;
  virtual Escher::EvenOddExpressionCell * valueCells(int j) = 0;
  virtual int abscissaCellsCount() const = 0;
  virtual Escher::EvenOddEditableTextCell * abscissaCells(int j) = 0;
  virtual int abscissaTitleCellsCount() const = 0;
  virtual Escher::EvenOddMessageTextCell * abscissaTitleCells(int j) = 0;
  virtual Escher::SelectableViewController * functionParameterController() = 0;
  virtual Shared::ColumnParameters * functionParameters() = 0;

  void clearSelectedColumn() override;
  void setClearPopUpContent() override { m_confirmPopUpController.setMessageWithPlaceholders(I18n::Message::ClearTableConfirmation, ""); }
  bool isColumnClearable(int columnIndex) override { return typeAtLocation(columnIndex, 0) == k_abscissaTitleCellType; }

  ValuesParameterController m_abscissaParameterController;

};

}

#endif
