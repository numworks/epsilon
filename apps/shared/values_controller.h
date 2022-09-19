#ifndef SHARED_VALUES_CONTROLLER_H
#define SHARED_VALUES_CONTROLLER_H

#include "editable_cell_table_view_controller.h"
#include "function_store.h"
#include "function_title_cell.h"
#include "interval.h"
#include "interval_parameter_controller.h"
#include "prefaced_table_view.h"
#include "values_parameter_controller.h"
#include <apps/i18n.h>
#include <escher/alternate_empty_view_delegate.h>
#include <escher/button_row_controller.h>
#include <escher/even_odd_buffer_text_cell.h>
#include <escher/even_odd_editable_text_cell.h>
#include <escher/even_odd_message_text_cell.h>

namespace Shared {

class ValuesController : public EditableCellTableViewController, public Escher::ButtonRowDelegate,  public Escher::AlternateEmptyViewDefaultDelegate {
public:
  ValuesController(Escher::Responder * parentResponder, Escher::ButtonRowController * header);
  // View controller
  const char * title() override;
  Escher::View * view() override { return prefacedView(); }
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
  int numberOfButtons(Escher::ButtonRowController::Position) const override;

  // AlternateEmptyViewDelegate
  bool isEmpty() const override;
  Escher::Responder * defaultController() override;

  virtual IntervalParameterController * intervalParameterController() = 0;
  void initializeInterval();

protected:
  // The cellWidth is increased by 3 pixels to avoid displaying more than 4 columns on the screen (and thus decrease the number of memoized cell)
  constexpr static KDFont::Size k_font = KDFont::Size::Small;
  constexpr static KDCoordinate k_cellWidth = (Poincare::PrintFloat::glyphLengthForFloatWithPrecision(Poincare::Preferences::VeryLargeNumberOfSignificantDigits)) * KDFont::GlyphWidth(k_font) + 2*Escher::Metric::SmallCellMargin+3; // we add 10 to avoid displaying more that 4 columns and decr
  constexpr static int k_abscissaTitleCellType = 0;
  constexpr static int k_functionTitleCellType = 1;
  constexpr static int k_editableValueCellType = 2;
  constexpr static int k_notEditableValueCellType = 3; // Must be last for Graph::ValuesController
  constexpr static int k_maxNumberOfDisplayableRows = 10;

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
  virtual void updateNumberOfColumns() const;
  mutable int m_numberOfColumns;
  mutable bool m_numberOfColumnsNeedUpdate;

  /* Function evaluation memoization
   * We memoize value cell buffers in order to increase scrolling speed. However,
   * abscissa cells are not memoized (their computation does not require any
   * expression evaluation and is therefore not significantly long).
   * In the following, we refer to 3 different tables:
   * - the absolute table - the complete displayed table
   * - the table of values cells only (the absolute table from which we pruned
   *   the titles and the abscissa columns)
   * - the memoized table (which is a subset of the table of values cells)
   */
  void resetValuesMemoization();
  virtual char * memoizedBufferAtIndex(int i) = 0;
  virtual int numberOfMemoizedColumn() = 0;
  // Coordinates of memoizedBufferForCell refer to the absolute table
  char * memoizedBufferForCell(int i, int j);

  ColumnParameterController * columnParameterController() override;
  virtual PrefacedTableView * prefacedView() = 0;

  int fillColumnName(int columnIndex, char * buffer) override;
  void setTitleCellText(Escher::HighlightCell * titleCell, int columnIndex) override;
  void setTitleCellStyle(Escher::HighlightCell * titleCell, int columnIndex) override;
  void reloadEditedCell(int column, int row) override;

private:
  // Specialization depending on the abscissa names (x, n, t...)
  virtual void setStartEndMessages(Shared::IntervalParameterController * controller, int column) = 0;

  // EditableCellTableViewController
  bool cellAtLocationIsEditable(int columnIndex, int rowIndex) override;
  double dataAtLocation(int columnIndex, int rowIndex) override;
  virtual int numberOfValuesColumns() { return functionStore()->numberOfActiveFunctions(); }
  int maxNumberOfElements() const override {
    return Interval::k_maxNumberOfElements;
  };

  /* Function evaluation memoization
   * The following 4 methods convert coordinate from the absolute table to the
   * table of values cell only and vice-versa. */
  virtual int valuesColumnForAbsoluteColumn(int column) { return column - 1; } // Subtract the abscissa column
  int valuesRowForAbsoluteRow(int row) { return row - 1; } // Subtract the title row
  virtual int absoluteColumnForValuesColumn(int column) { return column + 1; } // Add the abscissa column
  int absoluteRowForValuesRow(int row) { return row + 1; } // Add the title row
  virtual int valuesCellBufferSize() const = 0;
  // Coordinates of fillMemoizedBuffer refer to the absolute table but the index
  // refers to the memoized table
  virtual void fillMemoizedBuffer(int i, int j, int index) = 0;
  /* m_firstMemoizedColumn and m_firstMemoizedRow are coordinates of the table
   * of values cells.*/
  virtual int numberOfColumnsForAbscissaColumn(int column) { assert(column == 0); return numberOfColumns(); }
  mutable int m_firstMemoizedColumn;
  mutable int m_firstMemoizedRow;

  virtual Interval * intervalAtColumn(int columnIndex) = 0;
  virtual I18n::Message valuesParameterMessageAtColumn(int columnIndex) const = 0;
  virtual int maxNumberOfCells() = 0;
  virtual int maxNumberOfFunctions() = 0;
  virtual FunctionTitleCell * functionTitleCells(int j) = 0;
  virtual Escher::EvenOddBufferTextCell * floatCells(int j) = 0;
  virtual int abscissaCellsCount() const = 0;
  virtual Escher::EvenOddEditableTextCell * abscissaCells(int j) = 0;
  virtual int abscissaTitleCellsCount() const = 0;
  virtual Escher::EvenOddMessageTextCell * abscissaTitleCells(int j) = 0;
  virtual ColumnParameterController * functionParameterController() = 0;

  void clearSelectedColumn() override;
  void setClearPopUpContent() override;
  bool isColumnClearable(int columnIndex) override { return typeAtLocation(columnIndex, 0) == k_abscissaTitleCellType; }

  ValuesParameterController m_abscissaParameterController;

};

}

#endif
