#ifndef SHARED_STORE_CONTROLLER_H
#define SHARED_STORE_CONTROLLER_H

#include <escher/button_row_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/tab_view_controller.h>

#include "double_pair_store.h"
#include "editable_cell_table_view_controller.h"
#include "input_event_handler_delegate.h"
#include "layout_field_delegate.h"
#include "prefaced_table_view.h"
#include "store_cell.h"
#include "store_parameter_controller.h"
#include "store_title_cell.h"

namespace Shared {

class StoreController : public EditableCellTableViewController,
                        public Escher::ButtonRowDelegate,
                        public StoreColumnHelper,
                        public Escher::SelectableListViewDelegate {
 public:
  StoreController(Escher::Responder* parentResponder,
                  Escher::InputEventHandlerDelegate* inputEventHandlerDelegate,
                  DoublePairStore* store, Escher::ButtonRowController* header,
                  Poincare::Context* parentContext);
  TELEMETRY_ID("Store");

  // TextFieldDelegate
  bool textFieldDidFinishEditing(Escher::AbstractTextField* textField,
                                 const char* text,
                                 Ion::Events::Event event) override;

  // TableViewDataSource
  int numberOfColumns() const override;
  Escher::HighlightCell* reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
  void willDisplayCellAtLocation(Escher::HighlightCell* cell, int i,
                                 int j) override;

  // ViewController
  Escher::View* view() override { return &m_prefacedTableView; }
  const char* title() override;

  // Responder
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

  // ClearColumnHelper
  int fillColumnName(int columnIndex, char* buffer) override {
    return fillColumnNameFromStore(columnIndex, buffer);
  }

  // EditableCellTableViewController
  int numberOfRowsAtColumn(int i) const override;

  // SelectableListViewDelegate
  bool canStoreContentOfCell(Escher::SelectableListView* t,
                             int row) const override {
    return row > 0;
  }

  void loadMemoizedFormulasFromSnapshot();

 protected:
  constexpr static int k_maxNumberOfDisplayableRows =
      Escher::Metric::MinimalNumberOfScrollableRowsToFillDisplayHeight(
          k_cellHeight, Escher::Metric::TabHeight);
  constexpr static int k_maxNumberOfDisplayableColumns =
      Ion::Display::Width / k_cellWidth + 2;
  constexpr static int k_maxNumberOfEditableCells =
      k_maxNumberOfDisplayableRows * k_maxNumberOfDisplayableColumns;
  constexpr static int k_numberOfTitleCells = 4;
  constexpr static int k_titleCellType = 0;
  constexpr static int k_editableCellType = 1;

  /* authorizeNonEmptyRowDeletion = false ensures that no other non-empty cell
   * will be deleted. Example: in Statistics::Store, when deleting a cell, the
   * whole row is deleted, except if you set authorizeNonEmptyRowDeletion =
   * false. */
  virtual bool deleteCellValue(int series, int col, int row,
                               bool authorizeNonEmptyRowDeletion = true);
  Escher::StackViewController* stackController() const override;
  Escher::TabViewController* tabController() const override;
  bool checkDataAtLocation(double floatBody, int columnIndex,
                           int rowIndex) const override;
  bool setDataAtLocation(double floatBody, int columnIndex,
                         int rowIndex) override;
  double dataAtLocation(int columnIndex, int rowIndex) override;
  void setTitleCellText(Escher::HighlightCell* titleCell,
                        int columnIndex) override;
  void setTitleCellStyle(Escher::HighlightCell* titleCell,
                         int columnIndex) override;
  int numberOfElementsInColumn(int columnIndex) const override;

  Poincare::Layout memoizedFormulaAtColumn(int column) override {
    return m_memoizedFormulaForColumn[column];
  }
  void resetMemoizedFormulasForSeries(int series);
  void memoizeFormulaAtColumn(Poincare::Layout formula, int column) override;

  PrefacedTableView m_prefacedTableView;
  StoreCell m_editableCells[k_maxNumberOfEditableCells];
  DoublePairStore* m_store;
  Poincare::Layout
      m_memoizedFormulaForColumn[DoublePairStore::k_numberOfSeries *
                                 DoublePairStore::k_numberOfColumnsPerSeries];

 private:
  bool cellAtLocationIsEditable(int columnIndex, int rowIndex) override;
  int maxNumberOfElements() const override {
    return DoublePairStore::k_maxNumberOfPairs;
  }
  void handleDeleteEvent(bool authorizeNonEmptyRowDeletion = true,
                         bool* didDeleteRow = nullptr);

  // TableViewDataSource
  Escher::TableSize1DManager* columnWidthManager() override {
    return &m_widthManager;
  }
  Escher::TableSize1DManager* rowHeightManager() override {
    return &m_heightManager;
  }

  // StoreColumnHelper
  DoublePairStore* store() override { return m_store; }

  StoreTitleCell m_titleCells[k_numberOfTitleCells];

  Escher::RegularTableSize1DManager m_widthManager;
  Escher::RegularTableSize1DManager m_heightManager;
};

}  // namespace Shared

#endif
