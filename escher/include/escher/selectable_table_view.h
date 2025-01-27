#ifndef ESCHER_SELECTABLE_TABLE_VIEW_H
#define ESCHER_SELECTABLE_TABLE_VIEW_H

#include <escher/palette.h>
#include <escher/responder.h>
#include <escher/selectable_table_view_data_source.h>
#include <escher/selectable_table_view_delegate.h>
#include <escher/table_view.h>
#include <escher/table_view_data_source.h>

namespace Escher {

/* SelectableTableView is a Table View that handles selection. To implement it,
 * it needs a class which should be both data source and view controller. This
 * takes the selectable table view as instance variable and makes it first
 * responder. The selectable table view bubbles up events when they do not
 * concern selection. */
class SelectableTableView : public TableView, public Responder {
 public:
  SelectableTableView(
      Responder* parentResponder, TableViewDataSource* dataSource,
      SelectableTableViewDataSource* selectionDataSource = nullptr,
      SelectableTableViewDelegate* delegate = nullptr,
      Escher::ScrollViewDelegate* scrollViewDelegate = nullptr);
  template <typename T>
  SelectableTableView(T* p) : SelectableTableView(p, p, p){};

  void setDelegate(SelectableTableViewDelegate* delegate) {
    m_delegate = delegate;
  }
  int selectedRow() const { return m_selectionDataSource->selectedRow(); }
  int selectedColumn() const { return m_selectionDataSource->selectedColumn(); }
  void selectRow(int j) { m_selectionDataSource->selectRow(j); }
  void selectColumn(int i) { m_selectionDataSource->selectColumn(i); }
  void selectFirstRow() {
    totalNumberOfRows() > 0 ? selectRow(0) : selectRow(-1);
  }

  // TODO: selectedCell should have a const an a non-const version
  HighlightCell* selectedCell();
  void selectCellAtLocation(int col, int row, bool setFirstResponder = true,
                            bool withinTemporarySelection = false);

  bool handleEvent(Ion::Events::Event event) override;
  void unhighlightSelectedCell();
  void deselectTable(bool withinTemporarySelection = false);
  void reloadData(bool setFirstResponder = true, bool resetMemoization = true);

  void didBecomeFirstResponder() override;
  void handleResponderChainEvent(ResponderChainEvent event) override;

 protected:
  void layoutSubviews(bool force = false) override;
  SelectableTableViewDataSource* m_selectionDataSource;
  SelectableTableViewDelegate* m_delegate;

 private:
  bool canSelectCellAtLocation(int column, int row);
  /* This function searches (for delta = n) :
   * - The n-th next selectable row in the currentCol starting from currentRow
   *   if searchForRow = true
   * - The n-th next selectable col in the currentRow starting from currentCol
   *   if searchForRow = false
   * If delta < 0, it searches backwards.
   * If the found index goes out of bounds, return the last valid index.
   *
   * Examples:
   * delta = 2, searchForRow = false (only currentRow is displayed)
   * o = selectable, x = non selectable
   *
   * Col index: | 0 | 1 | 2 | 3 | 4 |
   * Example 1: | o | o | x | o | o | currentCol = 0 -> resultCol = 3
   * Example 2: | o | x | x | o | o | currentCol = 0 -> resultCol = 4
   * Example 3: | o | x | x | o | x | currentCol = 2 -> resultCol = 3
   * Example 4: | o | x | x | o | x | currentCol = 4 -> resultCol = 3
   * */
  void nextSelectableCellInDirection(int* col, int* row,
                                     OMG::Direction direction, int delta = 1);
  int lastSelectableIndexInDirection(int col, int row,
                                     OMG::Direction direction);
  int maxIndexInDirection(int col, int row, OMG::Direction direction);
  KDPoint offsetToRestoreAfterReload() const {
    return m_delegate ? m_delegate->offsetToRestoreAfterReload(this)
                      : contentOffset();
  }
  int numberOfRowsAtColumn(int column) const {
    return m_delegate ? m_delegate->numberOfRowsAtColumn(this, column)
                      : totalNumberOfRows();
  }
};

}  // namespace Escher
#endif
