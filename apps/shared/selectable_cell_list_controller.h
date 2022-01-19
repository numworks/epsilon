#ifndef SHARED_SELECTABLE_CELL_LIST_CONTROLLER_H
#define SHARED_SELECTABLE_CELL_LIST_CONTROLLER_H

#include <escher/highlight_cell.h>
#include <escher/responder.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>

#include <apps/shared/page_controller.h>

namespace Shared {
/*
 * This Class is useful to create a SelectableListViewController of
 * the same type of cells.
 * Warning: by design it's the opposite of the memoization implemented in
 * MemoizedListViewDataSource (every cell is stored).
 */
template <typename Cell, int n>
class SelectableCellListPage : public SelectableListViewPage {
 public:
  SelectableCellListPage(Escher::StackViewController * parent, Escher::SelectableTableViewDelegate * tableDelegate = nullptr) : SelectableListViewPage(parent, tableDelegate) {}
  int numberOfRows() const override { return k_numberOfRows; }
  Escher::HighlightCell * reusableCell(int i, int type) override {
    assert(type == 0);
    assert(i >= 0 && i < k_numberOfRows);
    return &m_cells[i];
  }
  Cell * cellAtIndex(int i) { return &m_cells[i]; }

 protected:
  constexpr static int k_numberOfRows = n;
  Cell m_cells[n];
};

}  // namespace Shared

#endif /* SHARED_SELECTABLE_CELL_LIST_CONTROLLER_H */
