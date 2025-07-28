#ifndef SHARED_STORE_CONTROLLER_H
#define SHARED_STORE_CONTROLLER_H

#include <escher/button_row_controller.h>
#include <escher/even_odd_editable_text_cell.h>
#include <escher/prefaced_table_view.h>
#include <escher/stack_view_controller.h>
#include <escher/tab_view_controller.h>

#include "buffer_function_title_cell.h"
#include "double_pair_store.h"
#include "editable_cell_table_view_controller.h"
#include "store_parameter_controller.h"

namespace Shared {

class StoreController : public EditableCellTableViewController,
                        public Escher::ButtonRowDelegate,
                        public StoreColumnHelper {
 public:
  StoreController(Escher::Responder* parentResponder, DoublePairStore* store,
                  Escher::ButtonRowController* header,
                  Poincare::Context* parentContext);

  // TextFieldDelegate
  bool textFieldDidFinishEditing(Escher::AbstractTextField* textField,
                                 Ion::Events::Event event) override;

  // TableViewDataSource
  int numberOfColumns() const override;
  Escher::HighlightCell* reusableCell(int index, int type) override;
  int reusableCellCount(int type) const override;
  int typeAtLocation(int column, int row) const override;
  void fillCellForLocation(Escher::HighlightCell* cell, int column,
                           int row) override;
  KDCoordinate separatorBeforeColumn(int column) const override;
  bool canStoreCellAtLocation(int column, int row) override { return row > 0; }

  // ViewController
  Escher::View* view() override { return &m_prefacedTableView; }
  void viewWillAppear() override;

  // Responder
  bool handleEvent(Ion::Events::Event event) override;

  // ClearColumnHelper
  size_t fillColumnName(int column, char* buffer) override {
    return fillColumnNameFromStore(column, buffer);
  }
  void setClearPopUpContent() override;

  void loadMemoizedFormulasFromSnapshot();

 protected:
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
  bool checkDataAtLocation(double floatBody, int column,
                           int row) const override;
  bool setDataAtLocation(double floatBody, int column, int row) override;
  double dataAtLocation(int column, int row) override;
  void setTitleCellText(Escher::HighlightCell* titleCell, int column) override;
  void setTitleCellStyle(Escher::HighlightCell* titleCell, int column) override;
  int numberOfElementsInColumn(int column) const override;

  Poincare::Layout memoizedFormula(int index) override {
    return m_memoizedFormulas[index];
  }
  void resetMemoizedFormulaOfColumn(int series, int column);
  void resetMemoizedFormulasOfEmptyColumns(int series);
  void memoizeFormula(const Poincare::Layout& formula, int index) override;

  Escher::PrefacedTableView m_prefacedTableView;
  DoublePairStore* m_store;
  Poincare::Layout
      m_memoizedFormulas[DoublePairStore::k_numberOfSeries *
                         DoublePairStore::k_numberOfColumnsPerSeries];

 protected:
  void handleResponderChainEvent(ResponderChainEvent event) override;

 private:
  bool cellAtLocationIsEditable(int column, int row) override;
  int maxNumberOfElements() const override {
    return DoublePairStore::k_maxNumberOfPairs;
  }
  void handleDeleteEvent(bool authorizeNonEmptyRowDeletion = true,
                         bool* didDeleteRow = nullptr);

  // TableViewDataSource
  Escher::TableSize1DManager* columnWidthManager() override {
    /* Do not implement a RegularTableSize1DManager for width since it does not
     * work with table separators. */
    return &m_widthManager;
  }
  Escher::TableSize1DManager* rowHeightManager() override {
    return &m_heightManager;
  }

  // StoreColumnHelper
  DoublePairStore* store() override { return m_store; }

  BufferFunctionTitleCell m_titleCells[k_maxNumberOfDisplayableColumns];
  Escher::EvenOddEditableTextCell<
      Escher::AbstractEvenOddBufferTextCell::k_defaultPrecision>
      m_editableCells[k_maxNumberOfDisplayableCells];

  Escher::MemoizedColumnWidthManager<6> m_widthManager;
  Escher::RegularTableSize1DManager m_heightManager;
};

}  // namespace Shared

#endif
