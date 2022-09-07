#ifndef SHARED_STORE_CONTROLLER_H
#define SHARED_STORE_CONTROLLER_H

#include <escher/button_row_controller.h>
#include <escher/stack_view_controller.h>
#include "double_pair_store.h"
#include "editable_cell_table_view_controller.h"
#include "layout_field_delegate.h"
#include "input_event_handler_delegate.h"
#include "layout_field_delegate.h"
#include "prefaced_table_view.h"
#include "store_cell.h"
#include "store_parameter_controller.h"
#include "store_selectable_table_view.h"
#include "store_title_cell.h"

namespace Shared {

class StoreController : public EditableCellTableViewController, public Escher::ButtonRowDelegate, public StoreColumnHelper {
public:
  StoreController(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, DoublePairStore * store, Escher::ButtonRowController * header, Poincare::Context * parentContext);
  Escher::View * view() override { return &m_prefacedView; }
  TELEMETRY_ID("Store");

  //TextFieldDelegate
  bool textFieldDidFinishEditing(Escher::AbstractTextField * textField, const char * text, Ion::Events::Event event) override;

  // TableViewDataSource
  int numberOfColumns() const override;
  KDCoordinate columnWidth(int i) override;
  KDCoordinate cumulatedWidthFromIndex(int i) override;
  int indexFromCumulatedWidth(KDCoordinate offsetX) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
  void willDisplayCellAtLocation(Escher::HighlightCell * cell, int i, int j) override;

  // ViewController
  const char * title() override;

  // Responder
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

  // ClearColumnHelper
  int fillColumnName(int columnIndex, char * buffer) override { return fillColumnNameFromStore(columnIndex, buffer); }

protected:
  constexpr static KDCoordinate k_cellWidth = Poincare::PrintFloat::glyphLengthForFloatWithPrecision(Poincare::Preferences::VeryLargeNumberOfSignificantDigits) * KDFont::GlyphWidth(KDFont::Size::Small) + 2*Escher::Metric::SmallCellMargin + Escher::Metric::TableSeparatorThickness;

  constexpr static int k_maxNumberOfDisplayableRows = Escher::Metric::MinimalNumberOfScrollableRowsToFillDisplayHeight(k_cellHeight, Escher::Metric::TabHeight);
  constexpr static int k_maxNumberOfDisplayableColumns = Ion::Display::Width/k_cellWidth + 2;
  constexpr static int k_maxNumberOfEditableCells = k_maxNumberOfDisplayableRows * k_maxNumberOfDisplayableColumns;
  constexpr static int k_numberOfTitleCells = 4;
  constexpr static int k_titleCellType = 0;
  constexpr static int k_editableCellType = 1;

  /* authorizeNonEmptyRowDeletion = false ensures that no other non-empty cell will be deleted.
   * Example: in Statistics::Store, when deleting a cell, the whole row is
   * deleted, except if you set authorizeNonEmptyRowDeletion = false. */
  virtual bool deleteCellValue(int series, int col, int row, bool authorizeNonEmptyRowDeletion = true);
  Escher::StackViewController * stackController() const override;
  Escher::Responder * tabController() const override;
  bool checkDataAtLocation(double floatBody, int columnIndex, int rowIndex) const override;
  bool setDataAtLocation(double floatBody, int columnIndex, int rowIndex) override;
  double dataAtLocation(int columnIndex, int rowIndex) override;
  void setTitleCellText(Escher::HighlightCell * titleCell, int columnIndex) override;
  void setTitleCellStyle(Escher::HighlightCell * titleCell, int columnIndex) override;
  int numberOfElementsInColumn(int columnIndex) const override;
  Escher::SelectableTableView * selectableTableView() override { return &m_dataView; }

  StoreCell m_editableCells[k_maxNumberOfEditableCells];
  DoublePairStore * m_store;

private:
  bool cellAtLocationIsEditable(int columnIndex, int rowIndex) override;
  int maxNumberOfElements() const override { return DoublePairStore::k_maxNumberOfPairs; }
  void handleDeleteEvent(bool authorizeNonEmptyRowDeletion = true, bool * didDeleteRow = nullptr);

  // StoreColumnHelper
  DoublePairStore * store() override { return m_store; }

  StoreTitleCell m_titleCells[k_numberOfTitleCells];
  PrefacedTableView m_prefacedView;
  StoreSelectableTableView m_dataView;

};

}

#endif
